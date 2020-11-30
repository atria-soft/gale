package org.atriasoft.gale.resource;

import java.util.ArrayList;
import java.util.List;

import org.atriasoft.etk.Uri;
import org.atriasoft.gale.internal.Log;
import org.atriasoft.gale.context.Context;

public abstract class Resource {
	protected static final String NO_NAME_RESOURCE = "---";
	protected static final int MAXRESOURCELEVEL = 5;
	private static int idGenerated = 10;
	protected long uid = -1; //!< unique ID definition
	protected int count = 1;
	protected int resourceLevel = MAXRESOURCELEVEL-1; //!< Level of the resource ==> for update priority [0..5] 0 must be update first.
	protected String name = NO_NAME_RESOURCE; //!< name of the resource ...
	protected boolean resourceHasBeenInit = false; //!< Know if the init function has bben called
	protected List<String> listType = new ArrayList<String>();
	/**
	 * @brief generic protected contructor (use factory to create this class)
	 */
	protected Resource() {
		this.uid = idGenerated++;
		this.addResourceType("gale::Resource");
		this.resourceHasBeenInit = true;
	}
	protected Resource(String name) {
		this.resourceHasBeenInit = true;
		this.name = name;
	}
	protected Resource( Uri uri) {
		this.resourceHasBeenInit = true;
		this.name = uri.getValue();
	}
	public void keep() {
		this.count++;
	}
	public int getCount() {
		return this.count;
	}
	public void release() {
		this.count--;
		if (this.count == 0) {
			
		}
	}
	public abstract void cleanUp();
	public long getId() {
		return this.uid;
	}
	public boolean resourceHasBeenCorectlyInit() {
		return this.resourceHasBeenInit;
	}
	/**
	 * @brief get the current type of the Resource
	 * @return the last type name of the element
	 */
	public String getType() {
		if (this.listType.size() == 0) {
			return "gale::Resource";
		}
		return this.listType.get(this.listType.size()-1);
	}
	/**
	 * @brief Get the herarchic of the Resource type.
	 * @return descriptive string.
	 */
	public String getTypeDescription() {
		String ret = "gale::Resource";
		for(String element : this.listType) {
			ret += "|";
			ret += element;
		}
		return ret;
	}
	/**
	 * @brief check  if the element herited from a specific type
	 * @param type Type to check.
	 * @return true if the element is compatible.
	 */
	public boolean isTypeCompatible( String type) {
		if (type == "gale::Resource") {
			return true;
		}
		for(String element : this.listType) {
			if (type == element) {
				return true;
			}
		}
		return false;
	}
	/**
	 * @brief Add a type of the list of Object.
	 * @param type Type to add.
	 */
	protected void addResourceType(String type) {
		if (type == null) {
			Log.error(" try to add a type with no value...");
			return;
		}
		this.listType.add(type);
	}
	/**
	 * @brief get the resource name
	 * @return The requested name
	 */
	public String getName()  {
		return this.name;
	}
	/**
	 * @brief get the resource name
	 * @param name The name to set.
	 */
	public void setName( String name) {
		this.name = name;
	}
	/**
	 * @brief Get the current resource level;
	 * @return value in [0..5]
	 */
	public int getResourceLevel() {
		return this.resourceLevel;
	};
	/**
	 * @brief Call when need to send data on the harware (openGL)
	 * @note This is done asynchronously with the create of the Resource.
	 * @return true The context is updated
	 * @return false The context is not updated
	 */
	public boolean updateContext() {
		Log.debug("Not set for : [" + getId() + "]" + getName() + " loaded ??? time(s)");
		return true;
	}

	/**
	 * @brief The current OpenGl context is removing ==> remove yout own system data
	 */
	public void removeContext() {
		Log.debug("Not set for : [" + getId() + "]" + getName() + " loaded ??? time(s)");
	}
	/**
	 * @brief The notification of the Context removing is too late, we have no more acces on the OpenGl context (thank you Android).
	 * Juste update your internal state
	 */
	public void removeContextToLate() {
		Log.debug("Not set for : [" + getId() + "]" + getName() + " loaded ??? time(s)");
	}
	/**
	 * @brief User request the reload of all resources (usefull when the file depend on DATA:GUI:xxx ...
	 */
	public void reload() {
		Log.debug("Not set for : [" + getId() + "]" + getName() + " loaded ??? time(s)");
	}
	/**
	 * @brief Get the current resource Manager
	 */
	protected static ResourceManager getManager() {
		return Context.getContext().getResourcesManager();
	}
}
