/** @file
 * @author Edouard DUPIN
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license MPL v2.0 (see license file)
 */
package org.atriasoft.gale;

import org.atriasoft.etk.math.Vector2f;
import org.atriasoft.gale.internal.Log;

/**
 * @brief in the dimention class we store the data as the more usefull unit (pixel) 
 * but one case need to be dynamic the %, then when requested in % the register the % value
 */
public class Dimension {
	private final static Vector2f ratio = new Vector2f(9999999, 888888);
	private final static Vector2f invRatio = new Vector2f(1, 1);
	private final static Dimension windowsSize = new Dimension(new Vector2f(9999999, 888888), Distance.PIXEL);
	
	public final static float INCH_TO_MILLIMETER = 1.0f / 25.4f;
	public final static float FOOT_TO_MILLIMETER = 1.0f / 304.8f;
	public final static float METER_TO_MILLIMETER = 1.0f / 1000.0f;
	public final static float CENTIMETER_TO_MILLIMETER = 1.0f / 10.0f;
	public final static float KILOMETER_TO_MILLIMETER = 1.0f / 1000000.0f;
	public final static float MILLIMETER_TO_INCH = 25.4f;
	public final static float MILLIMETER_TO_FOOT = 304.8f;
	public final static float MILLIMETER_TO_METER = 1000.0f;
	public final static float MILLIMETER_TO_CENTIMETER = 10.0f;
	public final static float MILLIMETER_TO_KILOMETER = 1000000.0f;
	/**
	 * @brief basic init
	 */
	static {
		final Dimension conversion = new Dimension(new Vector2f(72, 72), Distance.INCH);
		ratio.set(conversion.getMillimeter());
		invRatio.setValue(1.0f / ratio.x, 1.0f / ratio.y);
		windowsSize.set(new Vector2f(200, 200), Distance.PIXEL);
	}
	
	/**
	 * @brief get the Windows diagonal size in the request unit
	 * @param[in] type Unit type requested.
	 * @return the requested size
	 */
	public static float getWindowsDiag(final Distance _type) {
		final Vector2f size = getWindowsSize(_type);
		return size.length();
	}
	
	/**
	 * @brief get the Windows size in the request unit
	 * @param[in] type Unit type requested.
	 * @return the requested size
	 */
	public static Vector2f getWindowsSize(final Distance _type) {
		return windowsSize.get(_type);
	}
	
	/**
	 * @brief set the Milimeter ratio for calculation
	 * @param[in] Ratio Milimeter ration for the screen calculation interpolation
	 * @param[in] type Unit type requested.
	 * @note: same as @ref setPixelPerInch (internal manage convertion)
	 */
	public static void setPixelRatio(final Vector2f _ratio, final Distance _type) {
		Log.info("Set a new screen ratio for the screen : ratio=" + _ratio + " type=" + _type);
		final Dimension conversion = new Dimension(_ratio, _type);
		Log.info("     == > " + conversion);
		ratio.set(conversion.getMillimeter());
		invRatio.setValue(1.0f / ratio.x, 1.0f / ratio.y);
		Log.info("Set a new screen ratio for the screen : ratioMm=" + ratio);
	}
	
	/**
	 * @brief set the current Windows size
	 * @param[in] size size of the current windows in pixel.
	 */
	public static void setPixelWindowsSize(final Vector2f _size) {
		windowsSize.set(_size);
		Log.verbose("Set a new Windows property size " + windowsSize + "px");
	}
	
	private Vector2f size = new Vector2f(0, 0);
	private Distance type = Distance.PIXEL;
	
	/**
	 * @brief Constructor (default :0,0 mode pixel)
	 */
	public Dimension() {
		
	}
	
	/**
	 * @brief Constructor
	 * @param[in] _config dimension configuration.
	 */
	public Dimension(final String _config) {
		set(_config);
	}
	
	/**
	 * @brief Constructor
	 * @param[in] _size Requested dimension
	 * @param[in] _type Unit of the Dimension
	 */
	public Dimension(final Vector2f _size) {
		set(_size, Distance.PIXEL);
	}
	
	public Dimension(final Vector2f _size, final Distance _type) {
		set(_size, _type);
	}
	
	/*****************************************************
	 * isEqual
	 *****************************************************/
	@Override
	public boolean equals(final Object obj) {
		if (obj == null) {
			return false;
		}
		if (obj == this) {
			return true;
		}
		if (!(obj instanceof Dimension)) {
			return false;
		}
		final Dimension other = (Dimension) obj;
		return this.size.equals(other.size) && this.type == other.type;
	}
	
	/**
	 * @brief get the current dimention in requested type
	 * @param[in] _type Type of unit requested.
	 * @return dimention requested.
	 */
	public Vector2f get(final Distance _type) {
		switch (_type) {
			case POURCENT:
				return getPourcent();
			case PIXEL:
				return getPixel();
			case METER:
				return getMeter();
			case CENTIMETER:
				return getCentimeter();
			case MILLIMETER:
				return getMillimeter();
			case KILOMETER:
				return getKilometer();
			case INCH:
				return getInch();
			case FOOT:
				return getFoot();
		}
		return new Vector2f(0, 0);
	}
	
	/**
	 * @brief get the current dimention in Centimeter
	 * @return dimention in Centimeter
	 */
	public Vector2f getCentimeter() {
		return getMillimeter().multiplyNew(MILLIMETER_TO_CENTIMETER);
	}
	
