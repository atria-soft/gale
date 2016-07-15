/** @file
 * @author Edouard DUPIN
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */
#pragma once

#include <etk/types.h>
#include <gale/debug.h>
#include <gale/renderer/openGL/openGL.h>
#include <gale/resource/Resource.h>

namespace gale {
	namespace resource {
		class Texture : public gale::Resource {
			public:
				enum class color {
					mono = 0, //!< Monochrome color
					rgb, //!< red/green/blue data
					rgba //!< red/green/blue/alpha data
				};
				enum dataType {
					int16 = 0, //!< Image data are stored on integer 16 bit for each element
					float32, //!< Image data are stored on flaoting point value on 32 bit for each element
				};
			protected:
				uint32_t m_texId; //!< openGl textureID.
				vec2 m_endPointSize; //!< some image are not square  == > we need to sqared it to prevent some openGl api error the the displayable size is not all the time 0.0 -> 1.0.
				bool m_loaded; //!< internal state of the openGl system.
			// Gale internal API:
			public:
				virtual bool updateContext();
				virtual void removeContext();
				virtual void removeContextToLate();
			// middleware interface:
			public:
				uint32_t getRendererId() const {
					return m_texId;
				};
				const vec2& getUsableSize() const {
					return m_endPointSize;
				};
				const ivec2& getOpenGlSize() const {
					return m_size;
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
				// flush the data to send it at the openGl system
				void flush();
			
			private:
				// Image propoerties:
				ememory::SharedPtr<std::vector<char>> m_data; //!< pointer on the image data.
				ivec2 m_size; //!< size of the image data.
				enum dataType m_dataType; //!< Type of the image.
				enum color m_dataColorSpace; //!< Color space of the image.
			public:
				void setTexture(const ememory::SharedPtr<std::vector<char>>& _data,
				                const ivec2& _size,
				                enum gale::resource::Texture::dataType _dataType,
				                enum gale::resource::Texture::color _dataColorSpace);
		};
	}
}

