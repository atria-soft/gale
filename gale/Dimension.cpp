/** @file
 * @author Edouard DUPIN
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license MPL v2.0 (see license file)
 */

#include <gale/Dimension.hpp>
#include <gale/debug.hpp>

// TODO : set this in a super class acced in a statin fuction...
// ratio in milimeter :
static bool isInit = false;
static vec2 ratio(9999999,888888);
static vec2 invRatio(1,1);
static gale::Dimension windowsSize(vec2(9999999,888888), gale::distance::pixel);

static const float       inchToMillimeter = 1.0f/25.4f;
static const float       footToMillimeter = 1.0f/304.8f;
static const float      meterToMillimeter = 1.0f/1000.0f;
static const float centimeterToMillimeter = 1.0f/10.0f;
static const float  kilometerToMillimeter = 1.0f/1000000.0f;
static const float millimeterToInch = 25.4f;
static const float millimeterToFoot = 304.8f;
static const float millimeterToMeter =1000.0f;
static const float millimeterToCentimeter = 10.0f;
static const float millimeterToKilometer = 1000000.0f;


void gale::Dimension::init() {
	if (true == isInit) {
		return;
	}
	gale::Dimension conversion(vec2(72,72), gale::distance::inch);
	ratio = conversion.getMillimeter();
	invRatio.setValue(1.0f/ratio.x(),1.0f/ratio.y());
	windowsSize.set(vec2(200,200), gale::distance::pixel);
	isInit = true;
}

void gale::Dimension::unInit() {
	isInit = false;
	ratio.setValue(9999999,888888);
	invRatio.setValue(1.0f/ratio.x(),1.0f/ratio.y());
	windowsSize.set(vec2(9999999,88888), gale::distance::pixel);
}

void gale::Dimension::setPixelRatio(const vec2& _ratio, enum gale::distance _type) {
	gale::Dimension::init();
	GALE_INFO("Set a new screen ratio for the screen : ratio=" << _ratio << " type=" << _type);
	gale::Dimension conversion(_ratio, _type);
	GALE_INFO("     == > " << conversion);
	ratio = conversion.getMillimeter();
	invRatio.setValue(1.0f/ratio.x(),1.0f/ratio.y());
	GALE_INFO("Set a new screen ratio for the screen : ratioMm=" << ratio);
}

void gale::Dimension::setPixelWindowsSize(const vec2& _size) {
	windowsSize = _size;
	GALE_VERBOSE("Set a new Windows property size " << windowsSize << "px");
}

vec2 gale::Dimension::getWindowsSize(enum gale::distance _type) {
	return windowsSize.get(_type);
}

float gale::Dimension::getWindowsDiag(enum gale::distance _type) {
	vec2 size = gale::Dimension::getWindowsSize(_type);
	return size.length();
}

gale::Dimension::Dimension() :
  m_data(0,0),
  m_type(gale::distance::pixel) {
	// notinh to do ...
}

gale::Dimension::Dimension(const vec2& _size, enum gale::distance _type) :
  m_data(0,0),
  m_type(gale::distance::pixel) {
	set(_size, _type);
}

void gale::Dimension::set(etk::String _config) {
	m_data.setValue(0,0);
	m_type = gale::distance::pixel;
	enum distance type = gale::distance::pixel;
	if (etk::end_with(_config, "%", false) == true) {
		type = gale::distance::pourcent;
		_config.erase(_config.size()-1, 1);
	} else if (etk::end_with(_config, "px",false) == true) {
		type = gale::distance::pixel;
		_config.erase(_config.size()-2, 2);
	} else if (etk::end_with(_config, "ft",false) == true) {
		type = gale::distance::foot;
		_config.erase(_config.size()-2, 2);
	} else if (etk::end_with(_config, "in",false) == true) {
		type = gale::distance::inch;
		_config.erase(_config.size()-2, 2);
	} else if (etk::end_with(_config, "km",false) == true) {
		type = gale::distance::kilometer;
		_config.erase(_config.size()-2, 2);
	} else if (etk::end_with(_config, "mm",false) == true) {
		type = gale::distance::millimeter;
		_config.erase(_config.size()-2, 2);
	} else if (etk::end_with(_config, "cm",false) == true) {
		type = gale::distance::centimeter;
		_config.erase(_config.size()-2, 2);
	} else if (etk::end_with(_config, "m",false) == true) {
		type = gale::distance::meter;
		_config.erase(_config.size()-1, 1);
	} else {
		GALE_CRITICAL("Can not parse dimention : \"" << _config << "\"");
		return;
	}
	vec2 tmp = _config;
	set(tmp, type);
	GALE_VERBOSE(" config dimention : \"" << _config << "\"  == > " << *this );
}

gale::Dimension::~Dimension() {
	// nothing to do ...
}

gale::Dimension::operator etk::String() const {
	etk::String str;
	str = get(getType());
	
	switch(getType()) {
		case gale::distance::pourcent:
			str += "%";
			break;
		case gale::distance::pixel:
			str += "px";
			break;
		case gale::distance::meter:
			str += "m";
			break;
		case gale::distance::centimeter:
			str += "cm";
			break;
		case gale::distance::millimeter:
			str += "mm";
			break;
		case gale::distance::kilometer:
			str += "km";
			break;
		case gale::distance::inch:
			str += "in";
			break;
		case gale::distance::foot:
			str += "ft";
			break;
	}
	return str;
}

vec2 gale::Dimension::get(enum gale::distance _type) const {
	switch(_type) {
		case gale::distance::pourcent:
			return getPourcent();
		case gale::distance::pixel:
			return getPixel();
		case gale::distance::meter:
			return getMeter();
		case gale::distance::centimeter:
			return getCentimeter();
		case gale::distance::millimeter:
			return getMillimeter();
		case gale::distance::kilometer:
			return getKilometer();
		case gale::distance::inch:
			return getInch();
		case gale::distance::foot:
			return getFoot();
	}
	return vec2(0,0);
}

