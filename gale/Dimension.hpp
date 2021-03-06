/** @file
 * @author Edouard DUPIN
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license MPL v2.0 (see license file)
 */
#pragma once

#include <etk/types.hpp>
#include <etk/math/Vector2D.hpp>

namespace gale {
	enum class distance {
		pourcent=0,
		pixel,
		meter,
		centimeter,
		millimeter,
		kilometer,
		inch,
		foot,
	};
	/**
	 * @brief in the dimention class we store the data as the more usefull unit (pixel) 
	 * but one case need to be dynamic the %, then when requested in % the register the % value
	 */
	class Dimension {
		public:
		private:
			vec2 m_data;
			enum distance m_type;
		public:
			/**
			 * @brief Constructor (default :0,0 mode pixel)
			 */
			Dimension();
			/**
			 * @brief Constructor
			 * @param[in] _size Requested dimention
			 * @param[in] _type Unit of the Dimention
			 */
			Dimension(const vec2& _size, enum gale::distance _type=gale::distance::pixel);
			/**
			 * @brief Constructor
			 * @param[in] _config dimension configuration.
			 */
			Dimension(const etk::String& _config) :
			  m_data(0,0),
			  m_type(gale::distance::pixel) {
				set(_config);
			};
			/**
			 * @brief Constructor
			 * @param[in] _config dimension configuration.
			 */
			Dimension(const char* _config) :
			  m_data(0,0),
			  m_type(gale::distance::pixel) {
				set(_config);
			};
			/**
			 * @brief Destructor
			 */
			~Dimension();
			
			/**
			 * @brief string cast :
			 */
			operator etk::String() const;
			
			/**
			 * @brief get the current dimention in requested type
			 * @param[in] _type Type of unit requested.
			 * @return dimention requested.
			 */
			vec2 get(enum distance _type) const;
			/**
			 * @brief set the current dimention in requested type
			 * @param[in] _size Dimention to set
			 * @param[in] _type Type of unit requested.
			 */
			void set(const vec2& _size, enum distance _type);
			
		private:
			/**
			 * @brief set the current dimention in requested type
			 * @param[in] _config dimension configuration.
			 */
			void set(etk::String _config);
		public:
			/**
			 * @brief get the current dimention in pixel
			 * @return dimention in Pixel
			 */
			vec2 getPixel() const;
			/**
			 * @brief get the current dimention in Pourcent
			 * @return dimention in Pourcent
			 */
			vec2 getPourcent() const;
			/**
			 * @brief get the current dimention in Meter
			 * @return dimention in Meter
			 */
			vec2 getMeter() const;
			/**
			 * @brief get the current dimention in Centimeter
			 * @return dimention in Centimeter
			 */
			vec2 getCentimeter() const;
			/**
			 * @brief get the current dimention in Millimeter
			 * @return dimention in Millimeter
			 */
			vec2 getMillimeter() const;
			/**
			 * @brief get the current dimention in Kilometer
			 * @return dimention in Kilometer
			 */
			vec2 getKilometer() const;
			/**
			 * @brief get the current dimention in Inch
			 * @return dimention in Inch
			 */
			vec2 getInch() const;
			/**
			 * @brief get the current dimention in Foot
			 * @return dimention in Foot
			 */
			vec2 getFoot() const;
			/*****************************************************
			 *    = assigment
			 *****************************************************/
			const Dimension& operator= (const Dimension& _obj ) {
				if (this!=&_obj) {
					m_data = _obj.m_data;
					m_type = _obj.m_type;
				}
				return *this;
			}
			/*****************************************************
			 *    == operator
			 *****************************************************/
			bool operator ==  (const Dimension& _obj) const {
				if(    m_data == _obj.m_data
				    && m_type == _obj.m_type) {
					return true;
				}
				return false;
			}
			/*****************************************************
			 *    != operator
			 *****************************************************/
			bool operator!= (const Dimension& _obj) const {
				if(    m_data != _obj.m_data
				    || m_type != _obj.m_type) {
					return true;
				}
				return false;
			}
			/**
			 * @breif get the dimension type
			 * @return the type
			 */
			enum distance getType() const {
				return m_type;
			};
		public : // Global static access :
		/**
		 * @brief basic init
		 */
		static void init();
		/**
		 * @brief basic un-init
		 */
		static void unInit();
		/**
		 * @brief set the Milimeter ratio for calculation
		 * @param[in] Ratio Milimeter ration for the screen calculation interpolation
		 * @param[in] type Unit type requested.
		 * @note: same as @ref setPixelPerInch (internal manage convertion)
		 */
		static void setPixelRatio(const vec2& _ratio, enum gale::distance _type);
		/**
		 * @brief set the current Windows size
		 * @param[in] size size of the current windows in pixel.
		 */
		static void setPixelWindowsSize(const vec2& _size);
		/**
		 * @brief get the Windows size in the request unit
		 * @param[in] type Unit type requested.
		 * @return the requested size
		 */
		static vec2 getWindowsSize(enum gale::distance _type);
		/**
		 * @brief get the Windows diagonal size in the request unit
		 * @param[in] type Unit type requested.
		 * @return the requested size
		 */
		static float getWindowsDiag(enum gale::distance _type);
		
	};
	etk::Stream& operator <<(etk::Stream& _os, enum gale::distance _obj);
	etk::Stream& operator <<(etk::Stream& _os, const gale::Dimension& _obj);
}

