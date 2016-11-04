/** @file
 * @author Edouard DUPIN
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */
#pragma once

#include <etk/types.hpp>
#include <etk/math/Vector2D.hpp>
#include <gale/debug.hpp>
#include <gale/resource/Resource.hpp>
#include <gale/renderer/openGL/openGL.hpp>
#include <etk/Color.hpp>

namespace gale {
	namespace resource {
		/**
		 * @brief VirtualBufferObject is a specific resources for opengl, this load the data directly in the graphic card ad keep these inside
		 */
		class VirtualBufferObject : public gale::Resource {
			private :
				bool m_exist;  //!< This data is availlable in the Graphic card
				std::vector<uint32_t> m_vbo; //!< openGl ID of this VBO
				std::vector<bool> m_vboUsed; //!< true if the VBO is allocated or used ...
				std::vector<std::vector<float>> m_buffer; //!< data that is availlable in the VBO system ...
				std::vector<int8_t> m_vboSizeDataOffset; //!< Internal size of the VBO (dynamicly set)
			protected:
				/**
				 * @brief Constructor of this VBO.
				 * @param[in] accesMode Acces mode : ???
				 */
				VirtualBufferObject();
				void init(int32_t _number);
			public:
				DECLARE_RESOURCE_FACTORY(VirtualBufferObject);
				/**
				 * @brief Destructor of this VBO.
				 */
				virtual ~VirtualBufferObject();
			public:
				/**
				 * @brief clear buffers
				 */
				void clear();
				/**
				 * @brief get the real openGL ID.
				 * @return the Ogl id reference of this VBO.
				 */
				int64_t getGL_ID(int32_t _id) const {
					return m_vbo[_id];
				};
				/**
				 * @brief get a reference on hte buffer data for this VBO.
				 * @param[in] id Id of the buffer requested
				 * @return A reference on the data.
				 */
				std::vector<float>& getRefBuffer(int32_t _id) {
					m_vboUsed[_id] = true;
					return m_buffer[_id];
				};
				/**
				 * @brief Get the buffer Number of element.
				 * @param[in] _id VBO Element
				 * @return Number of Float in the buffer.
				 */
				int32_t bufferSize(int32_t _id) const;
				/**
				 * @brief Get the offset between element.
				 * @param[in] _id VBO Element
				 * @return Number of Float to jump between target.
				 */
				int32_t getElementSize(int32_t _id) const;
				/**
				 * @brief push data on a buffer with a custum type :
				 * @param[in] _id Id of the buffer requested.
				 * @param[in] _data Direct data that might be set.
				 */
				void pushOnBuffer(int32_t _id, const vec3& _data);
				vec3 getOnBufferVec3(int32_t _id, int32_t _elementID) const;
				/**
				 * @brief push data on a buffer with a custum type :
				 * @param[in] _id Id of the buffer requested.
				 * @param[in] _data Direct data that might be set.
				 */
				void pushOnBuffer(int32_t _id, const vec2& _data);
				vec2 getOnBufferVec2(int32_t _id, int32_t _elementID) const;
				/**
				 * @brief push data on a buffer with a custum type :
				 * @param[in] _id Id of the buffer requested.
				 * @param[in] _data Direct data that might be set (Color).
				 */
				void pushOnBuffer(int32_t _id, const etk::Color<float,4>& _data);
				//! @previous
				void pushOnBuffer(int32_t _id, const etk::Color<float,3>& _data);
				//! @previous
				void pushOnBuffer(int32_t _id, const etk::Color<float,2>& _data);
				//! @previous
				void pushOnBuffer(int32_t _id, const etk::Color<float,1>& _data);
				/**
				 * @brief get the data from the graphic card.
				 */
				void retreiveData();
				/**
				 * @brief Send the data to the graphic card.
				 */
				void flush();
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
		};
	}
}

