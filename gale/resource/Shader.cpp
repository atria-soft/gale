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

#undef __class__
#define __class__ "resource::Shader"

gale::resource::Shader::Shader() :
  gale::Resource(),
  m_exist(false),
  m_fileData(nullptr),
  m_shader(0),
  m_type(0) {
	addResourceType("gale::Shader");
	m_resourceLevel = 0;
}

void gale::resource::Shader::init(const std::string& _filename) {
	gale::Resource::init(_filename);
	GALE_DEBUG("OGL : load SHADER \"" << _filename << "\"");
	// load data from file "all the time ..."
	
	if (etk::end_with(m_name, ".frag") == true) {
		m_type = GL_FRAGMENT_SHADER;
	} else if (etk::end_with(m_name, ".vert") == true) {
		m_type = GL_VERTEX_SHADER;
	} else {
		GALE_ERROR("File does not have extention \".vert\" for Vertex Shader or \".frag\" for Fragment Shader. but : \"" << m_name << "\"");
		return;
	}
	reload();
}

gale::resource::Shader::~Shader() {
	if (nullptr != m_fileData) {
		delete [] m_fileData;
		m_fileData = nullptr;
	}
	if (0!=m_shader) {
		glDeleteShader(m_shader);
		m_shader = 0;
	}
	m_exist = false;
}

static void checkGlError(const char* _op) {
	for (GLint error = glGetError(); error; error = glGetError()) {
		GALE_ERROR("after " << _op << "() glError (" << error << ")");
	}
}
#define LOG_OGL_INTERNAL_BUFFER_LEN    (8192)
static char l_bufferDisplayError[LOG_OGL_INTERNAL_BUFFER_LEN] = "";

void gale::resource::Shader::updateContext() {
	if (true == m_exist) {
		// Do nothing  == > too dangerous ...
	} else {
		// create the Shader
		if (nullptr == m_fileData) {
			m_shader = 0;
			return;
		}
		GALE_INFO("Create Shader : '" << m_name << "'");
		m_shader = glCreateShader(m_type);
		if (!m_shader) {
			GALE_ERROR("glCreateShader return error ...");
			checkGlError("glCreateShader");
			GALE_CRITICAL(" can not load shader");
			return;
		} else {
			GALE_INFO("Compile shader with GLID=" << m_shader);
			glShaderSource(m_shader, 1, (const char**)&m_fileData, nullptr);
			glCompileShader(m_shader);
			GLint compiled = 0;
			glGetShaderiv(m_shader, GL_COMPILE_STATUS, &compiled);
			if (!compiled) {
				GLint infoLen = 0;
				l_bufferDisplayError[0] = '\0';
				glGetShaderInfoLog(m_shader, LOG_OGL_INTERNAL_BUFFER_LEN, &infoLen, l_bufferDisplayError);
				const char * tmpShaderType = "GL_FRAGMENT_SHADER";
				if (m_type == GL_VERTEX_SHADER){
					tmpShaderType = "GL_VERTEX_SHADER";
				}
				GALE_ERROR("Could not compile \"" << tmpShaderType << "\" name='" << m_name << "'");
				GALE_ERROR("Error " << l_bufferDisplayError);
				std::vector<std::string> lines = etk::split(m_fileData, '\n');
				for (size_t iii=0 ; iii<lines.size() ; iii++) {
					GALE_ERROR("file " << (iii+1) << "|" << lines[iii]);
				}
				GALE_CRITICAL(" can not load shader");
				return;
			}
		}
		m_exist = true;
	}
}

void gale::resource::Shader::removeContext() {
	if (true == m_exist) {
		glDeleteShader(m_shader);
		m_exist = false;
		m_shader = 0;
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
	// remove previous data ...
	if (nullptr != m_fileData) {
		delete[] m_fileData;
		m_fileData = 0;
	}
	// allocate data
	m_fileData = new char[fileSize+5];
	if (nullptr == m_fileData) {
		GALE_CRITICAL("Error Memory allocation size=" << fileSize);
		return;
	}
	memset(m_fileData, 0, (fileSize+5)*sizeof(char));
	// load data from the file :
	file.fileRead(m_fileData, 1, fileSize);
	// close the file:
	file.fileClose();
	
	// now change the OGL context ...
	removeContext();
	updateContext();
}

