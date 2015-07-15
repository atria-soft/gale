/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license APACHE v2.0 (see license file)
 */

#ifndef __GALE_TEXTURE_H__
#define __GALE_TEXTURE_H__

#include <etk/types.h>
#include <gale/debug.h>
#include <egami/Image.h>
#include <gale/openGL/openGL.h>
#include <gale/resource/Resource.h>

namespace gale {
	namespace resource {
		class Texture : public gale::Resource {
			protected:
				// openGl Context propoerties :
				egami::Image m_data;
				// openGl textureID :
				GLuint m_texId;
				// some image are not square  == > we need to sqared it to prevent some openGl api error the the displayable size is not all the time 0.0 -> 1.0
				vec2 m_endPointSize;
				// internal state of the openGl system :
				bool m_loaded;
			// Gale internal API:
			public:
				void updateContext();
				void removeContext();
				void removeContextToLate();
			// middleware interface:
			public:
				GLuint getId() const {
					return m_texId;
				};
				const vec2& getUsableSize() const {
					return m_endPointSize;
				};
				const ivec2& getOpenGlSize() const {
					return m_data.getSize();
				};
			// Public API:
			protected:
				void init(const std::string& _filename);
				void init();
				Texture();
			public:
				DECLARE_RESOURCE_FACTORY(Texture);
				virtual ~Texture();
			public:
				// you must set the size here, because it will be set in multiple of pow(2)
				void setImageSize(ivec2 newSize);
				// get the reference on this image to draw nomething on it ...
				inline egami::Image& get() {
					return m_data;
				};
				// flush the data to send it at the openGl system
				void flush();
		};
	};
};

#endif

