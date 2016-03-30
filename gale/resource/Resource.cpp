/** @file
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license APACHE v2.0 (see license file)
 */

#include <etk/types.h>
#include <gale/debug.h>
#include <gale/resource/Resource.h>
#include <gale/resource/Manager.h>
#include <gale/gale.h>
#include <gale/context/Context.h>


gale::Resource::Resource() :
  m_id(0),
  m_resourceHasBeenInit(false),
  m_resourceLevel(MAX_RESOURCE_LEVEL-1) {
	static size_t id = 0;
	m_id = id++;
	addResourceType("gale::Resource");
};

void gale::Resource::init() {
	m_resourceHasBeenInit=true;
}

void gale::Resource::init(const std::string& _name) {
	m_resourceHasBeenInit=true;
	m_name = _name;
}

const char * const gale::Resource::getType() {
	if (m_listType.size() == 0) {
		return "gale::Resource";
	}
	return m_listType.back();
}

void gale::Resource::addResourceType(const char* _type) {
	if (_type == nullptr) {
		GALE_ERROR(" try to add a type with no value...");
		return;
	}
	m_listType.push_back(_type);
}
std::string gale::Resource::getTypeDescription() {
	std::string ret("gale::Resource");
	for(auto element : m_listType) {
		ret += "|";
		ret += element;
	}
	return ret;
}

bool gale::Resource::isTypeCompatible(const std::string& _type) {
	if (_type == "gale::Resource") {
		return true;
	}
	for(auto element : m_listType) {
		if (_type == element) {
			return true;
		}
	}
	return false;
}


bool gale::Resource::updateContext() {
	GALE_DEBUG("Not set for : [" << getId() << "]" << getName() << " loaded " << shared_from_this().use_count() << " time(s)");
	return true;
}

void gale::Resource::removeContext() {
	GALE_DEBUG("Not set for : [" << getId() << "]" << getName() << " loaded " << shared_from_this().use_count() << " time(s)");
}

void gale::Resource::removeContextToLate() {
	GALE_DEBUG("Not set for : [" << getId() << "]" << getName() << " loaded " << shared_from_this().use_count() << " time(s)");
}

void gale::Resource::reload() {
	GALE_DEBUG("Not set for : [" << getId() << "]" << getName() << " loaded " << shared_from_this().use_count() << " time(s)");
}

gale::resource::Manager& gale::Resource::getManager() {
	return gale::getContext().getResourcesManager();
}