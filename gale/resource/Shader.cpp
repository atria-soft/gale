/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license APACHE v2.0 (see license file)
 */

#include <etk/types.h>
#include <etk/os/FSNode.h>
#include <gale/debug.h>
#include <gale/resource/Shader.h>
#include <gale/resource/Manager.h>

#include <gale/renderer/openGL/openGL-include.h>

#undef __class__
#define __class__ "resource::Shader"

gale::resource::Shader::Shader() :
  gale::Resource(),
  m_exist(false),
  m_fileData(""),
  m_shader(-1),
  m_type(gale::openGL::shader::type_vertex) {
	addResourceType("gale::Shader");
	m_resourceLevel = 0;
}

void gale::resource::Shader::init(const std::string& _filename) {
	gale::Resource::init(_filename);
	GALE_DEBUG("OGL : load SHADER \"" << _filename << "\"");
	// load data from file "all the time ..."
	
	if (etk::end_with(m_name, ".frag") == true) {
		m_type = gale::openGL::shader::type_fragment;
	} else if (etk::end_with(m_name, ".vert") == true) {
		m_type = gale::openGL::shader::type_vertex;
	} else {
		GALE_ERROR("File does not have extention \".vert\" for Vertex Shader or \".frag\" for Fragment Shader. but : \"" << m_name << "\"");
		return;
	}
	reload();
}

gale::resource::Shader::~Shader() {
	m_fileData.clear();
	gale::openGL::shader::remove(m_shader);
	m_exist = false;
}

void gale::resource::Shader::updateContext() {
	if (m_exist == true) {
		// Do nothing  == > too dangerous ...
	} else {
		// create the Shader
		if (m_fileData.size() == 0) {
			m_shader = -1;
			return;
		}
		GALE_INFO("Create Shader : '" << m_name << "'");
		m_shader = gale::openGL::shader::create(m_type);
		if (m_shader < 0) {
			GALE_CRITICAL(" can not load shader");
			return;
		} else {
			GALE_INFO("Compile shader with GLID=" << m_shader);
			bool ret = gale::openGL::shader::compile(m_shader, m_fileData);
			if (ret == false) {
				const char * tmpShaderType = "FRAGMENT SHADER";
				if (m_type == gale::openGL::shader::type_vertex){
					tmpShaderType = "VERTEX SHADER";
				}
				GALE_CRITICAL("Could not compile \"" << tmpShaderType << "\" name='" << m_name << "'");
				return;
			}
		}
		m_exist = true;
	}
}

void gale::resource::Shader::removeContext() {
	if (true == m_exist) {
		gale::openGL::shader::remove(m_shader);
		m_exist = false;
	}
}

void gale::resource::Shader::removeContextToLate() {
	m_exist = false;
	m_shader = 0;
}

void gale::resource::Shader::reload() {
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
	removeContext();
	updateContext();
}

