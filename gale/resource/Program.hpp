/** @file
 * @author Edouard DUPIN
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */
#pragma once

#include <etk/types.hpp>
#include <etk/math/Vector4D.hpp>
#include <gale/renderer/openGL/openGL.hpp>
#include <gale/resource/Resource.hpp>
#include <gale/resource/Shader.hpp>
#include <gale/resource/VirtualBufferObject.hpp>
#include <etk/Color.hpp>

namespace gale {
	namespace resource {
		/**
		 * @brief In a openGL program we need some data to communicate with them, we register all the name requested by the user in this structure:
		 * @note Register all requested element permit to abstract the fact that some element does not exist and remove control of existance from upper code.
		 *       This is important to note when the Program is reloaded the elements availlable can change.
		 * @not_in_doc
		 */
		class progAttributeElement {
			public :
				std::string m_name; //!< Name of the element
				int32_t m_elementId; //!< openGl Id if this element  == > can not exist ==> @ref m_isLinked
				bool m_isAttribute; //!< true if it was an attribute element, otherwite it was an uniform
				bool m_isLinked; //!< if this element does not exist this is false
		};
		//! @not_in_doc
		std::ostream& operator <<(std::ostream& _os, const gale::resource::progAttributeElement& _obj);
		//! @not_in_doc
		std::ostream& operator <<(std::ostream& _os, const std::vector<gale::resource::progAttributeElement>& _obj);
		/**
		 * @brief Program is a compilation of some fragment Shader and vertex Shader. This construct automaticly this assiciation
		 * The input file must have the form : "myFile.prog"
		 * The data is simple :
		 * <pre>
		 * # Comment line ... paid attention at the space at the end of lines, they are considered like a part of the file ...
		 * # The folder is automaticly get from the program file basic folder
		 * filename1.vert
		 * filename2.frag
		 * filename3.vert
		 * filename4.frag
		 * </pre>
		 */
		class Program : public gale::Resource {
			private :
				bool m_exist; //!< the file existed
				int64_t m_program; //!< openGL id of the current program
				std::vector<ememory::SharedPtr<gale::resource::Shader>> m_shaderList; //!< List of all the shader loaded
				std::vector<gale::resource::progAttributeElement> m_elementList; //!< List of all the attribute requested by the user
				bool m_hasTexture; //!< A texture has been set to the current shader
				bool m_hasTexture1; //!< A texture has been set to the current shader
			protected:
				/**
				 * @brief Contructor of an opengl Program.
				 * @param[in] filename Standard file name format. see @ref etk::FSNode
				 */
				Program();
				void init(const std::string& _filename);
			public:
				DECLARE_RESOURCE_NAMED_FACTORY(Program);
				/**
				 * @brief Destructor, remove the current Program.
				 */
				virtual ~Program();
			public:
				/**
				 * @brief Check If an Id is valid in the shader or not (sometime the shader have not some attribute, then we need to display some error)
				 * @return _idElem Id of the Attribute that might be sended.
				 * @return true The id is valid, false otherwise
				 */
				bool checkIdValidity(int32_t _idElem);
				/**
				 * @brief User request an attribute on this program.
				 * @note The attribute is send to the fragment shaders
				 * @param[in] _elementName Name of the requested attribute.
				 * @return An abstract ID of the current attribute (this value is all time availlable, even if the program will be reloaded)
				 */
				int32_t getAttribute(std::string _elementName);
				/**
				 * @brief Send attribute table to the spefified ID attribure (not send if does not really exist in the openGL program).
				 * @param[in] _idElem Id of the Attribute that might be sended.
				 * @param[in] _vbo Reference on the buffer to send.
				 * @param[in] _index Reference on the buffer to send.
				 * @param[in] _jumpBetweenSample Number of byte to jump between 2 vertex (this permit to enterlace informations)
				 * @param[in] _offset offset of start the elements send.
				 */
				void sendAttributePointer(int32_t _idElem,
				                          const ememory::SharedPtr<gale::resource::VirtualBufferObject>& _vbo,
				                          int32_t _index,
				                          int32_t _jumpBetweenSample=0,
				                          int32_t _offset=0);
				#ifndef THIS_IS_DEPRECATED_TO_REMOVE
					/**
					 * @brief Send attribute table to the spefified ID attribure (not send if does not really exist in the openGL program).
					 * @param[in] _idElem Id of the Attribute that might be sended.
					 * @param[in] _nbElement Specifies the number of elements that are to be modified.
					 * @param[in] _pointer Pointer on the data that might be sended.
					 * @param[in] _jumpBetweenSample Number of byte to jump between 2 vertex (this permit to enterlace informations)
					 */
					void sendAttribute(int32_t _idElem,
					                   int32_t _nbElement,
					                   const void* _pointer,
					                   int32_t _jumpBetweenSample=0);
					inline void sendAttribute(int32_t _idElem, const std::vector<vec2>& _data) {
						sendAttribute(_idElem, 2/*u,v / x,y*/, &_data[0]);
					}
					inline void sendAttribute(int32_t _idElem, const std::vector<vec3>& _data) {
						sendAttribute(_idElem, 3/*x,y,z,unused*/, &_data[0], 4*sizeof(btScalar));
					}
					inline void sendAttribute(int32_t _idElem, const std::vector<etk::Color<float>>& _data) {
						sendAttribute(_idElem, 4/*r,g,b,a*/, &_data[0]);
					}
					inline void sendAttribute(int32_t _idElem, const std::vector<float>& _data) {
						sendAttribute(_idElem, 1, &_data[0]);
					}
				#endif
				/**
				 * @brief User request an Uniform on this program.
				 * @note uniform value is availlable for all the fragment shader in the program (only one value for all)
				 * @param[in] _elementName Name of the requested uniform.
				 * @return An abstract ID of the current uniform (this value is all time availlable, even if the program will be reloaded)
				 */
				int32_t getUniform(std::string _elementName);
				/**
				 * @brief Send a uniform element to the spefified ID (not send if does not really exist in the openGL program)
				 * @param[in] _idElem Id of the uniform that might be sended.
				 * @param[in] _matrix Matrix that might be sended.
				 * @param[in] _transpose Transpose the matrix (needed all the taime in the normal openGl access (only not done in the openGL-ES2 due to the fact we must done it ourself)
				 */
				void uniformMatrix(int32_t _idElem, const mat4& _matrix, bool _transpose=true);
				
