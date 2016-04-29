/** @file
 * @author Edouard DUPIN
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#include <etk/types.h>
#include <gale/debug.h>
#include <gale/resource/Program.h>
#include <gale/resource/Manager.h>
#include <etk/os/FSNode.h>
#include <gale/gale.h>
#include <gale/renderer/openGL/openGL-include.h>

//#define LOCAL_DEBUG  GALE_VERBOSE
#define LOCAL_DEBUG  GALE_DEBUG

gale::resource::Program::Program() :
  gale::Resource(),
  m_exist(false),
  m_program(0),
  m_hasTexture(false),
  m_hasTexture1(false) {
	addResourceType("gale::resource::Program");
	m_resourceLevel = 1;
}

void gale::resource::Program::init(const std::string& _filename) {
	gale::Resource::init(_filename);
	std::unique_lock<std::recursive_mutex> lock(m_mutex);
	GALE_DEBUG("OGL : load PROGRAM '" << m_name << "'");
	// load data from file "all the time ..."
	
	etk::FSNode file(m_name);
	if (false == file.exist()) {
		GALE_INFO("File does not Exist : \"" << file << "\"  == > automatic load of framment and shader with same names... ");
		std::string tmpFilename = m_name;
		// remove extention ...
		tmpFilename.erase(tmpFilename.size()-4, 4);
		std::shared_ptr<gale::resource::Shader> tmpShader = gale::resource::Shader::create(tmpFilename+"vert");
		if (nullptr == tmpShader) {
			GALE_ERROR("Error while getting a specific shader filename : " << tmpFilename);
			return;
		} else {
			GALE_DEBUG("Add shader on program : "<< tmpFilename << "vert");
			m_shaderList.push_back(tmpShader);
		}
		tmpShader = gale::resource::Shader::create(tmpFilename+"frag");
		if (nullptr == tmpShader) {
			GALE_ERROR("Error while getting a specific shader filename : " << tmpFilename);
			return;
		} else {
			GALE_DEBUG("Add shader on program : "<< tmpFilename << "frag");
			m_shaderList.push_back(tmpShader);
		}
	} else {
		std::string fileExtention = file.fileGetExtention();
		if (fileExtention != "prog") {
			GALE_ERROR("File does not have extention \".prog\" for program but : \"" << fileExtention << "\"");
			return;
		}
		if (false == file.fileOpenRead()) {
			GALE_ERROR("Can not open the file : \"" << file << "\"");
			return;
		}
		#define MAX_LINE_SIZE   (2048)
		char tmpData[MAX_LINE_SIZE];
		while (file.fileGets(tmpData, MAX_LINE_SIZE) != nullptr) {
			int32_t len = strlen(tmpData);
			if(    tmpData[len-1] == '\n'
				|| tmpData[len-1] == '\r') {
				tmpData[len-1] = '\0';
				len--;
			}
			GALE_DEBUG(" Read data : \"" << tmpData << "\"");
			if (len == 0) {
				continue;
			}
			if (tmpData[0] == '#') {
				continue;
			}
			// get it with relative position :
			std::string tmpFilename = file.getRelativeFolder() + tmpData;
			std::shared_ptr<gale::resource::Shader> tmpShader = gale::resource::Shader::create(tmpFilename);
			if (nullptr == tmpShader) {
				GALE_ERROR("Error while getting a specific shader filename : " << tmpFilename);
			} else {
				GALE_DEBUG("Add shader on program : "<< tmpFilename);
				m_shaderList.push_back(tmpShader);
			}
			
		}
		// close the file:
		file.fileClose();
	}
	if (gale::openGL::hasContext() == true) {
		updateContext();
	} else {
		getManager().update(std::dynamic_pointer_cast<gale::Resource>(shared_from_this()));
	}
}

gale::resource::Program::~Program() {
	m_shaderList.clear();
	removeContext();
	m_elementList.clear();
	m_hasTexture = false;
	m_hasTexture1 = false;
}

std::ostream& gale::resource::operator <<(std::ostream& _os, const gale::resource::progAttributeElement& _obj) {
	_os << "{";
	_os << "[" << _obj.m_name << "] ";
	_os << _obj.m_elementId << " ";
	_os << _obj.m_isLinked;
	_os << "}";
	return _os;
}

std::ostream& gale::resource::operator <<(std::ostream& _os, const std::vector<gale::resource::progAttributeElement>& _obj){
	_os << "{";
	for (auto &it : _obj) {
		_os << it;
	}
	_os << "}";
	return _os;
}

void gale::resource::Program::checkGlError(const char* _op, int32_t _localLine, int32_t _idElem) {
	#ifdef DEBUG
		bool isPresent = false;
		for (GLint error = glGetError(); error; error = glGetError()) {
			GALE_ERROR("after " << _op << "() line=" << _localLine << " glError(" << error << ")");
			isPresent = true;
		}
		if (isPresent == true) {
			GALE_ERROR("    in program name : " << m_name);
			GALE_ERROR("    program OpenGL ID =" << m_program);
			GALE_ERROR("    List Uniform:");
			int32_t id = 0;
			for (auto &it : m_elementList) {
				if (it.m_isAttribute == false) {
					GALE_ERROR("      " << (id==_idElem?"*":" ") << " name :" << it.m_name << " OpenGL ID=" << it.m_elementId << " is linked=" << it.m_isLinked);
				}
				id++;
			}
			GALE_ERROR("    List Attribute:");
			id = 0;
			for (auto &it : m_elementList) {
				if (it.m_isAttribute == true) {
					GALE_ERROR("      " << (id==_idElem?"*":" ") << " name :" << it.m_name << " OpenGL ID=" << it.m_elementId << " is linked=" << it.m_isLinked);
				}
				id++;
			}
			GALE_CRITICAL("Stop on openGL ERROR");
		}
	#endif
}

#define LOG_OGL_INTERNAL_BUFFER_LEN    (8192)
static char l_bufferDisplayError[LOG_OGL_INTERNAL_BUFFER_LEN] = "";


bool gale::resource::Program::checkIdValidity(int32_t _idElem) {
	std::unique_lock<std::recursive_mutex> lock(m_mutex);
	if (    _idElem < 0
	     || (size_t)_idElem > m_elementList.size()) {
		return false;
	}
	return m_elementList[_idElem].m_isLinked;
}

int32_t gale::resource::Program::getAttribute(std::string _elementName) {
	std::unique_lock<std::recursive_mutex> lock(m_mutex);
	// check if it exist previously :
	for(size_t iii=0; iii<m_elementList.size(); iii++) {
		if (m_elementList[iii].m_name == _elementName) {
			return iii;
		}
	}
	progAttributeElement tmp;
	tmp.m_name = _elementName;
	tmp.m_isAttribute = true;
	if (gale::openGL::hasContext() == false) {
		getManager().update(std::dynamic_pointer_cast<gale::Resource>(shared_from_this()));
		tmp.m_elementId = -1;
		tmp.m_isLinked = false;
	} else if (m_exist == true) {
		tmp.m_elementId = gale::openGL::program::getAttributeLocation(m_program, tmp.m_name);
		tmp.m_isLinked = true;
		if (tmp.m_elementId<0) {
			GALE_WARNING("    {" << m_program << "}[" << m_elementList.size() << "] glGetAttribLocation(\"" << tmp.m_name << "\") = " << tmp.m_elementId);
			tmp.m_isLinked = false;
		} else {
			GALE_INFO("    {" << m_program << "}[" << m_elementList.size() << "] glGetAttribLocation(\"" << tmp.m_name << "\") = " << tmp.m_elementId);
		}
	} else {
		// program is not loaded ==> just local reister ...
		tmp.m_elementId = -1;
		tmp.m_isLinked = false;
	}
	m_elementList.push_back(tmp);
	return m_elementList.size()-1;
}

int32_t gale::resource::Program::getUniform(std::string _elementName) {
	std::unique_lock<std::recursive_mutex> lock(m_mutex);
	// check if it exist previously :
	for(size_t iii=0; iii<m_elementList.size(); iii++) {
		if (m_elementList[iii].m_name == _elementName) {
			return iii;
		}
	}
	progAttributeElement tmp;
	tmp.m_name = _elementName;
	tmp.m_isAttribute = false;
	if (gale::openGL::hasContext() == false) {
		getManager().update(std::dynamic_pointer_cast<gale::Resource>(shared_from_this()));
		tmp.m_elementId = -1;
		tmp.m_isLinked = false;
	} else if (m_exist == true) {
		tmp.m_elementId = gale::openGL::program::getUniformLocation(m_program, tmp.m_name);
		tmp.m_isLinked = true;
		if (tmp.m_elementId<0) {
			GALE_WARNING("    {" << m_program << "}[" << m_elementList.size() << "] glGetUniformLocation(\"" << tmp.m_name << "\") = " << tmp.m_elementId);
			tmp.m_isLinked = false;
		} else {
			GALE_INFO("    {" << m_program << "}[" << m_elementList.size() << "] glGetUniformLocation(\"" << tmp.m_name << "\") = " << tmp.m_elementId);
		}
	} else {
		// program is not loaded ==> just local reister ...
		tmp.m_elementId = -1;
		tmp.m_isLinked = false;
	}
	m_elementList.push_back(tmp);
	return m_elementList.size()-1;
}

bool gale::resource::Program::updateContext() {
	std::unique_lock<std::recursive_mutex> lock(m_mutex, std::defer_lock);
	if (lock.try_lock() == false) {
		//Lock error ==> try later ...
		return false;
	}
	if (m_exist == true) {
		// Do nothing  == > too dangerous ...
	} else {
		// create the Shader
		GALE_INFO("Create the Program ... \"" << m_name << "\"");
		m_program = gale::openGL::program::create();
		if (m_program < 0) {
			return true;
		}
		// first attach vertex shader, and after fragment shader
		for (size_t iii=0; iii<m_shaderList.size(); iii++) {
			if (nullptr != m_shaderList[iii]) {
				if (m_shaderList[iii]->getShaderType() == gale::openGL::shader::type_vertex) {
					gale::openGL::program::attach(m_program, m_shaderList[iii]->getGL_ID());
				}
			}
		}
		for (size_t iii=0; iii<m_shaderList.size(); iii++) {
			if (nullptr != m_shaderList[iii]) {
				if (m_shaderList[iii]->getShaderType() == gale::openGL::shader::type_fragment) {
					gale::openGL::program::attach(m_program, m_shaderList[iii]->getGL_ID());
				}
			}
		}
		if (gale::openGL::program::compile(m_program) == false) {
			GALE_ERROR("Could not compile \"PROGRAM\": \"" << m_name << "\"");
			gale::openGL::program::remove(m_program);
			return true;
		}
		// now get the old attribute requested priviously ...
		size_t iii = 0;
		for(auto &it : m_elementList) {
			if (it.m_isAttribute == true) {
				it.m_elementId = gale::openGL::program::getAttributeLocation(m_program, it.m_name);
				it.m_isLinked = true;
				if (it.m_elementId<0) {
					GALE_WARNING("    {" << m_program << "}[" << iii << "] openGL::getAttributeLocation(\"" << it.m_name << "\") = " << it.m_elementId);
					it.m_isLinked = false;
				} else {
					GALE_DEBUG("    {" << m_program << "}[" << iii << "] openGL::getAttributeLocation(\"" << it.m_name << "\") = " << it.m_elementId);
				}
			} else {
				it.m_elementId = gale::openGL::program::getUniformLocation(m_program, it.m_name);
				it.m_isLinked = true;
				if (it.m_elementId < 0) {
					GALE_WARNING("    {" << m_program << "}[" << iii << "] openGL::getUniformLocation(\"" << it.m_name << "\") = " << it.m_elementId);
					it.m_isLinked = false;
				} else {
					GALE_DEBUG("    {" << m_program << "}[" << iii << "] openGL::getUniformLocation(\"" << it.m_name << "\") = " << it.m_elementId);
				}
			}
			iii++;
		}
		// It will existed only when all is updated...
		m_exist = true;
	}
	return true;
}

void gale::resource::Program::removeContext() {
	std::unique_lock<std::recursive_mutex> lock(m_mutex);
	if (m_exist == true) {
		gale::openGL::program::remove(m_program);
		m_program = 0;
		m_exist = false;
		for(auto &it : m_elementList) {
			it.m_elementId=0;
			it.m_isLinked = false;
		}
	}
}

void gale::resource::Program::removeContextToLate() {
	std::unique_lock<std::recursive_mutex> lock(m_mutex);
	m_exist = false;
	m_program = 0;
}

void gale::resource::Program::reload() {
/* TODO : ...
	etk::file file(m_name, etk::FILE_TYPE_DATA);
	if (false == file.Exist()) {
		GALE_ERROR("File does not Exist : \"" << file << "\"");
		return;
	}
	
	int32_t fileSize = file.size();
	if (0 == fileSize) {
		GALE_ERROR("This file is empty : " << file);
		return;
	}
	if (false == file.fOpenRead()) {
		GALE_ERROR("Can not open the file : " << file);
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
		GALE_ERROR("Error Memory allocation size=" << fileSize);
		return;
	}
	memset(m_fileData, 0, (fileSize+5)*sizeof(char));
	// load data from the file :
	file.fRead(m_fileData, 1, fileSize);
	// close the file:
	file.fClose();
*/
	// now change the OGL context ...
	removeContext();
	updateContext();
}

