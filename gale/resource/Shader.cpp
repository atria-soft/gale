/** @file
 * @author Edouard DUPIN
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#include <etk/types.hpp>
#include <etk/os/FSNode.hpp>
#include <gale/debug.hpp>
#include <gale/resource/Shader.hpp>
#include <gale/resource/Manager.hpp>

#include <gale/renderer/openGL/openGL-include.hpp>

gale::resource::Shader::Shader() :
  gale::Resource(),
  m_exist(false),
  m_fileData(""),
  m_shader(-1),
  m_type(gale::openGL::shader::type::vertex) {
	addResourceType("gale::Shader");
	m_resourceLevel = 0;
}

void gale::resource::Shader::init(const std::string& _filename) {
	std::unique_lock<std::recursive_mutex> lock(m_mutex);
	gale::Resource::init(_filename);
	GALE_DEBUG("OGL : load SHADER '" << _filename << "'");
	// load data from file "all the time ..."
	
	if (etk::end_with(m_name, ".frag") == true) {
		m_type = gale::openGL::shader::type::fragment;
	} else if (etk::end_with(m_name, ".vert") == true) {
		m_type = gale::openGL::shader::type::vertex;
	} else {
		GALE_ERROR("File does not have extention \".vert\" for Vertex Shader or \".frag\" for Fragment Shader. but : \"" << m_name << "\"");
		return;
	}
	reload();
}

gale::resource::Shader::~Shader() {
	std::unique_lock<std::recursive_mutex> lock(m_mutex);
	m_fileData.clear();
	gale::openGL::shader::remove(m_shader);
	m_exist = false;
}

bool gale::resource::Shader::updateContext() {
	std::unique_lock<std::recursive_mutex> lock(m_mutex, std::defer_lock);
	if (lock.try_lock() == false) {
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
			GALE_INFO("Compile shader with GLID=" << m_shader);
			bool ret = gale::openGL::shader::compile(m_shader, m_fileData);
			if (ret == false) {
				const char * tmpShaderType = "FRAGMENT SHADER";
				if (m_type == gale::openGL::shader::type::vertex){
					tmpShaderType = "VERTEX SHADER";
				}
				GALE_CRITICAL("Could not compile \"" << tmpShaderType << "\" name='" << m_name << "'");
				return true;
			}
		}
		m_exist = true;
	}
	return true;
}

void gale::resource::Shader::removeContext() {
	std::unique_lock<std::recursive_mutex> lock(m_mutex);
	if (true == m_exist) {
		gale::openGL::shader::remove(m_shader);
		m_exist = false;
	}
}

void gale::resource::Shader::removeContextToLate() {
	std::unique_lock<std::recursive_mutex> lock(m_mutex);
	m_exist = false;
	m_shader = 0;
}

void gale::resource::Shader::reload() {
	std::unique_lock<std::recursive_mutex> lock(m_mutex);
	etk::FSNode file(m_name);
	if (false == file.exist()) {
		GALE_CRITICAL("File does not Exist : '" << file << "' : '" << file.getFileSystemName() << "'");
		return;
	}
	
	int64_t fileSize = file.fileSize();
	if (0 == fileSize) {
		GALE_CRITICAL("This file is empty : " << file);
		return;
	}
	if (false == file.fileOpenRead()) {
		GALE_CRITICAL("Can not open the file : " << file);
		return;
	}
	m_fileData = file.fileReadAllString();
	// close the file:
	file.fileClose();
	
	// now change the OGL context ...
	
	if (gale::openGL::hasContext() == true) {
		GALE_DEBUG("OGL : load SHADER \"" << m_name << "\" ==> call update context (direct)");
		removeContext();
		updateContext();
	} else {
		GALE_DEBUG("OGL : load SHADER \"" << m_name << "\" ==> tagged has update context needed");
		// TODO : Check this, this is a leek ==> in the GPU ... really bad ...
			m_exist = false;
			m_shader = 0;
		getManager().update(ememory::dynamicPointerCast<gale::Resource>(sharedFromThis()));
	}
}