				inline void uniform(int32_t _idElem, const etk::Color<float>& _value) {
					uniform4f(_idElem, _value.r(), _value.g(), _value.b(), _value.a());
				}
				
				/**
				 * @brief Send 1 float uniform element to the spefified ID (not send if does not really exist in the openGL program)
				 * @param[in] _idElem Id of the uniform that might be sended.
				 * @param[in] _value1 Value to send at the Uniform
				 */
				void uniform1f(int32_t _idElem, float _value1);
				/**
				 * @brief Send 2 float uniform element to the spefified ID (not send if does not really exist in the openGL program)
				 * @param[in] _idElem Id of the uniform that might be sended.
				 * @param[in] _value1 Value to send at the Uniform
				 * @param[in] _value2 Value to send at the Uniform
				 */
				void uniform2f(int32_t _idElem, float _value1, float _value2);
				/**
				 * @brief Send 3 float uniform element to the spefified ID (not send if does not really exist in the openGL program)
				 * @param[in] _idElem Id of the uniform that might be sended.
				 * @param[in] _value1 Value to send at the Uniform
				 * @param[in] _value2 Value to send at the Uniform
				 * @param[in] _value3 Value to send at the Uniform
				 */
				void uniform3f(int32_t _idElem, float _value1, float _value2, float _value3);
				/**
				 * @brief Send 4 float uniform element to the spefified ID (not send if does not really exist in the openGL program)
				 * @param[in] _idElem Id of the uniform that might be sended.
				 * @param[in] _value1 Value to send at the Uniform
				 * @param[in] _value2 Value to send at the Uniform
				 * @param[in] _value3 Value to send at the Uniform
				 * @param[in] _value4 Value to send at the Uniform
				 */
				void uniform4f(int32_t _idElem, float _value1, float _value2, float _value3, float _value4);
				
				/**
				 * @brief Send 1 signed integer uniform element to the spefified ID (not send if does not really exist in the openGL program)
				 * @param[in] _idElem Id of the uniform that might be sended.
				 * @param[in] _value1 Value to send at the Uniform
				 */
				void uniform1i(int32_t _idElem, int32_t _value1);
				/**
				 * @brief Send 2 signed integer uniform element to the spefified ID (not send if does not really exist in the openGL program)
				 * @param[in] _idElem Id of the uniform that might be sended.
				 * @param[in] _value1 Value to send at the Uniform
				 * @param[in] _value2 Value to send at the Uniform
				 */
				void uniform2i(int32_t _idElem, int32_t _value1, int32_t _value2);
				/**
				 * @brief Send 3 signed integer uniform element to the spefified ID (not send if does not really exist in the openGL program)
				 * @param[in] _idElem Id of the uniform that might be sended.
				 * @param[in] _value1 Value to send at the Uniform
				 * @param[in] _value2 Value to send at the Uniform
				 * @param[in] _value3 Value to send at the Uniform
				 */
				void uniform3i(int32_t _idElem, int32_t _value1, int32_t _value2, int32_t _value3);
				/**
				 * @brief Send 4 signed integer uniform element to the spefified ID (not send if does not really exist in the openGL program)
				 * @param[in] _idElem Id of the uniform that might be sended.
				 * @param[in] _value1 Value to send at the Uniform
				 * @param[in] _value2 Value to send at the Uniform
				 * @param[in] _value3 Value to send at the Uniform
				 * @param[in] _value4 Value to send at the Uniform
				 */
				void uniform4i(int32_t _idElem, int32_t _value1, int32_t _value2, int32_t _value3, int32_t _value4);
				