//////////////////////////////////////////////////////////////////////////////////////////////

void gale::resource::Program::sendAttribute(int32_t _idElem,
                                            int32_t _nbElement,
                                            const void* _pointer,
                                            int32_t _jumpBetweenSample) {
	std::unique_lock<std::recursive_mutex> lock(m_mutex);
	if (m_exist == false) {
		return;
	}
	if (    _idElem < 0
	     || (size_t)_idElem > m_elementList.size()) {
		GALE_ERROR("idElem = " << _idElem << " not in [0.." << (m_elementList.size()-1) << "]");
		return;
	}
	if (m_elementList[_idElem].m_isLinked == false) {
		return;
	}
	//GALE_ERROR("[" << m_elementList[_idElem].m_name << "] send " << _nbElement << " element");
	glVertexAttribPointer(m_elementList[_idElem].m_elementId, // attribute ID of openGL
	                      _nbElement, // number of elements per vertex, here (r,g,b,a)
	                      GL_FLOAT, // the type of each element
	                      GL_FALSE, // take our values as-is
	                      _jumpBetweenSample, // no extra data between each position
	                      _pointer); // Pointer on the buffer
	checkGlError("glVertexAttribPointer", __LINE__, _idElem);
	glEnableVertexAttribArray(m_elementList[_idElem].m_elementId);
	checkGlError("glEnableVertexAttribArray", __LINE__, _idElem);
}

