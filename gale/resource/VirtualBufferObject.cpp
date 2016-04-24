/** @file
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license APACHE v2.0 (see license file)
 */

#include <etk/types.h>
#include <gale/debug.h>
#include <gale/resource/Manager.h>
#include <gale/resource/VirtualBufferObject.h>
#include <gale/renderer/openGL/openGL-include.h>

void gale::resource::VirtualBufferObject::init(int32_t _number) {
	gale::Resource::init();
	m_vbo.resize(_number, 0);
	m_vboUsed.resize(_number, false);
	m_buffer.resize(_number);
	m_vboSizeDataOffset.resize(_number, -1);
	GALE_DEBUG("OGL : load VBO count=\"" << _number << "\"");
}

gale::resource::VirtualBufferObject::VirtualBufferObject() :
  m_exist(false) {
	addResourceType("gale::VirtualBufferObject");
	m_resourceLevel = 3;
}

gale::resource::VirtualBufferObject::~VirtualBufferObject() {
	removeContext();
}

void gale::resource::VirtualBufferObject::retreiveData() {
	GALE_ERROR("TODO ... ");
}

bool gale::resource::VirtualBufferObject::updateContext() {
	GALE_ERROR(" Start");
	std::unique_lock<std::recursive_mutex> lock(m_mutex, std::defer_lock);
	if (lock.try_lock() == false) {
		//Lock error ==> try later ...
		return false;
	}
	if (false == m_exist) {
		// Allocate and assign a Vertex Array Object to our handle
		gale::openGL::genBuffers(m_vbo);
	}
	m_exist = true;
	for (size_t iii=0; iii<m_vbo.size(); iii++) {
		GALE_INFO("VBO    : add [" << getId() << "]=" << m_buffer[iii].size() << "*sizeof(float) OGl_Id=" << m_vbo[iii]);
		if (true == m_vboUsed[iii]) {
			// select the buffer to set data inside it ...
			if (m_buffer[iii].size()>0) {
				gale::openGL::bindBuffer(m_vbo[iii]);
				gale::openGL::bufferData(sizeof(float)*m_buffer[iii].size(), &((m_buffer[iii])[0]), gale::openGL::usage_staticDraw);
			}
		}
	}
	// un-bind it to permet to have no error in the next display ...
	gale::openGL::unbindBuffer();
	GALE_ERROR(" Stop");
	return true;
}

void gale::resource::VirtualBufferObject::removeContext() {
	std::unique_lock<std::recursive_mutex> lock(m_mutex);
	if (m_exist == true) {
		gale::openGL::deleteBuffers(m_vbo);
		m_exist = false;
	}
}

void gale::resource::VirtualBufferObject::removeContextToLate() {
	std::unique_lock<std::recursive_mutex> lock(m_mutex);
	m_exist = false;
	for (size_t iii=0; iii<m_vbo.size(); iii++) {
		m_vbo[iii] = 0;
	}
}

void gale::resource::VirtualBufferObject::reload() {
	std::unique_lock<std::recursive_mutex> lock(m_mutex);
	removeContext();
	updateContext();
}

void gale::resource::VirtualBufferObject::flush() {
	std::unique_lock<std::recursive_mutex> lock(m_mutex);
	// request to the manager to be call at the next update ...
	getManager().update(std::dynamic_pointer_cast<gale::Resource>(shared_from_this()));
}

void gale::resource::VirtualBufferObject::pushOnBuffer(int32_t _id, const vec3& _data) {
	std::unique_lock<std::recursive_mutex> lock(m_mutex);
	if (m_vboSizeDataOffset[_id] == -1) {
		m_vboSizeDataOffset[_id] = 3;
	} else if (m_vboSizeDataOffset[_id] != 3) {
		GALE_WARNING("set multiType in VBO (Not supported ==> TODO : Maybe update it");
		return;
	}
	m_vboUsed[_id] = true;
	m_buffer[_id].push_back(_data.x());
	m_buffer[_id].push_back(_data.y());
	m_buffer[_id].push_back(_data.z());
}

vec3 gale::resource::VirtualBufferObject::getOnBufferVec3(int32_t _id, int32_t _elementID) {
	std::unique_lock<std::recursive_mutex> lock(m_mutex);
	if ((size_t)_elementID*3 > m_buffer[_id].size()) {
		return vec3(0,0,0);
	}
	return vec3(m_buffer[_id][3*_elementID],
	            m_buffer[_id][3*_elementID+1],
	            m_buffer[_id][3*_elementID+2]);
}

