package org.atriasoft.gale.resource;

import java.util.ArrayList;
import java.util.List;

import org.atriasoft.gale.internal.Log;

public class ResourceManager {
	private List<Resource> resourceList = new ArrayList<Resource>();
	private List<Resource> resourceListToUpdate = new ArrayList<Resource>();
	private boolean contextHasBeenRemoved = true;
	private boolean exiting = false;
	private static final int MAX_RESOURCE_LEVEL = 9;
	/**
	 * @brief initialize the internal variable
	 */
	public ResourceManager() {
		
	}
	/**
	 * @brief Uninitiamize the resource manager, free all resources previously requested
	 * @note when not free  == > generate warning, because the segfault can appear after...
	 */
	//public ~Manager();
	/**
	 * @brief remove all resources (un-init) out of the destructor (due to the system implementation)
	 */
	public void unInit() {
		display();
		this.resourceListToUpdate.clear();
		// remove all resources ...
		for(Resource it : this.resourceList) {
			Log.warning("Find a resource that is not removed : [" + it.getId() + "]"
			             + "='" + it.getName() + "' "
			             + it.getCount() + " elements");
		}
		this.resourceList.clear();
	}
	/**
	 * @brief display in the log all the resources loaded ...
	 */
	public void display(){
		Log.info("Resources loaded : ");
		// remove all resources ...
		for(Resource it : this.resourceList) {
			Log.info("    [" + it.getId() + "]"
			          + it.getType()
			          + "='" + it.getName() + "' "
			          + it.getCount() + " elements");
		}
		Log.info("Resources ---");
	}
	/**
	 * @brief Reload all resources from files, and send there in openGL card if needed.
	 * @note If file is reference at THEMEXXX:///filename if the Theme change the file will reload the newOne
	 */
	public void reLoadResources() {
		Log.info("-------------  Resources re-loaded  -------------");
		// remove all resources ...
		for (long jjj=0; jjj<MAX_RESOURCE_LEVEL; jjj++) {
			Log.info("    Reload level : " + jjj + "/" + (MAX_RESOURCE_LEVEL-1));
			for(Resource it : this.resourceList) {
				if(jjj == it.getResourceLevel()) {
					if (it.getCount() > 0) {
						it.reload();
						Log.info("        [" + it.getId() + "]="+ it.getType());
					}
				}
			}
		}
		// TODO UNderstand why it is set here ...
		//gale::requestUpdateSize();
		Log.info("-------------  Resources  -------------");
	}

	/**
	 * @brief Call by the system to send all the needed data on the graphic card chen they change ...
	 * @param object The resources that might be updated
	 */
	public void update(Resource object) {
		// check if not added before
		for (Resource it : this.resourceListToUpdate) {
			if (it == object) {
				// just prevent some double add ...
				return;
			}
		}
		// add it ...
		this.resourceListToUpdate.add(object);
	}
	/**
	 * @brief Call by the system chen the openGL Context has been unexpectially removed  == > This reload all the texture, VBO and other ....
	 */
	public void updateContext(){
		if (this.exiting == true) {
			Log.error("Request update after application EXIT ...");
			return;
		}
		// TODO Check the number of call this ... Log.info("update open-gl context ... ");
		if (this.contextHasBeenRemoved == true) {
			// need to update all ...
			this.contextHasBeenRemoved = false;
			this.resourceListToUpdate.clear();
			synchronized(this.resourceList) {
				if (this.resourceList.size() != 0) {
					for (long jjj=0; jjj<MAX_RESOURCE_LEVEL; jjj++) {
						Log.verbose("    updateContext level (D) : " + jjj + "/" + (MAX_RESOURCE_LEVEL-1));
						for (Resource it : this.resourceList) {
							if(jjj == it.getResourceLevel()) {
								//Log.debug("Update context named : " + lresourceList[iii].getName());
								if (it.updateContext() == false) {
									// Lock error ==> postponned
									this.resourceListToUpdate.add(it);
								}
							}
						}
					}
				}
			}
		} else {
			List<Resource> resourceListToUpdate = null;
			synchronized(this.resourceListToUpdate) {
				resourceListToUpdate = this.resourceListToUpdate;
				this.resourceListToUpdate = new ArrayList<Resource>();
			}
			if (resourceListToUpdate.size() != 0) {
				for (long jjj=0; jjj<MAX_RESOURCE_LEVEL; jjj++) {
					Log.verbose("    updateContext level (U) : " + jjj + "/" + (MAX_RESOURCE_LEVEL-1));
					for (Resource it : resourceListToUpdate) {
						if (jjj == it.getResourceLevel()) {
							if (it.updateContext() == false) {
								// Lock error ==> postponned
								this.resourceListToUpdate.add(it);
							}
						}
					}
				}
			}
		}
	}
	/**
	 * @brief This is to inform the resources manager that we have no more openGl context ...
	 */
	public void contextHasBeenDestroyed() {
		for (Resource it : this.resourceList) {
			if (it.getCount() > 0) {
				it.removeContextToLate();
			}
		}
		// no context preent ...
		this.contextHasBeenRemoved = true;
	}
	/**
	 * @brief special end of application
	 */
	public void applicationExiting(){
		contextHasBeenDestroyed();
		this.exiting = true;
	}
	// internal API to extent eResources in extern Soft
	public Resource localKeep(String filename) {
		Log.verbose("KEEP (DEFAULT) : file : '" + filename + "' in " + this.resourceList.size() + " resources");
		for (Resource it : this.resourceList) {
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
	public void localAdd(Resource object) {
		// add at the end if no slot is free
		this.resourceList.add(object);
	}
	public void cleanInternalRemoved() {
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

}