void gale::resource::Program::sendAttributePointer(int32_t _idElem,
                                                   const std::shared_ptr<gale::resource::VirtualBufferObject>& _vbo,
                                                   int32_t _index,
                                                   int32_t _jumpBetweenSample,
                                                   int32_t _offset) {
	std::unique_lock<std::recursive_mutex> lock(m_mutex);
	if (m_exist == false) {
		return;
	}
	if (_idElem<0 || (size_t)_idElem>m_elementList.size()) {
		GALE_ERROR("idElem = " << _idElem << " not in [0.." << (m_elementList.size()-1) << "]");
		return;
	}
	if (false == m_elementList[_idElem].m_isLinked) {
		return;
	}
	// check error of the VBO goog enought ...
	if (_vbo->getElementSize(_index) <= 0) {
		GALE_ERROR("Can not bind a VBO Buffer with an element size of : " << _vbo->getElementSize(_index) << " named=" << _vbo->getName());
		return;
	}
	
	GALE_VERBOSE("[" << m_elementList[_idElem].m_name << "] send " << _vbo->getElementSize(_index) << " element on oglID=" << _vbo->getGL_ID(_index) << " VBOindex=" << _index);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo->getGL_ID(_index));
	checkGlError("glBindBuffer", __LINE__, _idElem);
	GALE_VERBOSE("    id=" << m_elementList[_idElem].m_elementId);
	GALE_VERBOSE("    eleme size=" << _vbo->getElementSize(_index));
	GALE_VERBOSE("    jump sample=" << _jumpBetweenSample);
	GALE_VERBOSE("    offset=" << _offset);
	glVertexAttribPointer(m_elementList[_idElem].m_elementId, // attribute ID of openGL
	                      _vbo->getElementSize(_index), // number of elements per vertex, here (r,g,b,a)
	                      GL_FLOAT, // the type of each element
	                      GL_FALSE, // take our values as-is
	                      _jumpBetweenSample, // no extra data between each position
	                      (GLvoid *)_offset); // Pointer on the buffer
	checkGlError("glVertexAttribPointer", __LINE__, _idElem);
	glEnableVertexAttribArray(m_elementList[_idElem].m_elementId);
	checkGlError("glEnableVertexAttribArray", __LINE__, _idElem);
}

