/** @file
 * @author Edouard DUPIN
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license MPL v2.0 (see license file)
 */

#include <etk/types.hpp>
#include <gale/debug.hpp>
#include <gale/resource/Manager.hpp>
#include <gale/resource/VirtualBufferObject.hpp>
#include <gale/renderer/openGL/openGL-include.hpp>
#include <etk/typeInfo.hpp>
ETK_DECLARE_TYPE(gale::resource::VirtualBufferObject);

void gale::resource::VirtualBufferObject::init(int32_t _number) {
	gale::Resource::init();
	m_vbo.resize(_number, 0);
	m_vboUsed.resize(_number, false);
	m_buffer.resize(_number);
	m_vboSizeDataOffset.resize(_number, -1);
	GALE_DEBUG("OGL : load VBO count=\"" << _number << "\"");
}

void gale::resource::VirtualBufferObject::clear() {
	GALE_VERBOSE(" Clear: [" << getId() << "] '" << getName() << "' (size=" << m_buffer[0].size() << ")");
	// DO not clear the m_vbo indexed in the graphic cards ...
	for (size_t iii=0; iii<m_vboUsed.size(); ++iii) {
		m_vboUsed[iii] = false;
	}
	for (auto &it : m_buffer) {
		it.clear();
	}
	for (auto &it : m_vboSizeDataOffset) {
		it = -1;
	}
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
	GALE_VERBOSE(" Start: [" << getId() << "] '" << getName() << "' (size=" << m_buffer[0].size() << ")");
	ethread::RecursiveLock lock(m_mutex, true);
	if (lock.tryLock() == false) {
		//Lock error ==> try later ...
		GALE_WARNING("     ==> Lock error on VBO");
		return false;
	}
	if (m_exist == false) {
		GALE_DEBUG("     ==> ALLOCATE new handle");
		// Allocate and assign a Vertex Array Object to our handle
		gale::openGL::genBuffers(m_vbo);
	}
	m_exist = true;
	for (size_t iii=0; iii<m_vbo.size(); iii++) {
		GALE_VERBOSE("VBO    : add [" << getId() << "]=" << m_buffer[iii].size() << "*sizeof(float) OGl_Id=" << m_vbo[iii]);
		if (m_vboUsed[iii] == true) {
			// select the buffer to set data inside it ...
			if (m_buffer[iii].size() > 0) {
				gale::openGL::bindBuffer(m_vbo[iii]);
				float* bufferPonter =&(m_buffer[iii][0]);
				gale::openGL::bufferData(sizeof(float)*m_buffer[iii].size(), bufferPonter, gale::openGL::usage::streamDraw);
			}
		}
	}
	// un-bind it to permet to have no error in the next display ...
	gale::openGL::unbindBuffer();
	GALE_VERBOSE(" Stop: [" << getId() << "] '" << getName() << "'");
	return true;
}

void gale::resource::VirtualBufferObject::removeContext() {
	ethread::RecursiveLock lock(m_mutex);
	if (m_exist == true) {
		gale::openGL::deleteBuffers(m_vbo);
		m_exist = false;
	}
}

void gale::resource::VirtualBufferObject::removeContextToLate() {
	ethread::RecursiveLock lock(m_mutex);
	m_exist = false;
	for (size_t iii=0; iii<m_vbo.size(); iii++) {
		m_vbo[iii] = 0;
	}
}

void gale::resource::VirtualBufferObject::reload() {
	ethread::RecursiveLock lock(m_mutex);
	removeContext();
	updateContext();
}

void gale::resource::VirtualBufferObject::flush() {
	ethread::RecursiveLock lock(m_mutex);
	// request to the manager to be call at the next update ...
	getManager().update(ememory::dynamicPointerCast<gale::Resource>(sharedFromThis()));
	GALE_VERBOSE("Request flush of VBO: [" << getId() << "] '" << getName() << "'");
}

void gale::resource::VirtualBufferObject::pushOnBuffer(int32_t _id, const vec3& _data) {
	ethread::RecursiveLock lock(m_mutex);
	if (m_vboSizeDataOffset[_id] == -1) {
		m_vboSizeDataOffset[_id] = 3;
	} else if (m_vboSizeDataOffset[_id] != 3) {
		GALE_WARNING("set multiType in VBO (Not supported ==> TODO : Maybe update it");
		return;
	}
	m_vboUsed[_id] = true;
	m_buffer[_id].pushBack(_data.x());
	m_buffer[_id].pushBack(_data.y());
	m_buffer[_id].pushBack(_data.z());
}

vec3 gale::resource::VirtualBufferObject::getOnBufferVec3(int32_t _id, int32_t _elementID) const {
	ethread::RecursiveLock lock(m_mutex);
	if ((size_t)_elementID*3 > m_buffer[_id].size()) {
		return vec3(0,0,0);
	}
	return vec3(m_buffer[_id][3*_elementID],
	            m_buffer[_id][3*_elementID+1],
	            m_buffer[_id][3*_elementID+2]);
}

