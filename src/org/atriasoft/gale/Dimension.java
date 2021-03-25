/** @file
 * @author Edouard DUPIN
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license MPL v2.0 (see license file)
 */
package org.atriasoft.gale;

import org.atriasoft.etk.math.Vector2f;
import org.atriasoft.etk.math.Vector2i;
import org.atriasoft.gale.internal.Log;

/**
 * in the dimension class we store the data as the more usefull unit (pixel) 
 * but one case need to be dynamic the %, then when requested in % the register the % value
 */
@SuppressWarnings("preview")
public record Dimension(
		Vector2f size,
		Distance type) {
	public static final Dimension ZERO = new Dimension(Vector2f.ZERO, Distance.PIXEL);
	private static Vector2f ratio = new Vector2f(9999999, 888888);
	private static Vector2f invRatio = new Vector2f(1, 1);
	private static Dimension windowsSize = new Dimension(Vector2f.MAX_VALUE, Distance.PIXEL);
	
	public static final float INCH_TO_MILLIMETER = 1.0f / 25.4f;
	public static final float FOOT_TO_MILLIMETER = 1.0f / 304.8f;
	public static final float METER_TO_MILLIMETER = 1.0f / 1000.0f;
	public static final float CENTIMETER_TO_MILLIMETER = 1.0f / 10.0f;
	public static final float KILOMETER_TO_MILLIMETER = 1.0f / 1000000.0f;
	public static final float MILLIMETER_TO_INCH = 25.4f;
	public static final float MILLIMETER_TO_FOOT = 304.8f;
	public static final float MILLIMETER_TO_METER = 1000.0f;
	public static final float MILLIMETER_TO_CENTIMETER = 10.0f;
	public static final float MILLIMETER_TO_KILOMETER = 1000000.0f;
	/**
	 * basic init
	 */
	static {
		final Dimension conversion = new Dimension(new Vector2f(72, 72), Distance.INCH);
		ratio = conversion.getMillimeter();
		invRatio = new Vector2f(1.0f / ratio.x(), 1.0f / ratio.y());
		windowsSize = new Dimension(new Vector2f(200, 200), Distance.PIXEL);
	}
	
	/**
	 * get the Windows diagonal size in the request unit
	 * @param type Unit type requested.
	 * @return the requested size
	 */
	public static float getWindowsDiag(final Distance type) {
		final Vector2f size = getWindowsSize(type);
		return size.length();
	}
	
	/**
	 * get the Windows size in the request unit
	 * @param type Unit type requested.
	 * @return the requested size
	 */
	public static Vector2f getWindowsSize(final Distance type) {
		return windowsSize.get(type);
	}
	
	/**
	 * set the Milimeter ratio for calculation
	 * @param ratio Milimeter ration for the screen calculation interpolation
	 * @param type Unit type requested.
	 * @note: same as @ref setPixelPerInch (internal manage convertion)
	 */
	public static void setPixelRatio(final Vector2f ratio, final Distance type) {
		Log.info("Set a new screen ratio for the screen : ratio=" + ratio + " type=" + type);
		final Dimension conversion = new Dimension(ratio, type);
		Log.info("     == > " + conversion);
		Dimension.ratio = conversion.getMillimeter();
		invRatio = new Vector2f(1.0f / Dimension.ratio.x(), 1.0f / Dimension.ratio.y());
		Log.info("Set a new screen ratio for the screen : ratioMm=" + Dimension.ratio);
	}
	
	/**
	 * set the current Windows size
	 * @param size size of the current windows in pixel.
	 */
	public static void setPixelWindowsSize(final Vector2f size) {
		windowsSize = new Dimension(size);
		Log.verbose("Set a new Windows property size " + windowsSize + "px");
	}
	
	/**
	 * Constructor (default :0,0 mode pixel)
	 */
	public Dimension() {
		this(Vector2f.ZERO, Distance.PIXEL);
	}
	
	/**
	 * Constructor
	 * @param size Requested dimension
	 */
	public Dimension(final Vector2f size) {
		this(size, Distance.PIXEL);
	}
	
	public Dimension(final Vector2f size, final Distance type) {
		this.size = size;
		this.type = type;
	}
	
	/**
	 * get the current dimension in requested type
	 * @param type Type of unit requested.
	 * @return dimension requested.
	 */
	public Vector2f get(final Distance type) {
		return switch (type) {
			case POURCENT -> getPourcent();
			case PIXEL -> getPixel();
			case METER -> getMeter();
			case CENTIMETER -> getCentimeter();
			case MILLIMETER -> getMillimeter();
			case KILOMETER -> getKilometer();
			case INCH -> getInch();
			case FOOT -> getFoot();
		};
	}
	
	/**
	 * get the current dimension in Centimeter
	 * @return dimension in Centimeter
	 */
	public Vector2f getCentimeter() {
		return getMillimeter().multiply(MILLIMETER_TO_CENTIMETER);
	}
	
	/**
	 * get the current dimension in Foot
	 * @return dimension in Foot
	 */
	public Vector2f getFoot() {
		return getMillimeter().multiply(MILLIMETER_TO_FOOT);
	}
	
	/**
	 * get the current dimension in Inch
	 * @return dimension in Inch
	 */
	public Vector2f getInch() {
		return getMillimeter().multiply(MILLIMETER_TO_INCH);
	}
	
	/**
	 * get the current dimension in Kilometer
	 * @return dimension in Kilometer
	 */
	public Vector2f getKilometer() {
		return getMillimeter().multiply(MILLIMETER_TO_KILOMETER);
	}
	
	/**
	 * get the current dimension in Meter
	 * @return dimension in Meter
	 */
	public Vector2f getMeter() {
		return getMillimeter().multiply(MILLIMETER_TO_METER);
	}
	
	/**
	 * get the current dimension in Millimeter
	 * @return dimension in Millimeter
	 */
	public Vector2f getMillimeter() {
		return new Vector2f(getPixel().x() * invRatio.x(), getPixel().y() * invRatio.y());
	}
	
	/**
	 * get the current dimension in pixel
	 * @return dimension in Pixel
	 */
	public Vector2f getPixel() {
		if (this.type != Distance.POURCENT) {
			return this.size;
		}
		final Vector2f windDim = windowsSize.getPixel();
		final Vector2f res = new Vector2f(windDim.x() * this.size.x(), windDim.y() * this.size.y());
		//GALE_DEBUG("Get % : " + m_data + " / " + windDim + " == > " + res);
		return res;
	}
	
	public Vector2i getPixeli() {
		if (this.type != Distance.POURCENT) {
			return new Vector2i((int) this.size.x(), (int) this.size.y());
		}
		final Vector2f windDim = windowsSize.getPixel();
		final Vector2i res = new Vector2i((int) (windDim.x() * this.size.x()), (int) (windDim.y() * this.size.y()));
		//GALE_DEBUG("Get % : " + m_data + " / " + windDim + " == > " + res);
		return res;
	}
	
	/**
	 * get the current dimension in Pourcent
	 * @return dimension in Pourcent
	 */
	public Vector2f getPourcent() {
		if (this.type != Distance.POURCENT) {
			final Vector2f windDim = windowsSize.getPixel();
			//GALE_DEBUG(" windows dimension : " /*+ windowsSize*/ + "  == > " + windDim + "px"); // ==> infinite loop ...
			//printf(" windows dimension : %f,%f", windDim.x(),windDim.y());
			//printf(" data : %f,%f", m_data.x(),m_data.y());
			return new Vector2f((this.size.x() / windDim.x()) * 100.0f, (this.size.y() / windDim.y()) * 100.0f);
		}
		return new Vector2f(this.size.x() * 100.0f, this.size.y() * 100.0f);
	};
	
	/**
	 * get the dimension type
	 * @return the type
	 */
	public Distance getType() {
		return this.type;
	}
	
	/**
	 * set the current dimension in requested type
	 * @param config dimension configuration.
	 */
	public static Dimension valueOf(String config) {
		final Vector2f size = Vector2f.ZERO;
		Distance type = Distance.PIXEL;
		if (config.endsWith("%")) {
			type = Distance.POURCENT;
			config = config.substring(0, config.length() - 1);
		} else if (config.endsWith("px")) {
			type = Distance.PIXEL;
			config = config.substring(0, config.length() - 2);
		} else if (config.endsWith("ft")) {
			type = Distance.FOOT;
			config = config.substring(0, config.length() - 2);
		} else if (config.endsWith("in")) {
			type = Distance.INCH;
			config = config.substring(0, config.length() - 2);
		} else if (config.endsWith("km")) {
			type = Distance.KILOMETER;
			config = config.substring(0, config.length() - 2);
		} else if (config.endsWith("mm")) {
			type = Distance.MILLIMETER;
			config = config.substring(0, config.length() - 2);
		} else if (config.endsWith("cm")) {
			type = Distance.CENTIMETER;
			config = config.substring(0, config.length() - 2);
		} else if (config.endsWith("m")) {
			type = Distance.METER;
			config = config.substring(0, config.length() - 1);
		} else {
			Log.critical("Can not parse dimension : '" + config + "'");
			return null;
		}
		final Vector2f tmp = Vector2f.valueOf(config);
		final Dimension ret = new Dimension(tmp, type);
		Log.verbose(" config dimension : '" + config + "'  == > " + ret.toString());
		return ret;
	}
	
	/**
	 * string cast :
	 */
	@Override
	public String toString() {
		String str = get(getType()).toString();
		switch (getType()) {
			case POURCENT -> str += "%";
			case PIXEL -> str += "px";
			case METER -> str += "m";
			case CENTIMETER -> str += "cm";
			case MILLIMETER -> str += "mm";
			case KILOMETER -> str += "km";
			case INCH -> str += "in";
			case FOOT -> str += "ft";
			default -> str += "";
		}
		return str;
	}
	
}