//////////////////////////////////////////////////////////////////////////////////////////////

void gale::resource::Program::uniformMatrix(int32_t _idElem, const mat4& _matrix, bool _transpose) {
	std::unique_lock<std::recursive_mutex> lock(m_mutex);
	if (m_exist == false) {
		return;
	}
	if (_idElem<0 || (size_t)_idElem>m_elementList.size()) {
		GALE_ERROR("idElem = " << _idElem << " not in [0.." << (m_elementList.size()-1) << "]");
		return;
	}
	if (false == m_elementList[_idElem].m_isLinked) {
		return;
	}
	//GALE_ERROR("[" << m_elementList[_idElem].m_name << "] send 1 matrix");
	// note : Android des not supported the transposition of the matrix, then we will done it oursef:
	if (true == _transpose) {
		mat4 tmp = _matrix;
		tmp.transpose();
		glUniformMatrix4fv(m_elementList[_idElem].m_elementId, 1, GL_FALSE, tmp.m_mat);
	} else {
		glUniformMatrix4fv(m_elementList[_idElem].m_elementId, 1, GL_FALSE, _matrix.m_mat);
	}
	checkGlError("glUniformMatrix4fv", __LINE__, _idElem);
}

//////////////////////////////////////////////////////////////////////////////////////////////

