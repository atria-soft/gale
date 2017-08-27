/** @file
 * @author Edouard DUPIN
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license MPL v2.0 (see license file)
 */

#include <etk/types.hpp>
#include <gale/gale.hpp>
#include <gale/renderer/openGL/openGL.hpp>
#include <gale/resource/Manager.hpp>
#include <gale/resource/Texture.hpp>
#include <gale/renderer/openGL/openGL-include.hpp>

/**
 * @brief get the next power 2 if the input
 * @param[in] value Value that we want the next power of 2
 * @return result value
 */
static int32_t nextP2(int32_t _value) {
	int32_t val=1;
	for (int32_t iii=1; iii<31; iii++) {
		if (_value <= val) {
			return val;
		}
		val *=2;
	}
	GALE_CRITICAL("impossible CASE....");
	return val;
}


void gale::resource::Texture::init(const etk::String& _filename) {
	gale::Resource::init(_filename);
}
void gale::resource::Texture::init() {
	gale::Resource::init();
}

gale::resource::Texture::Texture() :
  m_texId(0),
  m_endPointSize(1,1),
  m_loaded(false),
  m_size(0,0),
  m_dataType(gale::resource::Texture::dataType::int16),
  m_dataColorSpace(gale::resource::Texture::color::mono) {
	addResourceType("gale::compositing::Texture");
}

gale::resource::Texture::~Texture() {
	removeContext();
}

bool gale::resource::Texture::updateContext() {
	std::unique_lock<std::recursive_mutex> lock(m_mutex, std::defer_lock);
	if (lock.try_lock() == false) {
		//Lock error ==> try later ...
		return false;
	}
	if (false == m_loaded) {
		// Request a new texture at openGl :
		glGenTextures(1, &m_texId);
	}
	// in all case we set the texture properties :
	// TODO : check error ???
	glBindTexture(GL_TEXTURE_2D, m_texId);
	// TODO : Check error ???
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//--- mode nearest
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//--- Mode linear
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	GALE_INFO("TEXTURE: add [" << getId() << "]=" << m_size << " OGl_Id=" << m_texId);
	glTexImage2D(GL_TEXTURE_2D, // Target
	             0, // Level
	             GL_RGBA, // Format internal
	             m_size.x(),
	             m_size.y(),
	             0, // Border
	             GL_RGBA, // format
	             GL_UNSIGNED_BYTE, // type
	             &((*m_data)[0]) );
	// now the data is loaded
	m_loaded = true;
	return true;
}

void gale::resource::Texture::removeContext() {
	std::unique_lock<std::recursive_mutex> lock(m_mutex);
	if (true == m_loaded) {
		// Request remove texture ...
		GALE_INFO("TEXTURE: Rm [" << getId() << "] texId=" << m_texId);
		//glDeleteTextures(1, &m_texId);
		m_loaded = false;
	}
}

void gale::resource::Texture::removeContextToLate() {
	std::unique_lock<std::recursive_mutex> lock(m_mutex);
	m_loaded = false;
	m_texId=0;
}

void gale::resource::Texture::flush() {
	std::unique_lock<std::recursive_mutex> lock(m_mutex);
	// request to the manager to be call at the next update ...
	getManager().update(ememory::dynamicPointerCast<gale::Resource>(sharedFromThis()));
}

void gale::resource::Texture::setTexture(const ememory::SharedPtr<etk::Vector<char>>& _data,
                                         const ivec2& _size,
                                         enum gale::resource::Texture::dataType _dataType,
                                         enum gale::resource::Texture::color _dataColorSpace) {
	std::unique_lock<std::recursive_mutex> lock(m_mutex);
	m_data = _data;
	m_size = _size;
	m_endPointSize = _size;
	m_dataType = _dataType;
	m_dataColorSpace = _dataColorSpace;
	// TODO : Reload ...
	flush();
}
