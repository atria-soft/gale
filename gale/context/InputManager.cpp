/** @file
 * @author Edouard DUPIN
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */

#include <etk/types.hpp>
#include <gale/debug.hpp>
#include <gale/gale.hpp>
#include <gale/context/Context.hpp>
#include <gale/context/InputManager.hpp>
#include <gale/Application.hpp>



#define EVENT_DEBUG  GALE_VERBOSE
//#define EVENT_DEBUG  GALE_DEBUG

void gale::context::InputManager::calculateLimit() {
	m_eventInputLimit.sepatateTime = 300000; // µs
	m_eventInputLimit.DpiOffset = m_dpi*100;
	m_eventMouseLimit.sepatateTime = 300000; // µs
	m_eventMouseLimit.DpiOffset = (float)m_dpi*(float)0.1;
}

void gale::context::InputManager::setDpi(int32_t newDPI) {
	m_dpi = newDPI;
	// recalculate the DPI system ...
	calculateLimit();
}

bool gale::context::InputManager::localEventInput(enum gale::key::type _type,
                                                  ememory::SharedPtr<gale::Application> _destApplication,
                                                  int32_t _IdInput,
                                                  enum gale::key::status _status,
                                                  vec2 _pos) {
	if (nullptr != _destApplication) {
		if (_type == gale::key::typeMouse || _type == gale::key::typeFinger) {
			// create the system Event :
			gale::event::InputSystem tmpEventSystem(_type, _status, _IdInput, _pos, _destApplication, 0, m_specialKey); // TODO : set the real ID ...
			// generate the event :
			return _destApplication->systemEventInput(tmpEventSystem);
		} else {
			return false;
		}
	}
	return false;
}

void gale::context::InputManager::abortElement(InputPoperty *_eventTable,
                                               int32_t _idInput,
                                               enum gale::key::type _type) {
	if (nullptr == _eventTable) {
		return;
	}
	if (_eventTable[_idInput].isUsed == true) {
		localEventInput(_type, 
		                _eventTable[_idInput].curentApplicationEvent.lock(),
		                _eventTable[_idInput].destinationInputId,
		                gale::key::statusAbort,
		                _eventTable[_idInput].posEvent);
	}
}

void gale::context::InputManager::cleanElement(InputPoperty *_eventTable,
                                               int32_t _idInput) {
	if (nullptr == _eventTable) {
		return;
	}
	//GALE_INFO("CleanElement[" << idInput << "] = @" << (int64_t)eventTable);
	_eventTable[_idInput].isUsed = false;
	_eventTable[_idInput].destinationInputId = 0;
	_eventTable[_idInput].lastTimeEvent = 0;
	_eventTable[_idInput].curentApplicationEvent.reset();
	_eventTable[_idInput].origin.setValue(0,0);
	_eventTable[_idInput].size.setValue(99999999,99999999);
	_eventTable[_idInput].downStart.setValue(0,0);
	_eventTable[_idInput].isDown = false;
	_eventTable[_idInput].isInside = false;
	_eventTable[_idInput].nbClickEvent = 0;
	_eventTable[_idInput].posEvent.setValue(0,0);
}

gale::context::InputManager::InputManager(gale::Context& _context) :
  m_grabApplication(),
  m_context(_context) {
	setDpi(200);
	GALE_INFO("Init (start)");
	for(int32_t iii=0; iii<MAX_MANAGE_INPUT; iii++) {
		// remove the property of this input ...
		cleanElement(m_eventInputSaved, iii);
		cleanElement(m_eventMouseSaved, iii);
	}
	GALE_INFO("Init (end)");
}

gale::context::InputManager::~InputManager() {
	GALE_INFO("Un-Init (start)");
	GALE_INFO("Un-Init (end)");
}

int32_t gale::context::InputManager::localGetDestinationId(enum gale::key::type _type,
                                                           ememory::SharedPtr<gale::Application> _destApplication,
                                                           int32_t _realInputId) {
	if (_type == gale::key::typeFinger) {
		int32_t lastMinimum = 0;
		for(int32_t iii=0; iii<MAX_MANAGE_INPUT; iii++) {
			if (true == m_eventInputSaved[iii].isUsed) {
				ememory::SharedPtr<gale::Application> tmpApplication = m_eventInputSaved[iii].curentApplicationEvent.lock();
				if (tmpApplication == _destApplication) {
					if (iii != _realInputId) {
						lastMinimum = std::max(lastMinimum, m_eventInputSaved[iii].destinationInputId);
					}
				}
			}
		}
		return lastMinimum+1;
	}
	return _realInputId;
}