void gale::resource::Program::uniform1f(int32_t _idElem, float _value1) {
	std::unique_lock<std::recursive_mutex> lock(m_mutex);
	if (m_exist == false) {
		return;
	}
	if (_idElem<0 || (size_t)_idElem>m_elementList.size()) {
		GALE_ERROR("idElem = " << _idElem << " not in [0.." << (m_elementList.size()-1) << "]");
		return;
	}
	if (false == m_elementList[_idElem].m_isLinked) {
		return;
	}
	glUniform1f(m_elementList[_idElem].m_elementId, _value1);
	checkGlError("glUniform1f", __LINE__, _idElem);
}
void gale::resource::Program::uniform2f(int32_t _idElem, float  _value1, float _value2) {
	std::unique_lock<std::recursive_mutex> lock(m_mutex);
	if (m_exist == false) {
		return;
	}
	if (_idElem<0 || (size_t)_idElem>m_elementList.size()) {
		GALE_ERROR("idElem = " << _idElem << " not in [0.." << (m_elementList.size()-1) << "]");
		return;
	}
	if (false == m_elementList[_idElem].m_isLinked) {
		return;
	}
	glUniform2f(m_elementList[_idElem].m_elementId, _value1, _value2);
	checkGlError("glUniform2f", __LINE__, _idElem);
}
void gale::resource::Program::uniform3f(int32_t _idElem, float _value1, float _value2, float _value3) {
	std::unique_lock<std::recursive_mutex> lock(m_mutex);
	if (m_exist == false) {
		return;
	}
	if (_idElem<0 || (size_t)_idElem>m_elementList.size()) {
		GALE_ERROR("idElem = " << _idElem << " not in [0.." << (m_elementList.size()-1) << "]");
		return;
	}
	if (false == m_elementList[_idElem].m_isLinked) {
		return;
	}
	glUniform3f(m_elementList[_idElem].m_elementId, _value1, _value2, _value3);
	checkGlError("glUniform3f", __LINE__, _idElem);
}
void gale::resource::Program::uniform4f(int32_t _idElem, float _value1, float _value2, float _value3, float _value4) {
	std::unique_lock<std::recursive_mutex> lock(m_mutex);
	if (m_exist == false) {
		return;
	}
	if (_idElem<0 || (size_t)_idElem>m_elementList.size()) {
		GALE_ERROR("idElem = " << _idElem << " not in [0.." << (m_elementList.size()-1) << "]");
		return;
	}
	if (false == m_elementList[_idElem].m_isLinked) {
		return;
	}
	glUniform4f(m_elementList[_idElem].m_elementId, _value1, _value2, _value3, _value4);
	checkGlError("glUniform4f", __LINE__, _idElem);
}

//////////////////////////////////////////////////////////////////////////////////////////////

