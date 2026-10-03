package org.atriasoft.gale.resource;

import java.util.ArrayList;
import java.util.List;

import org.atriasoft.etk.Uri;
import org.atriasoft.gale.backend3d.OpenGL;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/**
 * Registry and life cycle of the resources of a context.
 * <p>
 * A resource registers itself when it is built (count of references 1), is
 * shared by name through {@link #localKeep(String)} (the caller then calls
 * {@link Resource#keep()}), and leaves the registry when its count of
 * references falls to 0 ({@link Resource#release()}): its OpenGL objects are
 * deleted at once on the OpenGL thread, otherwise at the next
 * {@link #updateContext()}.
 * <p>
 * Thread safety: every method may be called from any thread. The lists are
 * guarded by one private lock, never held while a resource runs (upload,
 * clean-up), so a resource may create, keep or release other resources from
 * its callbacks. {@link #updateContext()} must be called by the thread that
 * owns the OpenGL context.
 */
public class ResourceManager {
	static final Logger LOGGER = LoggerFactory.getLogger(ResourceManager.class);
	private static final int MAX_RESOURCE_LEVEL = 9;
	/** Minimum time between two journal lines about the number of living resources. */
	private static final long COUNT_LOG_PERIOD_NS = 1_000_000_000L;

	/** Guards every list and the reference counts; never held while a resource callback runs. */
	private final Object lock = new Object();
	/** The living resources: registered and not released. */
	private final List<Resource> resourceList = new ArrayList<>();
	/** Resources to send to the graphic card at the next {@link #updateContext()}. */
	private final List<Resource> resourceListToUpdate = new ArrayList<>();
	/** Released resources whose OpenGL objects are deleted at the next {@link #updateContext()}. */
	private final List<Resource> resourceListToDestroy = new ArrayList<>();
	/** OpenGL work asked off the OpenGL thread, run at the next {@link #updateContext()}. */
	private final List<Runnable> glTasks = new ArrayList<>();
	private volatile boolean contextHasBeenRemoved = true;
	private volatile boolean exiting = false;
	private int lastLoggedCount = -1;
	private long lastCountLogTime = 0;

	/**
	 * initialize the internal variable
	 */
	public ResourceManager() {

	}

	/**
	 * special end of application
	 */
	public void applicationExiting() {
		contextHasBeenDestroyed();
		this.exiting = true;
	}

	/**
	 * Called by the context after each drawing: runs the pending OpenGL work and
	 * journals (DEBUG) the number of living resources when it changed.
	 */
	public void cleanInternalRemoved() {
		updateContext();
		logLivingCount();
	}

	/**
	 * This is to inform the resources manager that we have no more openGl context ...
	 */
	public void contextHasBeenDestroyed() {
		final List<Resource> living;
		final List<Resource> destroyed;
		synchronized (this.lock) {
			living = new ArrayList<>(this.resourceList);
			destroyed = new ArrayList<>(this.resourceListToDestroy);
			this.resourceListToDestroy.clear();
			// The OpenGL objects are gone with the context: nothing left to delete.
			this.glTasks.clear();
		}
		for (final Resource it : living) {
			it.removeContextToLate();
		}
		for (final Resource it : destroyed) {
			it.removeContextToLate();
		}
		// no context preent ...
		this.contextHasBeenRemoved = true;
	}

	/**
	 * display in the log all the resources loaded ...
	 */
	public void display() {
		LOGGER.info("Resources loaded : ");
		for (final Resource it : snapshot()) {
			LOGGER.info("    [{}]{}='{}' {} elements", it.getId(), it.getClass().getCanonicalName(), it.getName(), it.getCount());
		}
		LOGGER.info("Resources ---");
	}

	/**
	 * Number of living resources: registered and not released yet.
	 * @return the number of resources the manager holds.
	 */
	public int getResourceCount() {
		synchronized (this.lock) {
			return this.resourceList.size();
		}
	}

	/**
	 * Register a resource (done by the constructor of {@link Resource}). A
	 * resource registered already, or released, is not added again.
	 * @param object The resource to register.
	 */
	public void localAdd(final Resource object) {
		synchronized (this.lock) {
			if (object.registered || object.released) {
				return;
			}
			object.registered = true;
			this.resourceList.add(object);
		}
	}

