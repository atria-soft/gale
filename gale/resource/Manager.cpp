/** @file
 * @author Edouard DUPIN
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license MPL v2.0 (see license file)
 */

#include <etk/types.hpp>
#include <gale/debug.hpp>
#include <gale/resource/Manager.hpp>
#include <gale/gale.hpp>
#include <gale/renderer/openGL/openGL.hpp>
#include <gale/context/Context.hpp>



gale::resource::Manager::Manager() :
  m_contextHasBeenRemoved(true),
  m_exiting(false) {
	// nothing to do ...
}

gale::resource::Manager::~Manager() {
	std::unique_lock<std::recursive_mutex> lock(m_mutex);
	bool hasError = false;
	if (m_resourceListToUpdate.size()!=0) {
		GALE_ERROR("Must not have anymore resources to update !!!");
		hasError = true;
	}
	// TODO : Remove unneeded elements
	if (m_resourceList.size()!=0) {
		GALE_ERROR("Must not have anymore resources !!!");
		hasError = true;
	}
	if (true == hasError) {
		GALE_ERROR("Check if the function UnInit has been called !!!");
	}
}

void gale::resource::Manager::unInit() {
	std::unique_lock<std::recursive_mutex> lock(m_mutex);
	display();
	m_resourceListToUpdate.clear();
	// remove all resources ...
	auto it(m_resourceList.begin());
	while(it != m_resourceList.end()) {
		ememory::SharedPtr<gale::Resource> tmpRessource = (*it).lock();
		if (tmpRessource != nullptr) {
			GALE_WARNING("Find a resource that is not removed : [" << tmpRessource->getId() << "]"
			             << "=\"" << tmpRessource->getName() << "\" "
			             << tmpRessource.useCount() << " elements");
		}
		m_resourceList.erase(it);
		it = m_resourceList.begin();
	}
	m_resourceList.clear();
}

void gale::resource::Manager::display() {
	GALE_INFO("Resources loaded : ");
	// remove all resources ...
	std::unique_lock<std::recursive_mutex> lock(m_mutex);
	for (auto &it : m_resourceList) {
		ememory::SharedPtr<gale::Resource> tmpRessource = it.lock();
		if (tmpRessource != nullptr) {
			GALE_INFO("    [" << tmpRessource->getId() << "]"
			          << tmpRessource->getType()
			          << "=\"" << tmpRessource->getName() << "\" "
			          << tmpRessource.useCount() << " elements");
		}
	}
	GALE_INFO("Resources ---");
}

void gale::resource::Manager::reLoadResources() {
	GALE_INFO("-------------  Resources re-loaded  -------------");
	// remove all resources ...
	std::unique_lock<std::recursive_mutex> lock(m_mutex);
	if (m_resourceList.size() != 0) {
		for (size_t jjj=0; jjj<MAX_RESOURCE_LEVEL; jjj++) {
			GALE_INFO("    Reload level : " << jjj << "/" << (MAX_RESOURCE_LEVEL-1));
			for (auto &it : m_resourceList) {
				ememory::SharedPtr<gale::Resource> tmpRessource = it.lock();
				if(tmpRessource != nullptr) {
					if (jjj == tmpRessource->getResourceLevel()) {
						tmpRessource->reload();
						GALE_INFO("        [" << tmpRessource->getId() << "]="<< tmpRessource->getType());
					}
				}
			}
		}
	}
	// TODO : UNderstand why it is set here ...
	//gale::requestUpdateSize();
	GALE_INFO("-------------  Resources  -------------");
}

void gale::resource::Manager::update(const ememory::SharedPtr<gale::Resource>& _object) {
	// chek if not added before
	std::unique_lock<std::recursive_mutex> lock(m_mutex);
	for (auto &it : m_resourceListToUpdate) {
		if (    it != nullptr
		     && it == _object) {
			// just prevent some double add ...
			return;
		}
	}
	// add it ...
	m_resourceListToUpdate.push_back(_object);
}