void gale::resource::Program::uniform1i(int32_t _idElem, int32_t _value1) {
	std::unique_lock<std::recursive_mutex> lock(m_mutex);
	if (m_exist == false) {
		return;
	}
	if (_idElem<0 || (size_t)_idElem>m_elementList.size()) {
		GALE_ERROR("idElem = " << _idElem << " not in [0.." << (m_elementList.size()-1) << "]");
		return;
	}
	if (false == m_elementList[_idElem].m_isLinked) {
		return;
	}
	glUniform1i(m_elementList[_idElem].m_elementId, _value1);
	checkGlError("glUniform1i", __LINE__, _idElem);
}
void gale::resource::Program::uniform2i(int32_t _idElem, int32_t _value1, int32_t _value2) {
	std::unique_lock<std::recursive_mutex> lock(m_mutex);
	if (m_exist == false) {
		return;
	}
	if (_idElem<0 || (size_t)_idElem>m_elementList.size()) {
		GALE_ERROR("idElem = " << _idElem << " not in [0.." << (m_elementList.size()-1) << "]");
		return;
	}
	if (false == m_elementList[_idElem].m_isLinked) {
		return;
	}
	glUniform2i(m_elementList[_idElem].m_elementId, _value1, _value2);
	checkGlError("glUniform2i", __LINE__, _idElem);
}
void gale::resource::Program::uniform3i(int32_t _idElem, int32_t _value1, int32_t _value2, int32_t _value3) {
	std::unique_lock<std::recursive_mutex> lock(m_mutex);
	if (m_exist == false) {
		return;
	}
	if (_idElem<0 || (size_t)_idElem>m_elementList.size()) {
		GALE_ERROR("idElem = " << _idElem << " not in [0.." << (m_elementList.size()-1) << "]");
		return;
	}
	if (false == m_elementList[_idElem].m_isLinked) {
		return;
	}
	glUniform3i(m_elementList[_idElem].m_elementId, _value1, _value2, _value3);
	checkGlError("glUniform3i", __LINE__, _idElem);
}
void gale::resource::Program::uniform4i(int32_t _idElem, int32_t _value1, int32_t _value2, int32_t _value3, int32_t _value4) {
	std::unique_lock<std::recursive_mutex> lock(m_mutex);
	if (m_exist == false) {
		return;
	}
	if (_idElem<0 || (size_t)_idElem>m_elementList.size()) {
		GALE_ERROR("idElem = " << _idElem << " not in [0.." << (m_elementList.size()-1) << "]");
		return;
	}
	if (false == m_elementList[_idElem].m_isLinked) {
		return;
	}
	glUniform4i(m_elementList[_idElem].m_elementId, _value1, _value2, _value3, _value4);
	checkGlError("glUniform4i", __LINE__, _idElem);
}


//////////////////////////////////////////////////////////////////////////////////////////////

void gale::resource::Program::uniform1fv(int32_t _idElem, int32_t _nbElement, const float *_value) {
	std::unique_lock<std::recursive_mutex> lock(m_mutex);
	if (m_exist == false) {
		return;
	}
	if (_idElem<0 || (size_t)_idElem>m_elementList.size()) {
		GALE_ERROR("idElem = " << _idElem << " not in [0.." << (m_elementList.size()-1) << "]");
		return;
	}
	if (false == m_elementList[_idElem].m_isLinked) {
		return;
	}
	if (0 == _nbElement) {
		GALE_ERROR("No element to send at open GL ...");
		return;
	}
	if (nullptr == _value) {
		GALE_ERROR("nullptr Input pointer to send at open GL ...");
		return;
	}
	glUniform1fv(m_elementList[_idElem].m_elementId, _nbElement, _value);
	checkGlError("glUniform1fv", __LINE__, _idElem);
}
void gale::resource::Program::uniform2fv(int32_t _idElem, int32_t _nbElement, const float *_value) {
	std::unique_lock<std::recursive_mutex> lock(m_mutex);
	if (m_exist == false) {
		return;
	}
	if (_idElem<0 || (size_t)_idElem>m_elementList.size()) {
		GALE_ERROR("idElem = " << _idElem << " not in [0.." << (m_elementList.size()-1) << "]");
		return;
	}
	if (false == m_elementList[_idElem].m_isLinked) {
		return;
	}
	if (0 == _nbElement) {
		GALE_ERROR("No element to send at open GL ...");
		return;
	}
	if (nullptr == _value) {
		GALE_ERROR("nullptr Input pointer to send at open GL ...");
		return;
	}
	glUniform2fv(m_elementList[_idElem].m_elementId, _nbElement, _value);
	checkGlError("glUniform2fv", __LINE__, _idElem);
}
void gale::resource::Program::uniform3fv(int32_t _idElem, int32_t _nbElement, const float *_value) {
	std::unique_lock<std::recursive_mutex> lock(m_mutex);
	if (m_exist == false) {
		return;
	}
	if (_idElem<0 || (size_t)_idElem>m_elementList.size()) {
		GALE_ERROR("idElem = " << _idElem << " not in [0.." << (m_elementList.size()-1) << "]");
		return;
	}
	if (false == m_elementList[_idElem].m_isLinked) {
		return;
	}
	if (0 == _nbElement) {
		GALE_ERROR("No element to send at open GL ...");
		return;
	}
	if (nullptr == _value) {
		GALE_ERROR("nullptr Input pointer to send at open GL ...");
		return;
	}
	GALE_VERBOSE("[" << m_elementList[_idElem].m_name << "] send " << _nbElement << " vec3");
	glUniform3fv(m_elementList[_idElem].m_elementId, _nbElement, _value);
	checkGlError("glUniform3fv", __LINE__, _idElem);
}
void gale::resource::Program::uniform4fv(int32_t _idElem, int32_t _nbElement, const float *_value) {
	std::unique_lock<std::recursive_mutex> lock(m_mutex);
	if (m_exist == false) {
		return;
	}
	if (_idElem<0 || (size_t)_idElem>m_elementList.size()) {
		GALE_ERROR("idElem = " << _idElem << " not in [0.." << (m_elementList.size()-1) << "]");
		return;
	}
	if (false == m_elementList[_idElem].m_isLinked) {
		return;
	}
	if (0 == _nbElement) {
		GALE_ERROR("No element to send at open GL ...");
		return;
	}
	if (nullptr == _value) {
		GALE_ERROR("nullptr Input pointer to send at open GL ...");
		return;
	}
	GALE_VERBOSE("[" << m_elementList[_idElem].m_name << "] send " << _nbElement << " vec4");
	glUniform4fv(m_elementList[_idElem].m_elementId, _nbElement, _value);
	checkGlError("glUniform4fv", __LINE__, _idElem);
}