	/**
	 * Find a living resource by its name. The reference count is NOT increased:
	 * a factory that hands the resource out calls {@link Resource#keep()}, or
	 * uses {@link #localKeepAndRetain(String)} that does both at once.
	 * @param filename Name of the resource.
	 * @return the resource, or null if no living resource has this name.
	 */
	public Resource localKeep(final String filename) {
		synchronized (this.lock) {
			return find(filename);
		}
	}

	public Resource localKeep(final Uri uri) {
		return localKeep(uri.toString());
	}

	/**
	 * Find a living resource by its name and keep it (reference count + 1), in one
	 * step: it cannot be released by another thread in between.
	 * @param filename Name of the resource.
	 * @return the resource, kept, or null if no living resource has this name.
	 */
	public Resource localKeepAndRetain(final String filename) {
		synchronized (this.lock) {
			final Resource out = find(filename);
			if (out != null) {
				out.count++;
			}
			return out;
		}
	}

	private Resource find(final String filename) {
		if (filename == null || filename.contentEquals(Resource.NO_NAME_RESOURCE)) {
			return null;
		}
		for (final Resource it : this.resourceList) {
			final String name = it.getName();
			if (name == null || name.contentEquals(Resource.NO_NAME_RESOURCE)) {
				continue;
			}
			if (name.contentEquals(filename)) {
				return it;
			}
		}
		return null;
	}

	/** Reference count + 1 (see {@link Resource#keep()}). */
	void keep(final Resource object) {
		synchronized (this.lock) {
			if (object.released) {
				LOGGER.error("keep() on the released resource [{}] '{}': it is not brought back", object.getId(), object.getName(),
						new IllegalStateException("resource already released"));
				return;
			}
			object.count++;
		}
	}

	/**
	 * Reference count - 1 (see {@link Resource#release()}); at 0 the resource leaves
	 * the manager and its OpenGL objects are deleted (at once on the OpenGL
	 * thread, at the next {@link #updateContext()} otherwise).
	 */
	void release(final Resource object) {
		synchronized (this.lock) {
			if (object.released) {
				LOGGER.warn("release() on the resource [{}] '{}' already released", object.getId(), object.getName(),
						new IllegalStateException("resource released twice"));
				return;
			}
			object.count--;
			if (object.count > 0) {
				return;
			}
			object.released = true;
			removeIdentity(this.resourceList, object);
			removeIdentity(this.resourceListToUpdate, object);
			object.registered = false;
			if (!OpenGL.hasContext()) {
				this.resourceListToDestroy.add(object);
				return;
			}
		}
		destroy(object);
	}

	private static void removeIdentity(final List<Resource> list, final Resource object) {
		for (int iii = list.size() - 1; iii >= 0; iii--) {
			if (list.get(iii) == object) {
				list.remove(iii);
			}
		}
	}

	private static void destroy(final Resource object) {
		LOGGER.trace("Destroy resource [{}] '{}'", object.getId(), object.getName());
		try {
			object.cleanUp();
		} catch (final RuntimeException ex) {
			LOGGER.error("Cannot clean up the resource [{}] '{}'", object.getId(), object.getName(), ex);
		}
	}

	/**
	 * Run an OpenGL work on the OpenGL thread: at once if the caller is on it,
	 * at the next {@link #updateContext()} otherwise. Used to delete OpenGL
	 * objects from a thread that has no context (UI callbacks).
	 * @param task The work to run; its exceptions are journaled.
	 */
	public void runOnGlThread(final Runnable task) {
		if (OpenGL.hasContext()) {
			runTask(task);
			return;
		}
		synchronized (this.lock) {
			this.glTasks.add(task);
		}
	}

	private static void runTask(final Runnable task) {
		try {
			task.run();
		} catch (final RuntimeException ex) {
			LOGGER.error("OpenGL task failed", ex);
		}
	}

	private void logLivingCount() {
		if (!LOGGER.isDebugEnabled()) {
			return;
		}
		final long now = System.nanoTime();
		final int count;
		synchronized (this.lock) {
			count = this.resourceList.size();
			if (count == this.lastLoggedCount || now - this.lastCountLogTime < COUNT_LOG_PERIOD_NS) {
				return;
			}
			this.lastLoggedCount = count;
			this.lastCountLogTime = now;
		}
		LOGGER.debug("{} resources alive", count);
	}