// note if id<0  == > the it was finger event ...
void gale::context::InputManager::motion(enum gale::key::type _type,
                                         int _pointerID,
                                         vec2 _pos) {
	EVENT_DEBUG("motion event : " << _type << " " << _pointerID << " " << _pos);
	if (MAX_MANAGE_INPUT <= _pointerID) {
		// reject pointer  == > out of IDs...
		return;
	}
	InputPoperty *eventTable = nullptr;
	if (_type == gale::key::typeMouse) {
		eventTable = m_eventMouseSaved;
	} else if (_type == gale::key::typeFinger) {
		eventTable = m_eventInputSaved;
	} else {
		GALE_ERROR("Unknown type of event");
		return;
	}
	if(    _pointerID > MAX_MANAGE_INPUT
	    || _pointerID < 0) {
		// not manage input
		return;
	}
	ememory::SharedPtr<gale::Application::Windows> tmpWindows = m_context.getWindows();
	// special case for the mouse event 0 that represent the hover event of the system :
	if (_type == gale::key::typeMouse && _pointerID == 0) {
		// this event is all time on the good Application ... and manage the enter and leave ...
		// NOTE : the "layer Application" force us to get the Application at the specific position all the time :
		ememory::SharedPtr<gale::Application> tmpApplication;
		if (m_grabApplication.lock() != nullptr) {
			// grab all events ...
			tmpApplication = m_grabApplication.lock();
		} else {
			if (nullptr != tmpWindows) {
				tmpApplication = tmpWindows->getApplicationAtPos(_pos);
			}
		}
		if(    tmpApplication != eventTable[_pointerID].curentApplicationEvent.lock()
		    || (    true == eventTable[_pointerID].isInside
		         && (     eventTable[_pointerID].origin.x() > _pos.x()
		              ||  eventTable[_pointerID].origin.y() > _pos.y()
		              || (eventTable[_pointerID].origin.x() + eventTable[_pointerID].size.x()) < _pos.x()
		              || (eventTable[_pointerID].origin.y() + eventTable[_pointerID].size.y()) < _pos.y()) ) ) {
			eventTable[_pointerID].isInside = false;
			EVENT_DEBUG("GUI : Input ID=" << _pointerID << " == >" << eventTable[_pointerID].destinationInputId << " [LEAVE] " << _pos);
			eventTable[_pointerID].posEvent = _pos;
			localEventInput(_type,
			                eventTable[_pointerID].curentApplicationEvent.lock(),
			                eventTable[_pointerID].destinationInputId,
			                gale::key::statusLeave,
			                _pos);
		}
		if (eventTable[_pointerID].isInside == false) {
			// set the element inside ...
			eventTable[_pointerID].isInside = true;
			// get destination Application :
			eventTable[_pointerID].curentApplicationEvent = tmpApplication;
			if (tmpApplication == nullptr) {
				eventTable[_pointerID].isInside = false;
			} else {
				eventTable[_pointerID].origin = tmpApplication->getOrigin();
				eventTable[_pointerID].size = tmpApplication->getSize();
			}
			eventTable[_pointerID].destinationInputId = 0;
			EVENT_DEBUG("GUI : Input ID=" << _pointerID
			            << " == >" << eventTable[_pointerID].destinationInputId
			            << " [ENTER] " << _pos);
			eventTable[_pointerID].posEvent = _pos;
			localEventInput(_type,
			                tmpApplication,
			                eventTable[_pointerID].destinationInputId,
			                gale::key::statusEnter,
			                _pos);
		}
		EVENT_DEBUG("GUI : Input ID=" << _pointerID
		            << " == >" << eventTable[_pointerID].destinationInputId
		            << " [MOVE]  " << _pos);
		eventTable[_pointerID].posEvent = _pos;
		localEventInput(_type,
		                tmpApplication,
		                eventTable[_pointerID].destinationInputId,
		                gale::key::statusMove,
		                _pos);
	} else if (true == eventTable[_pointerID].isUsed) {
		if (true == eventTable[_pointerID].isInside) {
			if(     eventTable[_pointerID].origin.x() > _pos.x()
			    ||  eventTable[_pointerID].origin.y() > _pos.y()
			    || (eventTable[_pointerID].origin.x() + eventTable[_pointerID].size.x()) < _pos.x()
			    || (eventTable[_pointerID].origin.y() + eventTable[_pointerID].size.y()) < _pos.y()) {
				eventTable[_pointerID].isInside = false;
				EVENT_DEBUG("GUI : Input ID=" << _pointerID
				            << " == >" << eventTable[_pointerID].destinationInputId
				            << " [LEAVE] " << _pos);
				eventTable[_pointerID].posEvent = _pos;
				localEventInput(_type,
				                eventTable[_pointerID].curentApplicationEvent.lock(),
				                eventTable[_pointerID].destinationInputId,
				                gale::key::statusLeave,
				                _pos);
			}
		} else {
			if(    (     eventTable[_pointerID].origin.x() <= _pos.x()
			         && (eventTable[_pointerID].origin.x() + eventTable[_pointerID].size.x()) >= _pos.x() )
			    && (     eventTable[_pointerID].origin.y() <= _pos.y()
			         && (eventTable[_pointerID].origin.y() + eventTable[_pointerID].size.y()) >= _pos.y() ) ) {
				eventTable[_pointerID].isInside = true;
				EVENT_DEBUG("GUI : Input ID=" << _pointerID
				            << " == >" << eventTable[_pointerID].destinationInputId
				            << " [ENTER] " << _pos);
				eventTable[_pointerID].posEvent = _pos;
				localEventInput(_type,
				                eventTable[_pointerID].curentApplicationEvent.lock(),
				                eventTable[_pointerID].destinationInputId,
				                gale::key::statusEnter,
				                _pos);
			}
		}
		EVENT_DEBUG("GUI : Input ID=" << _pointerID
		            << " == >" << eventTable[_pointerID].destinationInputId
		            << " [MOVE]  " << _pos);
		eventTable[_pointerID].posEvent = _pos;
		localEventInput(_type,
		                eventTable[_pointerID].curentApplicationEvent.lock(),
		                eventTable[_pointerID].destinationInputId,
		                gale::key::statusMove,
		                _pos);
	}
}