//////////////////////////////////////////////////////////////////////////////////////////////

void gale::resource::Program::uniform1iv(int32_t _idElem, int32_t _nbElement, const int32_t *_value) {
	std::unique_lock<std::recursive_mutex> lock(m_mutex);
	if (m_exist == false) {
		return;
	}
	if (_idElem<0 || (size_t)_idElem>m_elementList.size()) {
		GALE_ERROR("idElem = " << _idElem << " not in [0.." << (m_elementList.size()-1) << "]");
		return;
	}
	if (false == m_elementList[_idElem].m_isLinked) {
		return;
	}
	if (0 == _nbElement) {
		GALE_ERROR("No element to send at open GL ...");
		return;
	}
	if (nullptr == _value) {
		GALE_ERROR("nullptr Input pointer to send at open GL ...");
		return;
	}
	glUniform1iv(m_elementList[_idElem].m_elementId, _nbElement, _value);
	checkGlError("glUniform1iv", __LINE__, _idElem);
}
void gale::resource::Program::uniform2iv(int32_t _idElem, int32_t _nbElement, const int32_t *_value) {
	std::unique_lock<std::recursive_mutex> lock(m_mutex);
	if (m_exist == false) {
		return;
	}
	if (_idElem<0 || (size_t)_idElem>m_elementList.size()) {
		GALE_ERROR("idElem = " << _idElem << " not in [0.." << (m_elementList.size()-1) << "]");
		return;
	}
	if (false == m_elementList[_idElem].m_isLinked) {
		return;
	}
	if (0 == _nbElement) {
		GALE_ERROR("No element to send at open GL ...");
		return;
	}
	if (nullptr == _value) {
		GALE_ERROR("nullptr Input pointer to send at open GL ...");
		return;
	}
	glUniform2iv(m_elementList[_idElem].m_elementId, _nbElement, _value);
	checkGlError("glUniform2iv", __LINE__, _idElem);
}
void gale::resource::Program::uniform3iv(int32_t _idElem, int32_t _nbElement, const int32_t *_value) {
	std::unique_lock<std::recursive_mutex> lock(m_mutex);
	if (m_exist == false) {
		return;
	}
	if (_idElem<0 || (size_t)_idElem>m_elementList.size()) {
		GALE_ERROR("idElem = " << _idElem << " not in [0.." << (m_elementList.size()-1) << "]");
		return;
	}
	if (false == m_elementList[_idElem].m_isLinked) {
		return;
	}
	if (0 == _nbElement) {
		GALE_ERROR("No element to send at open GL ...");
		return;
	}
	if (nullptr == _value) {
		GALE_ERROR("nullptr Input pointer to send at open GL ...");
		return;
	}
	glUniform3iv(m_elementList[_idElem].m_elementId, _nbElement, _value);
	checkGlError("glUniform3iv", __LINE__, _idElem);
}
void gale::resource::Program::uniform4iv(int32_t _idElem, int32_t _nbElement, const int32_t *_value) {
	std::unique_lock<std::recursive_mutex> lock(m_mutex);
	if (m_exist == false) {
		return;
	}
	if (_idElem<0 || (size_t)_idElem>m_elementList.size()) {
		GALE_ERROR("idElem = " << _idElem << " not in [0.." << (m_elementList.size()-1) << "]");
		return;
	}
	if (false == m_elementList[_idElem].m_isLinked) {
		return;
	}
	if (0 == _nbElement) {
		GALE_ERROR("No element to send at open GL ...");
		return;
	}
	if (nullptr == _value) {
		GALE_ERROR("nullptr Input pointer to send at open GL ...");
		return;
	}
	glUniform4iv(m_elementList[_idElem].m_elementId, _nbElement, _value);
	checkGlError("glUniform4iv", __LINE__, _idElem);
}