	/**
	 * Reload all resources from files, and send there in openGL card if needed.
	 * @note If file is reference at THEMEXXX:///filename if the Theme change the file will reload the newOne
	 */
	public void reLoadResources() {
		LOGGER.info("-------------  Resources re-loaded  -------------");
		final List<Resource> living = snapshot();
		for (long jjj = 0; jjj < ResourceManager.MAX_RESOURCE_LEVEL; jjj++) {
			LOGGER.info("    Reload level: {}/{}", jjj, ResourceManager.MAX_RESOURCE_LEVEL - 1);
			for (final Resource it : living) {
				if (jjj == it.getResourceLevel() && !it.isReleased()) {
					it.reload();
					LOGGER.info("        [{}]={}", it.getId(), it.getClass().getCanonicalName());
				}
			}
		}
		LOGGER.info("-------------  Resources  -------------");
	}

	private List<Resource> snapshot() {
		synchronized (this.lock) {
			return new ArrayList<>(this.resourceList);
		}
	}

	/**
	 * remove all resources (un-init) out of the destructor (due to the system implementation)
	 */
	public void unInit() {
		display();
		final List<Resource> living;
		synchronized (this.lock) {
			living = new ArrayList<>(this.resourceList);
			this.resourceListToUpdate.clear();
			this.resourceList.clear();
		}
		for (final Resource it : living) {
			LOGGER.warn("Find a resource that is not removed: [{}]='{}' {} elements", it.getId(), it.getName(), it.getCount());
		}
	}

	/**
	 * Call by the system to send all the needed data on the graphic card chen they change ...
	 * A released resource is not updated any more.
	 * @param object The resources that might be updated
	 */
	public void update(final Resource object) {
		synchronized (this.lock) {
			if (object.isReleased()) {
				LOGGER.debug("Update of the released resource [{}] '{}' ignored", object.getId(), object.getName());
				return;
			}
			for (final Resource it : this.resourceListToUpdate) {
				if (it == object) {
					// just prevent some double add ...
					return;
				}
			}
			this.resourceListToUpdate.add(object);
		}
	}

	/**
	 * Called by the OpenGL thread: deletes the OpenGL objects of the released
	 * resources, runs the OpenGL work asked from other threads, then sends to the
	 * graphic card the resources that changed (all of them after a context loss).
	 */
	public void updateContext() {
		if (this.exiting) {
			LOGGER.error("Request update after application EXIT ...");
			return;
		}
		// Deletions first, until none is left: the clean-up of a resource may release others (a program its shaders).
		while (true) {
			final List<Resource> toDestroy;
			final List<Runnable> tasks;
			synchronized (this.lock) {
				toDestroy = new ArrayList<>(this.resourceListToDestroy);
				this.resourceListToDestroy.clear();
				tasks = new ArrayList<>(this.glTasks);
				this.glTasks.clear();
			}
			if (toDestroy.isEmpty() && tasks.isEmpty()) {
				break;
			}
			for (final Resource it : toDestroy) {
				destroy(it);
			}
			for (final Runnable task : tasks) {
				runTask(task);
			}
		}
		final List<Resource> toUpdate;
		final boolean all = this.contextHasBeenRemoved;
		synchronized (this.lock) {
			if (all) {
				toUpdate = new ArrayList<>(this.resourceList);
			} else {
				toUpdate = new ArrayList<>(this.resourceListToUpdate);
			}
			this.resourceListToUpdate.clear();
		}
		this.contextHasBeenRemoved = false;
		if (toUpdate.isEmpty()) {
			return;
		}
		for (long jjj = 0; jjj < ResourceManager.MAX_RESOURCE_LEVEL; jjj++) {
			LOGGER.trace("    updateContext level ({}): {}/{}", all ? "D" : "U", jjj, ResourceManager.MAX_RESOURCE_LEVEL - 1);
			for (final Resource it : toUpdate) {
				// A resource released by an earlier one of this loop is not sent any more.
				if (jjj != it.getResourceLevel() || it.isReleased()) {
					continue;
				}
				if (!it.updateContext()) {
					// Lock error ==> postponned
					update(it);
				}
			}
		}
	}

}
