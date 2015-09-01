/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license APACHE v2.0 (see license file)
 */

#include <etk/types.h>
#include <gale/debug.h>
#include <gale/resource/Manager.h>
#include <gale/gale.h>
#include <gale/renderer/openGL/openGL.h>
#include <gale/context/Context.h>



gale::resource::Manager::Manager() :
  m_contextHasBeenRemoved(true) {
	// nothing to do ...
}

gale::resource::Manager::~Manager() {
	std11::unique_lock<std11::mutex> lock(m_mutex);
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
	std11::unique_lock<std11::mutex> lock(m_mutex);
	display();
	m_resourceListToUpdate.clear();
	// remove all resources ...
	auto it(m_resourceList.begin());
	while(it != m_resourceList.end()) {
		std::shared_ptr<gale::Resource> tmpRessource = (*it).lock();
		if (tmpRessource != nullptr) {
			GALE_WARNING("Find a resource that is not removed : [" << tmpRessource->getId() << "]"
			             << "=\"" << tmpRessource->getName() << "\" "
			             << tmpRessource.use_count() << " elements");
		}
		m_resourceList.erase(it);
		it = m_resourceList.begin();
	}
	m_resourceList.clear();
}

void gale::resource::Manager::display() {
	GALE_INFO("Resources loaded : ");
	// remove all resources ...
	std11::unique_lock<std11::mutex> lock(m_mutex);
	for (auto &it : m_resourceList) {
		std::shared_ptr<gale::Resource> tmpRessource = it.lock();
		if (tmpRessource != nullptr) {
			GALE_INFO("    [" << tmpRessource->getId() << "]"
			          << tmpRessource->getType()
			          << "=\"" << tmpRessource->getName() << "\" "
			          << tmpRessource.use_count() << " elements");
		}
	}
	GALE_INFO("Resources ---");
}

void gale::resource::Manager::reLoadResources() {
	GALE_INFO("-------------  Resources re-loaded  -------------");
	// remove all resources ...
	std11::unique_lock<std11::mutex> lock(m_mutex);
	if (m_resourceList.size() != 0) {
		for (size_t jjj=0; jjj<MAX_RESOURCE_LEVEL; jjj++) {
			GALE_INFO("    Reload level : " << jjj << "/" << (MAX_RESOURCE_LEVEL-1));
			for (auto &it : m_resourceList) {
				std::shared_ptr<gale::Resource> tmpRessource = it.lock();
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

void gale::resource::Manager::update(const std::shared_ptr<gale::Resource>& _object) {
	// chek if not added before
	std11::unique_lock<std11::mutex> lock(m_mutex);
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
	std11::unique_lock<std11::mutex> lock(m_mutex);
	// TODO : Check the number of call this ... GALE_INFO("update open-gl context ... ");
	if (m_contextHasBeenRemoved == true) {
		// need to update all ...
		m_contextHasBeenRemoved = false;
		if (m_resourceList.size() != 0) {
			for (size_t jjj=0; jjj<MAX_RESOURCE_LEVEL; jjj++) {
				GALE_INFO("    updateContext level (D) : " << jjj << "/" << (MAX_RESOURCE_LEVEL-1));
				for (auto &it : m_resourceList) {
					std::shared_ptr<gale::Resource> tmpRessource = it.lock();
					if(    tmpRessource != nullptr
					    && jjj == tmpRessource->getResourceLevel()) {
						//GALE_DEBUG("Update context named : " << l_resourceList[iii]->getName());
						tmpRessource->updateContext();
					}
				}
			}
		}
	} else {
		if (m_resourceListToUpdate.size() != 0) {
			for (size_t jjj=0; jjj<MAX_RESOURCE_LEVEL; jjj++) {
				GALE_INFO("    updateContext level (U) : " << jjj << "/" << (MAX_RESOURCE_LEVEL-1));
				for (auto &it : m_resourceListToUpdate) {
					if (    it != nullptr
					     && jjj == it->getResourceLevel()) {
						it->updateContext();
					}
				}
			}
		}
	}
	// Clean the update list
	m_resourceListToUpdate.clear();
}

// in this case, it is really too late ...
void gale::resource::Manager::contextHasBeenDestroyed() {
	std11::unique_lock<std11::mutex> lock(m_mutex);
	for (auto &it : m_resourceList) {
		std::shared_ptr<gale::Resource> tmpRessource = it.lock();
		if (tmpRessource != nullptr) {
			tmpRessource->removeContextToLate();
		}
	}
	// no context preent ...
	m_contextHasBeenRemoved = true;
}

// internal generic keeper ...
std::shared_ptr<gale::Resource> gale::resource::Manager::localKeep(const std::string& _filename) {
	std11::unique_lock<std11::mutex> lock(m_mutex);
	GALE_VERBOSE("KEEP (DEFAULT) : file : '" << _filename << "' in " << m_resourceList.size() << " resources");
	for (auto &it : m_resourceList) {
		std::shared_ptr<gale::Resource> tmpRessource = it.lock();
		if (tmpRessource != nullptr) {
			if (tmpRessource->getName() == _filename) {
				return tmpRessource;
			}
		}
	}
	return nullptr;
}

// internal generic keeper ...
void gale::resource::Manager::localAdd(const std::shared_ptr<gale::Resource>& _object) {
	std11::unique_lock<std11::mutex> lock(m_mutex);
	//Add ... find empty slot
	for (auto &it : m_resourceList) {
		std::shared_ptr<gale::Resource> tmpRessource = it.lock();
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
	std11::unique_lock<std11::mutex> lock(m_mutex);
	//GALE_INFO("remove object in Manager");
	updateContext();
	for (auto it(m_resourceList.begin()); it!=m_resourceList.end(); ++it) {
		if ((*it).expired() == true) {
			m_resourceList.erase(it);
			it = m_resourceList.begin();
		}
	}
}