//#define PROGRAM_DISPLAY_SPEED

//////////////////////////////////////////////////////////////////////////////////////////////
#ifdef PROGRAM_DISPLAY_SPEED
	int64_t g_startTime = 0;
#endif


void gale::resource::Program::use() {
	#ifdef PROGRAM_DISPLAY_SPEED
		g_startTime = gale::getTime();
	#endif
	// event if it was 0  == > set it to prevent other use of the previous shader display ...
	gale::openGL::useProgram(m_program);
	checkGlError("glUseProgram", __LINE__);
}


void gale::resource::Program::setTexture0(int32_t _idElem, int64_t _textureOpenGlID) {
	std::unique_lock<std::recursive_mutex> lock(m_mutex);
	if (m_exist == false) {
		return;
	}
	if (_idElem<0 || (size_t)_idElem>m_elementList.size()) {
		return;
	}
	if (false == m_elementList[_idElem].m_isLinked) {
		return;
	}
	#if 0
		gale::openGL::enable(GL_TEXTURE_2D);
		checkGlError("glEnable", __LINE__);
	#endif
	gale::openGL::activeTexture(GL_TEXTURE0);
	checkGlError("glActiveTexture", __LINE__, _idElem);
	// set the textureID
	glBindTexture(GL_TEXTURE_2D, _textureOpenGlID);
	checkGlError("glBindTexture", __LINE__, _idElem);
	// set the texture on the uniform attribute
	glUniform1i(m_elementList[_idElem].m_elementId, /*GL_TEXTURE*/0);
	checkGlError("glUniform1i", __LINE__, _idElem);
	m_hasTexture = true;
}

void gale::resource::Program::setTexture1(int32_t _idElem, int64_t _textureOpenGlID) {
	std::unique_lock<std::recursive_mutex> lock(m_mutex);
	if (m_exist == false) {
		return;
	}
	if (_idElem<0 || (size_t)_idElem>m_elementList.size()) {
		return;
	}
	if (false == m_elementList[_idElem].m_isLinked) {
		return;
	}
	#if 0
		gale::openGL::enable(GL_TEXTURE_2D);
		checkGlError("glEnable", __LINE__);
	#endif
	gale::openGL::activeTexture(GL_TEXTURE1);
	checkGlError("glActiveTexture", __LINE__, _idElem);
	// set the textureID
	glBindTexture(GL_TEXTURE_2D, _textureOpenGlID);
	checkGlError("glBindTexture", __LINE__, _idElem);
	// set the texture on the uniform attribute
	glUniform1i(m_elementList[_idElem].m_elementId, /*GL_TEXTURE*/1);
	checkGlError("glUniform1i", __LINE__, _idElem);
	m_hasTexture1 = true;
}


void gale::resource::Program::unUse() {
	//GALE_WARNING("Will use program : " << m_program);
	std::unique_lock<std::recursive_mutex> lock(m_mutex);
	if (m_exist == false) {
		return;
	}
	#if 0
	if (true == m_hasTexture) {
		gale::openGL::disable(GL_TEXTURE_2D);
		//checkGlError("glDisable", __LINE__);
		m_hasTexture = false;
	}
	#endif
	// no need to disable program  == > this only generate perturbation on speed ...
	gale::openGL::useProgram(-1);
	#ifdef PROGRAM_DISPLAY_SPEED
		float localTime = (float)(gale::getTime() - g_startTime) / 1000.0f;
		if (localTime>1) {
			GALE_ERROR("      prog : " << localTime << "ms    resource=\"" << m_name << "\"");
		} else {
			GALE_DEBUG("      prog : " << localTime << "ms    resource=\"" << m_name << "\"");
		}
	#endif
	checkGlError("glUseProgram", __LINE__);
}