void gale::context::InputManager::state(enum gale::key::type _type,
                                        int _pointerID,
                                        bool _isDown,
                                        vec2 _pos)
{
	if (MAX_MANAGE_INPUT <= _pointerID) {
		// reject pointer  == > out of IDs...
		return;
	}
	EVENT_DEBUG("event pointerId=" << _pointerID);
	// convert position in open-GL coordonates ...
	InputPoperty *eventTable = nullptr;
	InputLimit   localLimit;
	if (_type == gale::key::typeMouse) {
		eventTable = m_eventMouseSaved;
		localLimit = m_eventMouseLimit;
	} else if (_type == gale::key::typeFinger) {
		eventTable = m_eventInputSaved;
		localLimit = m_eventInputLimit;
	} else {
		GALE_ERROR("Unknown type of event");
		return;
	}
	if(    _pointerID > MAX_MANAGE_INPUT
	    || _pointerID <= 0) {
		// not manage input
		return;
	}
	// get the curent time ...
	int64_t currentTime = gale::getTime();
	ememory::SharedPtr<gale::Application::Windows> tmpWindows = m_context.getWindows();
	
	if (true == _isDown) {
		EVENT_DEBUG("GUI : Input ID=" << _pointerID
		             << " == >" << eventTable[_pointerID].destinationInputId
		             << " [DOWN] " << _pos);
		if(true == eventTable[_pointerID].isUsed) {
			// we have an event previously ... check delay between click and offset position
			if (currentTime - eventTable[_pointerID].lastTimeEvent > localLimit.sepatateTime) {
				cleanElement(eventTable, _pointerID);
			} else if(    abs(eventTable[_pointerID].downStart.x() - _pos.x()) >= localLimit.DpiOffset
			           || abs(eventTable[_pointerID].downStart.y() - _pos.y()) >= localLimit.DpiOffset ){
				cleanElement(eventTable, _pointerID);
			}
		}
		if(true == eventTable[_pointerID].isUsed) {
			// save start time
			eventTable[_pointerID].lastTimeEvent = currentTime;
			// generate DOWN Event
			EVENT_DEBUG("GUI : Input ID=" << _pointerID
			            << " == >" << eventTable[_pointerID].destinationInputId
			            << " [DOWN]   " << _pos);
			eventTable[_pointerID].posEvent = _pos;
			localEventInput(_type,
			                eventTable[_pointerID].curentApplicationEvent.lock(),
			                eventTable[_pointerID].destinationInputId,
			                gale::key::statusDown,
			                _pos);
		} else {
			// Mark it used :
			eventTable[_pointerID].isUsed = true;
			// Save current position :
			eventTable[_pointerID].downStart = _pos;
			// save start time
			eventTable[_pointerID].lastTimeEvent = currentTime;
			// set the element inside ...
			eventTable[_pointerID].isInside = true;
			ememory::SharedPtr<gale::Application> tmpApplication = m_grabApplication.lock();
			// get destination Application :
			if(nullptr != tmpWindows) {
				if (    tmpApplication != nullptr
				     && _type == gale::key::typeMouse) {
					eventTable[_pointerID].curentApplicationEvent = tmpApplication;
				} else {
					eventTable[_pointerID].curentApplicationEvent = tmpWindows->getApplicationAtPos(_pos);
				}
			} else {
				eventTable[_pointerID].curentApplicationEvent.reset();
			}
			tmpApplication = eventTable[_pointerID].curentApplicationEvent.lock();
			if (tmpApplication != nullptr) {
				eventTable[_pointerID].origin = tmpApplication->getOrigin();
				eventTable[_pointerID].size = tmpApplication->getSize();
				eventTable[_pointerID].destinationInputId = localGetDestinationId(_type, tmpApplication, _pointerID);
			} else {
				eventTable[_pointerID].destinationInputId = -1;
			}
			// generate DOWN Event
			EVENT_DEBUG("GUI : Input ID=" << _pointerID
			            << " == >" << eventTable[_pointerID].destinationInputId
			            << " [DOWN]   " << _pos);
			eventTable[_pointerID].posEvent = _pos;
			localEventInput(_type,
			                tmpApplication,
			                eventTable[_pointerID].destinationInputId,
			                gale::key::statusDown,
			                _pos);
		}
	} else {
		EVENT_DEBUG("GUI : Input ID=" << _pointerID
		             << " == >" << eventTable[_pointerID].destinationInputId
		             << " [UP]     " << _pos);
		if(false == eventTable[_pointerID].isUsed) {
			// bad case ... ???
			GALE_DEBUG("Up event without previous down ... ");
			// Mark it un-used :
			eventTable[_pointerID].isUsed = false;
			// revove the Application ...
			eventTable[_pointerID].curentApplicationEvent.reset();
		} else {
			ememory::SharedPtr<gale::Application> tmpApplication = eventTable[_pointerID].curentApplicationEvent.lock();
			// generate UP Event
			EVENT_DEBUG("GUI : Input ID=" << _pointerID
			            << " == >" << eventTable[_pointerID].destinationInputId
			            << " [UP]     " << _pos);
			eventTable[_pointerID].posEvent = _pos;
			// send up event after the single event to prevent multiple Application getting elements
			localEventInput(_type,
			                tmpApplication,
			                _pointerID,
			                gale::key::statusUp,
			                _pos);
			// generate event (single)
			if(    abs(eventTable[_pointerID].downStart.x() - _pos.x()) < localLimit.DpiOffset
			    && abs(eventTable[_pointerID].downStart.y() - _pos.y()) < localLimit.DpiOffset ){
				// Save current position :
				eventTable[_pointerID].downStart = _pos;
				// save start time
				eventTable[_pointerID].lastTimeEvent = currentTime;
				int32_t nbClickMax = 0;
				if(tmpApplication != nullptr) {
					nbClickMax = tmpApplication->getMouseLimit();
					if (nbClickMax>5) {
						nbClickMax = 5;
					}
				}
				// in grab mode the single to quinte event are not generated ....
				if(    (    m_grabApplication.lock() == nullptr
				         || _type != gale::key::typeMouse )
				    && eventTable[_pointerID].nbClickEvent < nbClickMax) {
					// generate event SINGLE :
					eventTable[_pointerID].nbClickEvent++;
					EVENT_DEBUG("GUI : Input ID=" << _pointerID
					            << " == >" << eventTable[_pointerID].destinationInputId
					            << " [" << eventTable[_pointerID].nbClickEvent << "] " << _pos);
					eventTable[_pointerID].posEvent = _pos;
					localEventInput(_type,
					                tmpApplication,
					                eventTable[_pointerID].destinationInputId,
					                (enum gale::key::status)(gale::key::statusSingle + eventTable[_pointerID].nbClickEvent-1),
					                _pos);
					if( eventTable[_pointerID].nbClickEvent >= nbClickMax) {
						eventTable[_pointerID].nbClickEvent = 0;
					}
				} else {
					eventTable[_pointerID].nbClickEvent = 0;
				}
			}
			// send up event after the single event to prevent multiple Application getting elements
			localEventInput(_type,
			                tmpApplication,
			                _pointerID,
			                gale::key::statusUpAfter,
			                _pos);
			// specific for tuch event
			if (_type == gale::key::typeFinger) {
				cleanElement(eventTable, _pointerID);
			}
		}
	}
}


