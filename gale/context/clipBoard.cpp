/** @file
 * @author Edouard DUPIN
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license MPL v2.0 (see license file)
 */

#include <etk/types.hpp>

#include <gale/debug.hpp>
#include <gale/context/clipBoard.hpp>
#include <gale/context/Context.hpp>
#include <etk/typeInfo.hpp>
ETK_DECLARE_TYPE(gale::context::clipBoard::clipboardListe);

/*
note: copy id data :
        0 : copy standard
   [1..9] : copy internal
       10 : middle button
*/
//!< Local copy of the clipboards
static etk::String myCopy[gale::context::clipBoard::clipboardCount];

static const char* clipboardDescriptionString[gale::context::clipBoard::clipboardCount+1] = {
	"clipboard0",
	"clipboard1",
	"clipboard2",
	"clipboard3",
	"clipboard4",
	"clipboard5",
	"clipboard6",
	"clipboard7",
	"clipboard8",
	"clipboard9",
	"clipboardStd",
	"clipboardSelection",
	"clipboardCount"
};

etk::Stream& gale::operator <<(etk::Stream& _os, enum gale::context::clipBoard::clipboardListe _obj) {
	if (_obj >= 0 && _obj <gale::context::clipBoard::clipboardCount) {
		_os << clipboardDescriptionString[int32_t(_obj)];
	} else {
		_os << "[ERROR]";
	}
	return _os;
}

namespace etk {
	template<> etk::String toString<enum gale::context::clipBoard::clipboardListe>(const enum gale::context::clipBoard::clipboardListe& _obj) {
		return clipboardDescriptionString[int32_t(_obj)];
	}
	
	template <> bool from_string<enum gale::context::clipBoard::clipboardListe>(enum gale::context::clipBoard::clipboardListe& _variableRet, const etk::String& _value) {
		for (size_t iii=0; iii< sizeof(clipboardDescriptionString); ++iii) {
			if (_value == clipboardDescriptionString[iii]) {
				_variableRet = (enum gale::context::clipBoard::clipboardListe)iii;
				return true;
			}
		}
		return false;
	}
}

void gale::context::clipBoard::init() {
	GALE_INFO("Initialyse ClipBoards");
	for(int32_t iii=0; iii<gale::context::clipBoard::clipboardCount; ++iii) {
		myCopy[iii].clear();
	}
}


void gale::context::clipBoard::unInit() {
	GALE_INFO("Initialyse ClipBoards");
	for(int32_t i=0; i<gale::context::clipBoard::clipboardCount; i++) {
		myCopy[i].clear();
	}
}


void gale::context::clipBoard::set(enum gale::context::clipBoard::clipboardListe _clipboardID, const etk::String& _data) {
	// check if ID is correct
	if(0 == _data.size()) {
		GALE_INFO("request a copy of nothing");
		return;
	} else 
	
	if(_clipboardID >= gale::context::clipBoard::clipboardCount) {
		GALE_WARNING("request ClickBoard id error");
		return;
	}
	
	gale::context::clipBoard::setSystem(_clipboardID, _data);
	
	if(    gale::context::clipBoard::clipboardStd == _clipboardID
	    || gale::context::clipBoard::clipboardSelection == _clipboardID) {
		gale::getContext().clipBoardSet(_clipboardID);
	}
}


void gale::context::clipBoard::request(enum gale::context::clipBoard::clipboardListe _clipboardID) {
	if(_clipboardID >= gale::context::clipBoard::clipboardCount) {
		GALE_WARNING("request ClickBoard id error");
		return;
	}
	
	if(    gale::context::clipBoard::clipboardStd == _clipboardID
	    || gale::context::clipBoard::clipboardSelection == _clipboardID) {
		gale::getContext().clipBoardGet(_clipboardID);
	} else {
		// generate an event on the main thread ...
		gale::getContext().OS_ClipBoardArrive(_clipboardID);
	}
}


void gale::context::clipBoard::setSystem(enum gale::context::clipBoard::clipboardListe _clipboardID, const etk::String& _data) {
	if(_clipboardID >= gale::context::clipBoard::clipboardCount) {
		GALE_WARNING("request ClickBoard id error");
		return;
	}
	// Copy datas ...
	myCopy[_clipboardID] = _data;
}


const etk::String& gale::context::clipBoard::get(enum gale::context::clipBoard::clipboardListe _clipboardID) {
	static const etk::String emptyString("");
	if(_clipboardID >= gale::context::clipBoard::clipboardCount) {
		GALE_WARNING("request ClickBoard id error");
		return emptyString;
	}
	// Copy datas ...
	return myCopy[_clipboardID];
}