int32_t gale::resource::VirtualBufferObject::bufferSize(int32_t _id) {
	std::unique_lock<std::recursive_mutex> lock(m_mutex);
	return m_buffer[_id].size()/m_vboSizeDataOffset[_id];
}
int32_t gale::resource::VirtualBufferObject::getElementSize(int32_t _id) {
	std::unique_lock<std::recursive_mutex> lock(m_mutex);
	return m_vboSizeDataOffset[_id];
}

void gale::resource::VirtualBufferObject::pushOnBuffer(int32_t _id, const vec2& _data) {
	std::unique_lock<std::recursive_mutex> lock(m_mutex);
	if (m_vboSizeDataOffset[_id] == -1) {
		m_vboSizeDataOffset[_id] = 2;
	} else if (m_vboSizeDataOffset[_id] != 2) {
		GALE_WARNING("set multiType in VBO (Not supported ==> TODO : Maybe update it");
		return;
	}
	m_vboUsed[_id] = true;
	m_buffer[_id].push_back(_data.x());
	m_buffer[_id].push_back(_data.y());
}

vec2 gale::resource::VirtualBufferObject::getOnBufferVec2(int32_t _id, int32_t _elementID) {
	std::unique_lock<std::recursive_mutex> lock(m_mutex);
	if ((size_t)_elementID*2 > m_buffer[_id].size()) {
		return vec2(0,0);
	}
	return vec2(m_buffer[_id][2*_elementID],
	            m_buffer[_id][2*_elementID+1]);
}


void gale::resource::VirtualBufferObject::pushOnBuffer(int32_t _id, const etk::Color<float,4>& _data) {
	std::unique_lock<std::recursive_mutex> lock(m_mutex);
	if (m_vboSizeDataOffset[_id] == -1) {
		m_vboSizeDataOffset[_id] = 4;
	} else if (m_vboSizeDataOffset[_id] != 4) {
		GALE_WARNING("set multiType in VBO (Not supported ==> TODO : Maybe update it");
		return;
	}
	m_vboUsed[_id] = true;
	m_buffer[_id].push_back(_data.r());
	m_buffer[_id].push_back(_data.g());
	m_buffer[_id].push_back(_data.b());
	m_buffer[_id].push_back(_data.a());
}

void gale::resource::VirtualBufferObject::pushOnBuffer(int32_t _id, const etk::Color<float,3>& _data) {
	std::unique_lock<std::recursive_mutex> lock(m_mutex);
	if (m_vboSizeDataOffset[_id] == -1) {
		m_vboSizeDataOffset[_id] = 3;
	} else if (m_vboSizeDataOffset[_id] != 3) {
		GALE_WARNING("set multiType in VBO (Not supported ==> TODO : Maybe update it");
		return;
	}
	m_vboUsed[_id] = true;
	m_buffer[_id].push_back(_data.r());
	m_buffer[_id].push_back(_data.g());
	m_buffer[_id].push_back(_data.b());
}

void gale::resource::VirtualBufferObject::pushOnBuffer(int32_t _id, const etk::Color<float,2>& _data) {
	std::unique_lock<std::recursive_mutex> lock(m_mutex);
	if (m_vboSizeDataOffset[_id] == -1) {
		m_vboSizeDataOffset[_id] = 2;
	} else if (m_vboSizeDataOffset[_id] != 2) {
		GALE_WARNING("set multiType in VBO (Not supported ==> TODO : Maybe update it");
		return;
	}
	m_vboUsed[_id] = true;
	m_buffer[_id].push_back(_data.r());
	m_buffer[_id].push_back(_data.g());
}

void gale::resource::VirtualBufferObject::pushOnBuffer(int32_t _id, const etk::Color<float,1>& _data) {
	std::unique_lock<std::recursive_mutex> lock(m_mutex);
	if (m_vboSizeDataOffset[_id] == -1) {
		m_vboSizeDataOffset[_id] = 1;
	} else if (m_vboSizeDataOffset[_id] != 1) {
		GALE_WARNING("set multiType in VBO (Not supported ==> TODO : Maybe update it");
		return;
	}
	m_vboUsed[_id] = true;
	m_buffer[_id].push_back(_data.r());
}