// Specific to load or update the data in the openGl context  == > system use only
void gale::resource::Manager::updateContext() {
	if (m_exiting == true) {
		GALE_ERROR("Request update after application EXIT ...");
		return;
	}
	// TODO : Check the number of call this ... GALE_INFO("update open-gl context ... ");
	if (m_contextHasBeenRemoved == true) {
		// need to update all ...
		m_contextHasBeenRemoved = false;
		std::list<ememory::WeakPtr<gale::Resource>> resourceList;
		{
			std::unique_lock<std::recursive_mutex> lock(m_mutex);
			// Clean the update list
			m_resourceListToUpdate.clear();
			resourceList = m_resourceList;
		}
		if (resourceList.size() != 0) {
			for (size_t jjj=0; jjj<MAX_RESOURCE_LEVEL; jjj++) {
				GALE_INFO("    updateContext level (D) : " << jjj << "/" << (MAX_RESOURCE_LEVEL-1));
				for (auto &it : resourceList) {
					ememory::SharedPtr<gale::Resource> tmpRessource = it.lock();
					if(    tmpRessource != nullptr
					    && jjj == tmpRessource->getResourceLevel()) {
						//GALE_DEBUG("Update context named : " << l_resourceList[iii]->getName());
						if (tmpRessource->updateContext() == false) {
							// Lock error ==> postponned
							std::unique_lock<std::recursive_mutex> lock(m_mutex);
							m_resourceListToUpdate.push_back(tmpRessource);
						}
					}
				}
			}
		}
	} else {
		std::vector<ememory::SharedPtr<gale::Resource>> resourceListToUpdate;
		{
			std::unique_lock<std::recursive_mutex> lock(m_mutex);
			resourceListToUpdate = m_resourceListToUpdate;
			// Clean the update list
			m_resourceListToUpdate.clear();
		}
		if (resourceListToUpdate.size() != 0) {
			for (size_t jjj=0; jjj<MAX_RESOURCE_LEVEL; jjj++) {
				GALE_DEBUG("    updateContext level (U) : " << jjj << "/" << (MAX_RESOURCE_LEVEL-1));
				for (auto &it : resourceListToUpdate) {
					if (    it != nullptr
					     && jjj == it->getResourceLevel()) {
						if (it->updateContext() == false) {
							std::unique_lock<std::recursive_mutex> lock(m_mutex);
							// Lock error ==> postponned
							m_resourceListToUpdate.push_back(it);
						}
					}
				}
			}
		}
	}
}

// in this case, it is really too late ...
void gale::resource::Manager::contextHasBeenDestroyed() {
	std::unique_lock<std::recursive_mutex> lock(m_mutex);
	for (auto &it : m_resourceList) {
		ememory::SharedPtr<gale::Resource> tmpRessource = it.lock();
		if (tmpRessource != nullptr) {
			tmpRessource->removeContextToLate();
		}
	}
	// no context preent ...
	m_contextHasBeenRemoved = true;
}

void gale::resource::Manager::applicationExiting() {
	contextHasBeenDestroyed();
	m_exiting = true;
}

// internal generic keeper ...
ememory::SharedPtr<gale::Resource> gale::resource::Manager::localKeep(const std::string& _filename) {
	std::unique_lock<std::recursive_mutex> lock(m_mutex);
	GALE_VERBOSE("KEEP (DEFAULT) : file : '" << _filename << "' in " << m_resourceList.size() << " resources");
	for (auto &it : m_resourceList) {
		ememory::SharedPtr<gale::Resource> tmpRessource = it.lock();
		if (tmpRessource != nullptr) {
			if (tmpRessource->getName() == _filename) {
				return tmpRessource;
			}
		}
	}
	return nullptr;
}

// internal generic keeper ...
void gale::resource::Manager::localAdd(const ememory::SharedPtr<gale::Resource>& _object) {
	std::unique_lock<std::recursive_mutex> lock(m_mutex);
	//Add ... find empty slot
	for (auto &it : m_resourceList) {
		ememory::SharedPtr<gale::Resource> tmpRessource = it.lock();
		if (tmpRessource == nullptr) {
			it = _object;
			return;
		}
	}
	// add at the end if no slot is free
	m_resourceList.push_back(_object);
}

// in case of error ...
void gale::resource::Manager::cleanInternalRemoved() {
	std::unique_lock<std::recursive_mutex> lock(m_mutex);
	//GALE_INFO("remove object in Manager");
	updateContext();
	for (auto it(m_resourceList.begin()); it!=m_resourceList.end(); ++it) {
		if ((*it).expired() == true) {
			m_resourceList.erase(it);
			it = m_resourceList.begin();
		}
	}
}
