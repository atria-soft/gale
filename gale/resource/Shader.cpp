/** @file
 * @author Edouard DUPIN
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license MPL v2.0 (see license file)
 */

#include <etk/types.hpp>
#include <etk/uri/uri.hpp>
#include <gale/debug.hpp>
#include <gale/resource/Shader.hpp>
#include <gale/resource/Manager.hpp>

#include <gale/renderer/openGL/openGL-include.hpp>

#include <etk/typeInfo.hpp>
ETK_DECLARE_TYPE(gale::resource::Shader);

gale::resource::Shader::Shader() :
  gale::Resource(),
  m_exist(false),
  m_fileData(""),
  m_shader(-1),
  m_type(gale::openGL::shader::type::vertex) {
	addResourceType("gale::Shader");
	m_resourceLevel = 0;
}

void gale::resource::Shader::init(const etk::Uri& _uri) {
	ethread::RecursiveLock lock(m_mutex);
	gale::Resource::init(_uri.get());
	GALE_DEBUG("OGL : load SHADER '" << _uri << "'");
	// load data from file "all the time ..."
	
	if (_uri.getPath().getExtention() == "frag") {
		m_type = gale::openGL::shader::type::fragment;
	} else if (_uri.getPath().getExtention() == "vert") {
		m_type = gale::openGL::shader::type::vertex;
	} else {
		GALE_ERROR("File does not have extention \".vert\" for Vertex Shader or \".frag\" for Fragment Shader. but : \"" << _uri << "\"");
		return;
	}
	reload();
}

gale::resource::Shader::~Shader() {
	ethread::RecursiveLock lock(m_mutex);
	m_fileData.clear();
	gale::openGL::shader::remove(m_shader);
	m_exist = false;
}

bool gale::resource::Shader::updateContext() {
	ethread::RecursiveLock lock(m_mutex, true);
	if (lock.tryLock() == false) {
		//Lock error ==> try later ...
		return false;
	}
	if (m_exist == true) {
		// Do nothing  == > too dangerous ...
	} else {
		// create the Shader
		if (m_fileData.size() == 0) {
			m_shader = -1;
			return true;
		}
		GALE_DEBUG("Create Shader : '" << m_name << "'");
		m_shader = gale::openGL::shader::create(m_type);
		if (m_shader < 0) {
			GALE_CRITICAL(" can not load shader");
			return true;
		} else {
			GALE_DEBUG("Compile shader with GLID=" << m_shader);
			bool ret = gale::openGL::shader::compile(m_shader, m_fileData);
			if (ret == false) {
				const char * tmpShaderType = "FRAGMENT SHADER";
				if (m_type == gale::openGL::shader::type::vertex){
					tmpShaderType = "VERTEX SHADER";
				}
				GALE_CRITICAL("Could not compile '" << tmpShaderType << "' name='" << m_name << "'");
				return true;
			}
		}
		m_exist = true;
	}
	return true;
}

void gale::resource::Shader::removeContext() {
	ethread::RecursiveLock lock(m_mutex);
	if (true == m_exist) {
		gale::openGL::shader::remove(m_shader);
		m_exist = false;
	}
}

void gale::resource::Shader::removeContextToLate() {
	ethread::RecursiveLock lock(m_mutex);
	m_exist = false;
	m_shader = 0;
}

void gale::resource::Shader::reload() {
	ethread::RecursiveLock lock(m_mutex);
	etk::Uri uri = m_name;
	if (etk::uri::exist(uri) == false) {
		GALE_CRITICAL("File does not Exist : '" << uri << "' : path='" << uri.getPath() << "'");
		return;
	}
	etk::uri::readAll(uri, m_fileData);
	GALE_VERBOSE("load shader:\n-----------------------------------------------------------------\n" << m_fileData << "\n-----------------------------------------------------------------");
	// now change the OGL context ...
	if (gale::openGL::hasContext() == true) {
		GALE_DEBUG("OGL : load SHADER '" << m_name << "' ==> call update context (direct)");
		removeContext();
		updateContext();
	} else {
		GALE_DEBUG("OGL : load SHADER '" << m_name << "' ==> tagged has update context needed");
		// TODO : Check this, this is a leek ==> in the GPU ... really bad ...
		m_exist = false;
		m_shader = 0;
		getManager().update(ememory::dynamicPointerCast<gale::Resource>(sharedFromThis()));
	}
}

