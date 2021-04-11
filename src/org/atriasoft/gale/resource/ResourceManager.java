package org.atriasoft.gale.resource;

import java.util.ArrayList;
import java.util.List;

import org.atriasoft.etk.Uri;
import org.atriasoft.gale.internal.Log;

public class ResourceManager {
	private static final int MAX_RESOURCE_LEVEL = 9;
	private final List<Resource> resourceList = new ArrayList<>();
	private List<Resource> resourceListToUpdate = new ArrayList<>();
	private boolean contextHasBeenRemoved = true;
	private boolean exiting = false;
	
	/**
	 * initialize the internal variable
	 */
	public ResourceManager() {
		
	}
	
	/**
	 * special end of application
	 */
	public synchronized void applicationExiting() {
		contextHasBeenDestroyed();
		this.exiting = true;
	}
	
	public synchronized void cleanInternalRemoved() {
		//Log.info("remove object in Manager");
		updateContext();
		// TODO ...
		//		for (auto it(this.resourceList.begin()); it!=this.resourceList.end(); ++it) {
		//			if ((*it).expired() == true) {
		//				this.resourceList.erase(it);
		//				it = this.resourceList.begin();
		//			}
		//		}
	}
	
	/**
	 * This is to inform the resources manager that we have no more openGl context ...
	 */
	public synchronized void contextHasBeenDestroyed() {
		for (final Resource it : this.resourceList) {
			if (it.getCount() > 0) {
				it.removeContextToLate();
			}
		}
		// no context preent ...
		this.contextHasBeenRemoved = true;
	}
	
	/**
	 * display in the log all the resources loaded ...
	 */
	public synchronized void display() {
		Log.info("Resources loaded : ");
		// remove all resources ...
		for (final Resource it : this.resourceList) {
			Log.info("    [" + it.getId() + "]" + it.getClass().getCanonicalName() + "='" + it.getName() + "' " + it.getCount() + " elements");
		}
		Log.info("Resources ---");
	}
	
	public synchronized void localAdd(final Resource object) {
		// add at the end if no slot is free
		this.resourceList.add(object);
	}
	
	// internal API to extent eResources in extern Soft
	public synchronized Resource localKeep(final String filename) {
		Log.verbose("KEEP (DEFAULT) : file : '" + filename + "' in " + this.resourceList.size() + " resources");
		for (final Resource it : this.resourceList) {
			if (it == null) {
				continue;
			}
			if (it.getName() == null) {
				continue;
			}
			//Log.verbose("compare : " + filename + " ==???== " + it.getName());
			if (it.getName().contentEquals(Resource.NO_NAME_RESOURCE)) {
				continue;
			}
			if (it.getName().contentEquals(filename)) {
				return it;
			}
		}
		return null;
	}
	
	public synchronized Resource localKeep(final Uri uri) {
		// TODO Auto-generated method stub
		return localKeep(uri.toString());
	}
	
	/**
	 * Reload all resources from files, and send there in openGL card if needed.
	 * @note If file is reference at THEMEXXX:///filename if the Theme change the file will reload the newOne
	 */
	public synchronized void reLoadResources() {
		Log.info("-------------  Resources re-loaded  -------------");
		// remove all resources ...
		for (long jjj = 0; jjj < MAX_RESOURCE_LEVEL; jjj++) {
			Log.info("    Reload level : " + jjj + "/" + (MAX_RESOURCE_LEVEL - 1));
			for (final Resource it : this.resourceList) {
				if (jjj == it.getResourceLevel()) {
					if (it.getCount() > 0) {
						it.reload();
						Log.info("        [" + it.getId() + "]=" + it.getClass().getCanonicalName());
					}
				}
			}
		}
		// TODO UNderstand why it is set here ...
		//gale::requestUpdateSize();
		Log.info("-------------  Resources  -------------");
	}
	
	/**
	 * Uninitiamize the resource manager, free all resources previously requested
	 * @note when not free  == > generate warning, because the segfault can appear after...
	 */
	//public ~Manager();
	/**
	 * remove all resources (un-init) out of the destructor (due to the system implementation)
	 */
	public synchronized void unInit() {
		display();
		this.resourceListToUpdate.clear();
		// remove all resources ...
		for (final Resource it : this.resourceList) {
			Log.warning("Find a resource that is not removed : [" + it.getId() + "]" + "='" + it.getName() + "' " + it.getCount() + " elements");
		}
		this.resourceList.clear();
	}
	
	/**
	 * Call by the system to send all the needed data on the graphic card chen they change ...
	 * @param object The resources that might be updated
	 */
	public void update(final Resource object) {
		synchronized (this.resourceListToUpdate) {
			// check if not added before
			for (final Resource it : this.resourceListToUpdate) {
				if (it == object) {
					// just prevent some double add ...
					return;
				}
			}
			// add it ...
			this.resourceListToUpdate.add(object);
		}
	}
	
	/**
	 * Call by the system chen the openGL Context has been unexpectially removed  == > This reload all the texture, VBO and other ....
	 */
	public void updateContext() {
		if (this.exiting) {
			Log.error("Request update after application EXIT ...");
			return;
		}
		// TODO Check the number of call this ... Log.info("update open-gl context ... ");
		if (this.contextHasBeenRemoved) {
			// need to update all ...
			this.contextHasBeenRemoved = false;
			synchronized (this.resourceListToUpdate) {
				this.resourceListToUpdate.clear();
			}
			synchronized (this.resourceList) {
				if (this.resourceList.size() != 0) {
					for (long jjj = 0; jjj < MAX_RESOURCE_LEVEL; jjj++) {
						Log.verbose("    updateContext level (D) : " + jjj + "/" + (MAX_RESOURCE_LEVEL - 1));
						for (final Resource it : this.resourceList) {
							if (jjj == it.getResourceLevel()) {
								//Log.debug("Update context named : " + lresourceList[iii].getName());
								if (!it.updateContext()) {
									// Lock error ==> postponned
									synchronized (this.resourceListToUpdate) {
										this.resourceListToUpdate.add(it);
									}
								}
							}
						}
					}
				}
			}
		} else {
			List<Resource> resourceListToUpdate = null;
			synchronized (this.resourceListToUpdate) {
				resourceListToUpdate = this.resourceListToUpdate;
				this.resourceListToUpdate = new ArrayList<>();
			}
			if (resourceListToUpdate.size() != 0) {
				for (long jjj = 0; jjj < MAX_RESOURCE_LEVEL; jjj++) {
					Log.verbose("    updateContext level (U) : " + jjj + "/" + (MAX_RESOURCE_LEVEL - 1));
					for (final Resource it : resourceListToUpdate) {
						if (jjj == it.getResourceLevel()) {
							if (!it.updateContext()) {
								// Lock error ==> postponned
								this.resourceListToUpdate.add(it);
							}
						}
					}
				}
			}
		}
	}
	
}