void gale::Dimension::set(const vec2& _size, enum gale::distance _type) {
	// set min max on input to limit error : 
	vec2 size(etk::avg(0.0f,_size.x(),9999999.0f),
	          etk::avg(0.0f,_size.y(),9999999.0f));
	switch(_type) {
		case gale::distance::pourcent: {
			vec2 size2(etk::avg(0.0f,_size.x(),100.0f),
			           etk::avg(0.0f,_size.y(),100.0f));
			m_data = vec2(size2.x()*0.01f, size2.y()*0.01f);
			//GALE_VERBOSE("Set % : " << size2 << "  == > " << m_data);
			break;
		}
		case gale::distance::pixel:
			m_data = size;
			break;
		case gale::distance::meter:
			m_data = vec2(size.x()*meterToMillimeter*ratio.x(), size.y()*meterToMillimeter*ratio.y());
			break;
		case gale::distance::centimeter:
			m_data = vec2(size.x()*centimeterToMillimeter*ratio.x(), size.y()*centimeterToMillimeter*ratio.y());
			break;
		case gale::distance::millimeter:
			m_data = vec2(size.x()*ratio.x(), size.y()*ratio.y());
			break;
		case gale::distance::kilometer:
			m_data = vec2(size.x()*kilometerToMillimeter*ratio.x(), size.y()*kilometerToMillimeter*ratio.y());
			break;
		case gale::distance::inch:
			m_data = vec2(size.x()*inchToMillimeter*ratio.x(), size.y()*inchToMillimeter*ratio.y());
			break;
		case gale::distance::foot:
			m_data = vec2(size.x()*footToMillimeter*ratio.x(), size.y()*footToMillimeter*ratio.y());
			break;
	}
	m_type = _type;
}

vec2 gale::Dimension::getPixel() const {
	if (m_type!=gale::distance::pourcent) {
		return m_data;
	} else {
		vec2 windDim = windowsSize.getPixel();
		vec2 res = vec2(windDim.x()*m_data.x(), windDim.y()*m_data.y());
		//GALE_DEBUG("Get % : " << m_data << " / " << windDim << " == > " << res);
		return res;
	}
}

vec2 gale::Dimension::getPourcent() const {
	if (m_type!=gale::distance::pourcent) {
		vec2 windDim = windowsSize.getPixel();
		//GALE_DEBUG(" windows dimention : " /*<< windowsSize*/ << "  == > " << windDim << "px"); // ==> infinite loop ...
		//printf(" windows dimention : %f,%f", windDim.x(),windDim.y());
		//printf(" data : %f,%f", m_data.x(),m_data.y());
		return vec2((m_data.x()/windDim.x())*100.0f, (m_data.y()/windDim.y())*100.0f);
	}
	return vec2(m_data.x()*100.0f, m_data.y()*100.0f);;
}

vec2 gale::Dimension::getMeter() const {
	return gale::Dimension::getMillimeter()*millimeterToMeter;
}

vec2 gale::Dimension::getCentimeter() const {
	return gale::Dimension::getMillimeter()*millimeterToCentimeter;
}

vec2 gale::Dimension::getMillimeter() const {
	return gale::Dimension::getPixel()*invRatio;
}

vec2 gale::Dimension::getKilometer() const {
	return gale::Dimension::getMillimeter()*millimeterToKilometer;
}

vec2 gale::Dimension::getInch() const {
	return gale::Dimension::getMillimeter()*millimeterToInch;
}

vec2 gale::Dimension::getFoot() const {
	return gale::Dimension::getMillimeter()*millimeterToFoot;
}

etk::Stream& gale::operator <<(etk::Stream& _os, enum gale::distance _obj) {
	switch(_obj) {
		case gale::distance::pourcent:
			_os << "%";
			break;
		case gale::distance::pixel:
			_os << "px";
			break;
		case gale::distance::meter:
			_os << "m";
			break;
		case gale::distance::centimeter:
			_os << "cm";
			break;
		case gale::distance::millimeter:
			_os << "mm";
			break;
		case gale::distance::kilometer:
			_os << "km";
			break;
		case gale::distance::inch:
			_os << "in";
			break;
		case gale::distance::foot:
			_os << "ft";
			break;
	}
	return _os;
}

etk::Stream& gale::operator <<(etk::Stream& _os, const gale::Dimension& _obj) {
	_os << _obj.get(_obj.getType()) << _obj.getType();
	return _os;
}

namespace etk {
	template<> etk::String toString<gale::Dimension>(const gale::Dimension& _obj) {
		return _obj;
	}
	template<> etk::UString toUString<gale::Dimension>(const gale::Dimension& _obj) {
		return etk::toUString(etk::toString(_obj));
	}
	template<> bool from_string<gale::Dimension>(gale::Dimension& _variableRet, const etk::String& _value) {
		_variableRet = gale::Dimension(_value);
		return true;
	}
	template<> bool from_string<gale::Dimension>(gale::Dimension& _variableRet, const etk::UString& _value) {
		return from_string(_variableRet, etk::toString(_value));
	}
};

#ifdef GALE_BUILD_ESIGNAL
	#include <esignal/details/Signal.hxx>
	ESIGNAL_DECLARE_SIGNAL(gale::Dimension);
#endif

#ifdef GALE_BUILD_EPROPERTY
	//#include <eproperty/details/Range.hxx>
	#include <eproperty/details/Value.hxx>
	//template class eproperty::Range<gale::Dimension>;
	template class eproperty::Value<gale::Dimension>;
#endif
