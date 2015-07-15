/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license APACHE v2.0 (see license file)
 */

#ifndef __OPEN_GL__SHADER_H__
#define __OPEN_GL__SHADER_H__

#include <etk/types.h>
#include <gale/debug.h>
#include <gale/openGL/openGL.h>
#include <gale/resource/Resource.h>

namespace gale {
	namespace resource {
		/**
		 * @brief Shader is a specific resources for opengl, used only in @ref Program. This are components of the renderer pipe-line
		 */
		class Shader : public gale::Resource {
			private :
				bool m_exist; //!< The shader file existed and has been loaded
				char* m_fileData; //!< A copy of the data loaded from the file (usefull only when opengl context is removed)
				GLuint m_shader; //!< opengl id of this element
				GLenum m_type; //!< Type of the current shader(vertex/fragment)
			protected:
				/**
				 * @brief Contructor of an opengl Shader
				 * @param[in] filename Standard file name format. see @ref etk::FSNode
				 */
				Shader();
			public:
				void init(const std::string& _filename);
				DECLARE_RESOURCE_NAMED_FACTORY(Shader);
				/**
				 * @brief Destructor, remove the current Shader
				 */
				virtual ~Shader();
			public:
				/**
				 * @brief get the opengl reference id of this shader.
				 * @return The opengl id.
				 */
				GLuint getGL_ID() {
					return m_shader;
				};
				/**
				 * @brief get the opengl type of this shader.
				 * @return The type of this loaded shader.
				 */
				GLenum getShaderType() {
					return m_type;
				};
				/**
				 * @brief This load/reload the data in the opengl context, needed when removed previously.
				 */
				void updateContext();
				/**
				 * @brief remove the data from the opengl context.
				 */
				void removeContext();
				/**
				 * @brief Special android spec! It inform us that all context is removed and after notify us...
				 */
				void removeContextToLate();
				/**
				 * @brief Relode the shader from the file. used when a request of resouces reload is done.
				 * @note this is really usefull when we tested the new themes or shader developpements.
				 */
				void reload();
		};
	};
};

#endif