int32_t gale::resource::VirtualBufferObject::bufferSize(int32_t _id) const {
	ethread::RecursiveLock lock(m_mutex);
	return m_buffer[_id].size()/m_vboSizeDataOffset[_id];
}
int32_t gale::resource::VirtualBufferObject::getElementSize(int32_t _id) const {
	ethread::RecursiveLock lock(m_mutex);
	return m_vboSizeDataOffset[_id];
}

void gale::resource::VirtualBufferObject::pushOnBuffer(int32_t _id, const vec2& _data) {
	ethread::RecursiveLock lock(m_mutex);
	if (m_vboSizeDataOffset[_id] == -1) {
		m_vboSizeDataOffset[_id] = 2;
	} else if (m_vboSizeDataOffset[_id] != 2) {
		GALE_WARNING("set multiType in VBO (Not supported ==> TODO : Maybe update it");
		return;
	}
	m_vboUsed[_id] = true;
	m_buffer[_id].pushBack(_data.x());
	m_buffer[_id].pushBack(_data.y());
}

vec2 gale::resource::VirtualBufferObject::getOnBufferVec2(int32_t _id, int32_t _elementID) const {
	ethread::RecursiveLock lock(m_mutex);
	if ((size_t)_elementID*2 > m_buffer[_id].size()) {
		return vec2(0,0);
	}
	return vec2(m_buffer[_id][2*_elementID],
	            m_buffer[_id][2*_elementID+1]);
}

void gale::resource::VirtualBufferObject::pushOnBuffer(int32_t _id, const float& _data) {
	ethread::RecursiveLock lock(m_mutex);
	if (m_vboSizeDataOffset[_id] == -1) {
		m_vboSizeDataOffset[_id] = 1;
	} else if (m_vboSizeDataOffset[_id] != 1) {
		GALE_WARNING("set multiType in VBO (Not supported ==> TODO : Maybe update it");
		return;
	}
	m_vboUsed[_id] = true;
	m_buffer[_id].pushBack(_data);
}

void gale::resource::VirtualBufferObject::pushOnBuffer(int32_t _id, const etk::Color<float,4>& _data) {
	ethread::RecursiveLock lock(m_mutex);
	if (m_vboSizeDataOffset[_id] == -1) {
		m_vboSizeDataOffset[_id] = 4;
	} else if (m_vboSizeDataOffset[_id] != 4) {
		GALE_WARNING("set multiType in VBO (Not supported ==> TODO : Maybe update it");
		return;
	}
	m_vboUsed[_id] = true;
	m_buffer[_id].pushBack(_data.r());
	m_buffer[_id].pushBack(_data.g());
	m_buffer[_id].pushBack(_data.b());
	m_buffer[_id].pushBack(_data.a());
}

void gale::resource::VirtualBufferObject::pushOnBuffer(int32_t _id, const etk::Color<float,3>& _data) {
	ethread::RecursiveLock lock(m_mutex);
	if (m_vboSizeDataOffset[_id] == -1) {
		m_vboSizeDataOffset[_id] = 3;
	} else if (m_vboSizeDataOffset[_id] != 3) {
		GALE_WARNING("set multiType in VBO (Not supported ==> TODO : Maybe update it");
		return;
	}
	m_vboUsed[_id] = true;
	m_buffer[_id].pushBack(_data.r());
	m_buffer[_id].pushBack(_data.g());
	m_buffer[_id].pushBack(_data.b());
}

void gale::resource::VirtualBufferObject::pushOnBuffer(int32_t _id, const etk::Color<float,2>& _data) {
	ethread::RecursiveLock lock(m_mutex);
	if (m_vboSizeDataOffset[_id] == -1) {
		m_vboSizeDataOffset[_id] = 2;
	} else if (m_vboSizeDataOffset[_id] != 2) {
		GALE_WARNING("set multiType in VBO (Not supported ==> TODO : Maybe update it");
		return;
	}
	m_vboUsed[_id] = true;
	m_buffer[_id].pushBack(_data.r());
	m_buffer[_id].pushBack(_data.g());
}

void gale::resource::VirtualBufferObject::pushOnBuffer(int32_t _id, const etk::Color<float,1>& _data) {
	ethread::RecursiveLock lock(m_mutex);
	if (m_vboSizeDataOffset[_id] == -1) {
		m_vboSizeDataOffset[_id] = 1;
	} else if (m_vboSizeDataOffset[_id] != 1) {
		GALE_WARNING("set multiType in VBO (Not supported ==> TODO : Maybe update it");
		return;
	}
	m_vboUsed[_id] = true;
	m_buffer[_id].pushBack(_data.r());
}

