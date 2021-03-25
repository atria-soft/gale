package org.atriasoft.gale.resource;

import org.atriasoft.etk.Uri;
import org.atriasoft.gale.context.Context;
import org.atriasoft.gale.internal.Log;

public abstract class Resource {
	protected static final String NO_NAME_RESOURCE = "---";
	protected static final int MAXRESOURCELEVEL = 5;
	private static int idGenerated = 10;
	
	/**
	 * Get the current resource Manager
	 */
	protected static ResourceManager getManager() {
		return Context.getContext().getResourcesManager();
	}
	
	protected long uid = -1; //!< unique ID definition
	protected int count = 1;
	protected int resourceLevel = MAXRESOURCELEVEL - 1; //!< Level of the resource ==> for update priority [0..5] 0 must be update first.
	protected String name = NO_NAME_RESOURCE; //!< name of the resource ...
	
	/**
	 * generic protected contructor (use factory to create this class)
	 */
	protected Resource() {
		this.uid = idGenerated++;
		getManager().localAdd(this);
	}
	
	protected Resource(final String name) {
		this.name = name;
		getManager().localAdd(this);
	}
	
	protected Resource(final Uri uri) {
		this.name = uri.getValue();
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
	
	public void keep() {
		this.count++;
	}
	
	public void release() {
		this.count--;
		if (this.count == 0) {
			
		}
	}
	
	/**
	 * User request the reload of all resources (usefull when the file depend on DATA:GUI:xxx ...
	 */
	public void reload() {
		Log.debug("Not set for : [" + getId() + "]" + getName() + " loaded ??? time(s)");
	};
	
	/**
	 * The current OpenGl context is removing ==> remove yout own system data
	 */
	public void removeContext() {
		Log.debug("Not set for : [" + getId() + "]" + getName() + " loaded ??? time(s)");
	}
	
	/**
	 * The notification of the Context removing is too late, we have no more acces on the OpenGl context (thank you Android).
	 * Just update your internal state
	 */
	public void removeContextToLate() {
		Log.debug("Not set for : [" + getId() + "]" + getName() + " loaded ??? time(s)");
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
		Log.debug("Not set for : [" + getId() + "]" + getName() + " loaded ??? time(s)");
		return true;
	}
}