				/**
				 * @brief Send "vec1" uniform element to the spefified ID (not send if does not really exist in the openGL program)
				 * @param[in] _idElem Id of the uniform that might be sended.
				 * @param[in] _nbElement Number of element sended
				 * @param[in] _value Pointer on the data
				 */
				void uniform1fv(int32_t _idElem, int32_t _nbElement, const float *_value);
				/**
				 * @brief Send "vec2" uniform element to the spefified ID (not send if does not really exist in the openGL program)
				 * @param[in] _idElem Id of the uniform that might be sended.
				 * @param[in] _nbElement Number of element sended
				 * @param[in] _value Pointer on the data
				 */
				void uniform2fv(int32_t _idElem, int32_t _nbElement, const float *_value);
				/**
				 * @brief Send "vec3" uniform element to the spefified ID (not send if does not really exist in the openGL program)
				 * @param[in] _idElem Id of the uniform that might be sended.
				 * @param[in] _nbElement Number of element sended
				 * @param[in] _value Pointer on the data
				 */
				void uniform3fv(int32_t _idElem, int32_t _nbElement, const float *_value);
				/**
				 * @brief Send "vec4" uniform element to the spefified ID (not send if does not really exist in the openGL program)
				 * @param[in] _idElem Id of the uniform that might be sended.
				 * @param[in] _nbElement Number of element sended
				 * @param[in] _value Pointer on the data
				 */
				void uniform4fv(int32_t _idElem, int32_t _nbElement, const float *_value);
				
				/**
				 * @brief Send "ivec1" uniform element to the spefified ID (not send if does not really exist in the openGL program)
				 * @param[in] _idElem Id of the uniform that might be sended.
				 * @param[in] _nbElement Number of element sended
				 * @param[in] _value Pointer on the data
				 */
				void uniform1iv(int32_t _idElem, int32_t _nbElement, const int32_t *_value);
				/**
				 * @brief Send "ivec2" uniform element to the spefified ID (not send if does not really exist in the openGL program)
				 * @param[in] _idElem Id of the Attribute that might be sended.
				 * @param[in] _nbElement Number of element sended
				 * @param[in] _value Pointer on the data
				 */
				void uniform2iv(int32_t _idElem, int32_t _nbElement, const int32_t *_value);
				/**
				 * @brief Send "ivec3" uniform element to the spefified ID (not send if does not really exist in the openGL program)
				 * @param[in] _idElem Id of the uniform that might be sended.
				 * @param[in] _nbElement Number of element sended
				 * @param[in] _value Pointer on the data
				 */
				void uniform3iv(int32_t _idElem, int32_t _nbElement, const int32_t *_value);
				/**
				 * @brief Send "ivec4" uniform element to the spefified ID (not send if does not really exist in the openGL program)
				 * @param[in] _idElem Id of the uniform that might be sended.
				 * @param[in] _nbElement Number of element sended
				 * @param[in] _value Pointer on the data
				 */
				void uniform4iv(int32_t _idElem, int32_t _nbElement, const int32_t *_value);
				
				inline void uniform2(int32_t _idElem, const vec2& _value) {
					uniform2fv(_idElem, 1, &_value.m_floats[0]);
				};
				inline void uniform3(int32_t _idElem, const vec3& _value) {
					uniform3fv(_idElem, 1, &_value.m_floats[0]);
				};
				inline void uniform4(int32_t _idElem, const vec4& _value) {
					uniform4fv(_idElem, 1, &_value.m_floats[0]);
				};
				inline void uniform2(int32_t _idElem, const ivec2& _value) {
					uniform2iv(_idElem, 1, &_value.m_floats[0]);
				};
				inline void uniform3(int32_t _idElem, const ivec3& _value) {
					uniform3iv(_idElem, 1, &_value.m_floats[0]);
				};
				inline void uniform4(int32_t _idElem, const ivec4& _value) {
					uniform4iv(_idElem, 1, &_value.m_floats[0]);
				};
				
				/**
				 * @brief Request the processing of this program
				 */
				void use();
				/**
				 * @brief set the testure Id on the specify uniform element.
				 * @param[in] _idElem Id of the uniform that might be sended.
				 * @param[in] _textureOpenGlID Real openGL texture ID
				 */
				void setTexture0(int32_t _idElem, int64_t _textureOpenGlID);
				void setTexture1(int32_t _idElem, int64_t _textureOpenGlID);
				/**
				 * @brief Stop the processing of this program
				 */
				void unUse();
				/**
				 * @brief This load/reload the data in the opengl context, needed when removed previously.
				 */
				bool updateContext();
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
			private:
				void checkGlError(const char* _op, int32_t _localLine, int32_t _idElem=-2);
		};
	}
}

