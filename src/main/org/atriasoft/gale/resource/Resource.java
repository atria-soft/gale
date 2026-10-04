package org.atriasoft.gale.resource;

import org.atriasoft.etk.Uri;
import org.atriasoft.gale.context.GaleContext;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public abstract class Resource {
	static final Logger LOGGER = LoggerFactory.getLogger(Resource.class);
	protected static final String NO_NAME_RESOURCE = "---";
	protected static final int MAXRESOURCELEVEL = 5;
	private static int idGenerated = 10;
	
	/**
	 * Get the current resource Manager
	 */
	protected static ResourceManager getManager() {
		return GaleContext.getContext().getResourcesManager();
	}

	/**
	 * Get the living resource named {@code name}, kept for the caller (count of
	 * references + 1), as factories with a cache do.
	 * @param name Name of the resource.
	 * @param type Type the resource must have.
	 * @return the kept resource, or null if no living resource has this name.
	 * @throws IllegalStateException if the resource of this name has another type.
	 */
	protected static <T extends Resource> T keepExisting(final String name, final Class<T> type) {
		final Resource found = getManager().localKeepAndRetain(name);
		if (found == null) {
			return null;
		}
		if (type.isInstance(found)) {
			return type.cast(found);
		}
		found.release();
		throw new IllegalStateException("The resource '" + name + "' exists with the type " + found.getClass().getName()
				+ ", not " + type.getName());
	}

	/**
	 * For a constructor that fails after the registration done here: the
	 * half-built resource gives back its reference (count 1 to 0), so it leaves
	 * the manager and its name the cache; the constructor then throws.
	 */
	protected final void abandonConstruction() {
		release();
	}

	/**
	 * Run an OpenGL work (deletion of OpenGL objects) on the OpenGL thread: at
	 * once when the caller is on it, at the next update of the context otherwise.
	 * @param task The OpenGL work.
	 */
	protected static void runOnGlThread(final Runnable task) {
		getManager().runOnGlThread(task);
	}

	protected long uid = -1; //!< unique ID definition
	protected int count = 1;
	protected int resourceLevel = MAXRESOURCELEVEL - 1; //!< Level of the resource ==> for update priority [0..5] 0 must be update first.
	protected String name = NO_NAME_RESOURCE; //!< name of the resource ...
	/** Set by the manager (under its lock) while the resource is in its list. */
	boolean registered = false;
	/** Set by the manager when the count of references fell to 0: the resource is dead. */
	volatile boolean released = false;
	
	/**
	 * generic protected contructor (use factory to create this class)
	 */
	protected Resource() {
		this.uid = idGenerated++;
		getManager().localAdd(this);
	}
	
	protected Resource(final String name) {
		this.uid = idGenerated++;
		if (name == null) {
			this.name = "---";
		} else {
			this.name = name;
		}
		getManager().localAdd(this);
	}
	
	protected Resource(final Uri uri) {
		this.uid = idGenerated++;
		if (uri == null) {
			this.name = "---";
		} else {
			this.name = uri.toString();
		}
		getManager().localAdd(this);
	}
	
	public abstract void cleanUp();
	
	public int getCount() {
		return this.count;
	}
	
	public long getId() {
		return this.uid;
	}
	
	/**
	 * get the resource name
	 * @return The requested name
	 */
	public String getName() {
		return this.name;
	}
	
	/**
	 * Get the current resource level;
	 * @return value in [0..5]
	 */
	public int getResourceLevel() {
		return this.resourceLevel;
	}
	
	/**
	 * Whether the count of references fell to 0: the resource left the manager and
	 * its OpenGL objects are (or will be) deleted. It must not be used any more.
	 * @return true when released.
	 */
	public boolean isReleased() {
		return this.released;
	}
	
	/**
	 * One more user of this resource (count of references + 1).
	 */
	public void keep() {
		getManager().keep(this);
	}
	
	/**
	 * One user less (count of references - 1). At 0 the resource leaves the
	 * manager and {@link #cleanUp()} deletes its OpenGL objects: at once when the
	 * caller is on the OpenGL thread, at the next update of the context otherwise.
	 * Call it once per {@code create()} (or {@link #keep()}), from any thread.
	 */
	public void release() {
		getManager().release(this);
	}
	
	/**
	 * User request the reload of all resources (usefull when the file depend on DATA:GUI:xxx ...
	 */
	public void reload() {
		LOGGER.debug("Not set for: [{}]{} loaded ??? time(s)", getId(), getName());
	}
	
	/**
	 * The current OpenGl context is removing ==> remove yout own system data
	 */
	public void removeContext() {
		LOGGER.debug("Not set for: [{}]{} loaded ??? time(s)", getId(), getName());
	}
	
	/**
	 * The notification of the Context removing is too late, we have no more acces on the OpenGl context (thank you Android).
	 * Just update your internal state
	 */
	public void removeContextToLate() {
		LOGGER.debug("Not set for: [{}]{} loaded ??? time(s)", getId(), getName());
	}
	
	/**
	 * get the resource name
	 * @param name The name to set.
	 */
	public void setName(final String name) {
		this.name = name;
	}
	
	/**
	 * Call when need to send data on the harware (openGL)
	 * @note This is done asynchronously with the create of the Resource.
	 * @return true The context is updated
	 * @return false The context is not updated
	 */
	public boolean updateContext() {
		LOGGER.debug("Not set for: [{}]{} loaded ??? time(s)", getId(), getName());
		return true;
	}
}