	/**
	 * @brief get the current dimention in Foot
	 * @return dimention in Foot
	 */
	public Vector2f getFoot() {
		return getMillimeter().multiplyNew(MILLIMETER_TO_FOOT);
	}
	
	/**
	 * @brief get the current dimention in Inch
	 * @return dimention in Inch
	 */
	public Vector2f getInch() {
		return getMillimeter().multiplyNew(MILLIMETER_TO_INCH);
	}
	
	/**
	 * @brief get the current dimention in Kilometer
	 * @return dimention in Kilometer
	 */
	public Vector2f getKilometer() {
		return getMillimeter().multiplyNew(MILLIMETER_TO_KILOMETER);
	}
	
	/**
	 * @brief get the current dimention in Meter
	 * @return dimention in Meter
	 */
	public Vector2f getMeter() {
		return getMillimeter().multiplyNew(MILLIMETER_TO_METER);
	}
	
	/**
	 * @brief get the current dimention in Millimeter
	 * @return dimention in Millimeter
	 */
	public Vector2f getMillimeter() {
		return new Vector2f(getPixel().x * invRatio.x, getPixel().y * invRatio.y);
	}
	
	/**
	 * @brief get the current dimention in pixel
	 * @return dimention in Pixel
	 */
	public Vector2f getPixel() {
		if (this.type != Distance.POURCENT) {
			return this.size;
		} else {
			final Vector2f windDim = windowsSize.getPixel();
			final Vector2f res = new Vector2f(windDim.x * this.size.x, windDim.y * this.size.y);
			//GALE_DEBUG("Get % : " + m_data + " / " + windDim + " == > " + res);
			return res;
		}
	}
	
	/**
	 * @brief get the current dimention in Pourcent
	 * @return dimention in Pourcent
	 */
	public Vector2f getPourcent() {
		if (this.type != Distance.POURCENT) {
			final Vector2f windDim = windowsSize.getPixel();
			//GALE_DEBUG(" windows dimention : " /*+ windowsSize*/ + "  == > " + windDim + "px"); // ==> infinite loop ...
			//printf(" windows dimention : %f,%f", windDim.x(),windDim.y());
			//printf(" data : %f,%f", m_data.x(),m_data.y());
			return new Vector2f((this.size.x / windDim.x) * 100.0f, (this.size.y / windDim.y) * 100.0f);
		}
		return new Vector2f(this.size.x * 100.0f, this.size.y * 100.0f);
	};
	
	/**
	 * @breif get the dimension type
	 * @return the type
	 */
	public Distance getType() {
		return this.type;
	}
	
	/*****************************************************
	 * assigment
	 *****************************************************/
	public Dimension set(final Dimension _obj) {
		if (this != _obj) {
			this.size = _obj.size;
			this.type = _obj.type;
		}
		return this;
	}
	
	/**
	 * @brief set the current dimention in requested type
	 * @param[in] _config dimension configuration.
	 */
	private void set(String _config) {
		this.size.setValue(0, 0);
		this.type = Distance.PIXEL;
		Distance type = Distance.PIXEL;
		if (_config.endsWith("%") == true) {
			type = Distance.POURCENT;
			_config = _config.substring(0, _config.length() - 1);
		} else if (_config.endsWith("px") == true) {
			type = Distance.PIXEL;
			_config = _config.substring(0, _config.length() - 2);
		} else if (_config.endsWith("ft") == true) {
			type = Distance.FOOT;
			_config = _config.substring(0, _config.length() - 2);
		} else if (_config.endsWith("in") == true) {
			type = Distance.INCH;
			_config = _config.substring(0, _config.length() - 2);
		} else if (_config.endsWith("km") == true) {
			type = Distance.KILOMETER;
			_config = _config.substring(0, _config.length() - 2);
		} else if (_config.endsWith("mm") == true) {
			type = Distance.MILLIMETER;
			_config = _config.substring(0, _config.length() - 2);
		} else if (_config.endsWith("cm") == true) {
			type = Distance.CENTIMETER;
			_config = _config.substring(0, _config.length() - 2);
		} else if (_config.endsWith("m") == true) {
			type = Distance.METER;
			_config = _config.substring(0, _config.length() - 1);
		} else {
			Log.critical("Can not parse dimention : '" + _config + "'");
			return;
		}
		final Vector2f tmp = Vector2f.valueOf(_config);
		set(tmp, type);
		Log.verbose(" config dimention : \"" + _config + "\"  == > " + this);
	}
	
	public void set(final Vector2f _size) {
		this.size = _size;
		this.type = Distance.PIXEL;
	}
	
	/**
	 * @brief set the current dimention in requested type
	 * @param[in] _size Dimention to set
	 * @param[in] _type Type of unit requested.
	 */
	public void set(final Vector2f _size, final Distance _type) {
		this.size = _size;
		this.type = _type;
	}
	
	/**
	 * @brief string cast :
	 */
	@Override
	public String toString() {
		String str = get(getType()).toString();
		switch (getType()) {
			case POURCENT:
				str += "%";
				break;
			case PIXEL:
				str += "px";
				break;
			case METER:
				str += "m";
				break;
			case CENTIMETER:
				str += "cm";
				break;
			case MILLIMETER:
				str += "mm";
				break;
			case KILOMETER:
				str += "km";
				break;
			case INCH:
				str += "in";
				break;
			case FOOT:
				str += "ft";
				break;
		}
		return str;
	}
	
}
