/**
 * @author Edouard DUPIN, Kevin BILLONNEAU
 *
 * @copyright 2011, Edouard DUPIN, all right reserved
 *
 * @license APACHE v2.0 (see license file)
 */

package org.gale;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.view.MotionEvent;
import android.view.KeyEvent;
import android.util.Log;

import org.gale.Gale;

/**
 * @brief Class : 
 *
 */
public class GaleSurfaceViewGL extends GLSurfaceView implements GaleConstants {
	public static final int SDK_VERSION = android.os.Build.VERSION.SDK_INT;
	private GaleRendererGL m_galeDrawer = null;
	private boolean inputDown1 = false;
	private boolean inputDown2 = false;
	private boolean inputDown3 = false;
	private Gale m_galeNative;
		
	public GaleSurfaceViewGL(Context _context, Gale _galeInstance) {
		// super must be first statement in constructor
		super(_context);
		m_galeNative = _galeInstance;
		/*
		  List of the Android API :
		  Android 4.1, 4.1.1          16  JELLY_BEAN               Platform Highlights
		  Android 4.0.3, 4.0.4        15  ICE_CREAM_SANDWICH_MR1   Platform Highlights
		  Android 4.0, 4.0.1, 4.0.2   14  ICE_CREAM_SANDWICH
		  Android 3.2                 13  HONEYCOMB_MR2
		  Android 3.1.x               12  HONEYCOMB_MR1            Platform Highlights
		  Android 3.0.x               11  HONEYCOMB                Platform Highlights
		  Android 2.3.4
		  Android 2.3.3               10  GINGERBREAD_MR1          Platform Highlights
		  Android 2.3.2
		  Android 2.3.1
		  Android 2.3                  9  GINGERBREAD
		  Android 2.2.x                8  FROYO                    Platform Highlights
		  Android 2.1.x                7  ECLAIR_MR1               Platform Highlights
		  Android 2.0.1                6  ECLAIR_0_1
		  Android 2.0                  5  ECLAIR
		  Android 1.6                  4  DONUT                    Platform Highlights
		  Android 1.5                  3  CUPCAKE                  Platform Highlights
		  Android 1.1                  2  BASE_1_1
		  Android 1.0                  1  BASE
		*/
		// Create an OpenGL ES 2.0 context
		setEGLContextClientVersion(2);
		
		// je n'ai pas compris ...
		m_galeDrawer = new GaleRendererGL(m_galeNative);
		setRenderer(m_galeDrawer);
		
		// Can get the focus ==> get keyboard from JAVA : 
		setFocusable(true);
		setFocusableInTouchMode(true);
	}
	
	public boolean onTouchEvent(final MotionEvent _event) {
		// Wrapper on input events : 
		
		int tmpActionType = _event.getAction();
		
		if (tmpActionType == MotionEvent.ACTION_MOVE) {
			final int pointerCount = _event.getPointerCount();
			for (int p = 0; p < pointerCount; p++) {
				if (SDK_VERSION>=14) {
					final int typeOfPointer = _event.getToolType(p);
					if(    typeOfPointer == MotionEvent.TOOL_TYPE_FINGER
					    || typeOfPointer == MotionEvent.TOOL_TYPE_STYLUS) {
						m_galeNative.inputEventMotion(_event.getPointerId(p), (float)_event.getX(p), (float)_event.getY(p));
					} else if(typeOfPointer == MotionEvent.TOOL_TYPE_MOUSE) {
						m_galeNative.mouseEventMotion(_event.getPointerId(p), (float)_event.getX(p), (float)_event.getY(p));
					}
				} else {
					m_galeNative.inputEventMotion(_event.getPointerId(p), (float)_event.getX(p), (float)_event.getY(p));
				}
			}
		} else if (    tmpActionType == MotionEvent.ACTION_POINTER_1_DOWN
		            || tmpActionType == MotionEvent.ACTION_DOWN) {
			if (SDK_VERSION>=14) {
				final int typeOfPointer = _event.getToolType(0);
				if (    typeOfPointer == MotionEvent.TOOL_TYPE_FINGER
				     || typeOfPointer == MotionEvent.TOOL_TYPE_STYLUS) {
					m_galeNative.inputEventState(_event.getPointerId(0), true, (float)_event.getX(0), (float)_event.getY(0));
				} else if(typeOfPointer == MotionEvent.TOOL_TYPE_MOUSE) {
					m_galeNative.mouseEventState(_event.getPointerId(0), true, (float)_event.getX(0), (float)_event.getY(0));
				}
			} else {
				m_galeNative.inputEventState(_event.getPointerId(0), true, (float)_event.getX(0), (float)_event.getY(0));
			}
			inputDown1 = true;
		} else if (tmpActionType == MotionEvent.ACTION_POINTER_1_UP) {
			if (SDK_VERSION >= 14) {
				final int typeOfPointer = _event.getToolType(0);
				if (    typeOfPointer == MotionEvent.TOOL_TYPE_FINGER
				     || typeOfPointer == MotionEvent.TOOL_TYPE_STYLUS) {
					m_galeNative.inputEventState(_event.getPointerId(0), false, (float)_event.getX(0), (float)_event.getY(0));
				} else if (typeOfPointer == MotionEvent.TOOL_TYPE_MOUSE) {
					m_galeNative.mouseEventState(_event.getPointerId(0), false, (float)_event.getX(0), (float)_event.getY(0));
				}
			} else {
				m_galeNative.inputEventState(_event.getPointerId(0), false, (float)_event.getX(0), (float)_event.getY(0));
			}
			inputDown1 = false;
		} else if (tmpActionType == MotionEvent.ACTION_POINTER_2_DOWN) {
			if (SDK_VERSION>=14) {
				final int typeOfPointer = _event.getToolType(1);
				if (    typeOfPointer == MotionEvent.TOOL_TYPE_FINGER
				     || typeOfPointer == MotionEvent.TOOL_TYPE_STYLUS) {
					m_galeNative.inputEventState(_event.getPointerId(1), true, (float)_event.getX(1), (float)_event.getY(1));
				} else if(typeOfPointer == MotionEvent.TOOL_TYPE_MOUSE) {
					m_galeNative.mouseEventState(_event.getPointerId(1), true, (float)_event.getX(1), (float)_event.getY(1));
				}
			} else {
				m_galeNative.inputEventState(_event.getPointerId(1), true, (float)_event.getX(1), (float)_event.getY(1));
			}
			inputDown2 = true;
		} else if (tmpActionType == MotionEvent.ACTION_POINTER_2_UP) {
			if (SDK_VERSION>=14) {
				final int typeOfPointer = _event.getToolType(1);
				if (    typeOfPointer == MotionEvent.TOOL_TYPE_FINGER
				     || typeOfPointer == MotionEvent.TOOL_TYPE_STYLUS) {
					m_galeNative.inputEventState(_event.getPointerId(1), false, (float)_event.getX(1), (float)_event.getY(1));
				} else if(typeOfPointer == MotionEvent.TOOL_TYPE_MOUSE) {
					m_galeNative.mouseEventState(_event.getPointerId(1), false, (float)_event.getX(1), (float)_event.getY(1));
				}
			} else {
				m_galeNative.inputEventState(_event.getPointerId(1), false, (float)_event.getX(1), (float)_event.getY(1));
			}
			inputDown2 = false;
		} else if (tmpActionType == MotionEvent.ACTION_POINTER_3_DOWN) {
			if (SDK_VERSION >= 14) {
				final int typeOfPointer = _event.getToolType(2);
				if (    typeOfPointer == MotionEvent.TOOL_TYPE_FINGER
				     || typeOfPointer == MotionEvent.TOOL_TYPE_STYLUS) {
					m_galeNative.inputEventState(_event.getPointerId(2), true, (float)_event.getX(2), (float)_event.getY(2));
				} else if(typeOfPointer == MotionEvent.TOOL_TYPE_MOUSE) {
					m_galeNative.mouseEventState(_event.getPointerId(2), true, (float)_event.getX(2), (float)_event.getY(2));
				}
			} else {
				m_galeNative.inputEventState(_event.getPointerId(2), true, (float)_event.getX(2), (float)_event.getY(2));
			}
			inputDown3 = true;
		} else if (tmpActionType == MotionEvent.ACTION_POINTER_3_UP) {
			if (SDK_VERSION >= 14) {
				final int typeOfPointer = _event.getToolType(2);
				if (    typeOfPointer == MotionEvent.TOOL_TYPE_FINGER
				     || typeOfPointer == MotionEvent.TOOL_TYPE_STYLUS) {
					m_galeNative.inputEventState(_event.getPointerId(2), false, (float)_event.getX(2), (float)_event.getY(2));
				} else if(typeOfPointer == MotionEvent.TOOL_TYPE_MOUSE) {
					m_galeNative.mouseEventState(_event.getPointerId(2), false, (float)_event.getX(2), (float)_event.getY(2));
				}
			} else {
				m_galeNative.inputEventState(_event.getPointerId(2), false, (float)_event.getX(2), (float)_event.getY(2));
			}
			inputDown3 = false;
		} else if(tmpActionType == MotionEvent.ACTION_UP){
			if (inputDown1) {
				if (SDK_VERSION >= 14) {
					final int typeOfPointer = _event.getToolType(0);
					if (    typeOfPointer == MotionEvent.TOOL_TYPE_FINGER
					     || typeOfPointer == MotionEvent.TOOL_TYPE_STYLUS) {
						m_galeNative.inputEventState(_event.getPointerId(0), false, (float)_event.getX(0), (float)_event.getY(0));
					} else if(typeOfPointer == MotionEvent.TOOL_TYPE_MOUSE) {
						m_galeNative.mouseEventState(_event.getPointerId(0), false, (float)_event.getX(0), (float)_event.getY(0));
					}
				} else {
					m_galeNative.inputEventState(_event.getPointerId(0), false, (float)_event.getX(0), (float)_event.getY(0));
				}
				inputDown1 = false;
			} else if (inputDown2) {
				if (SDK_VERSION >= 14) {
					final int typeOfPointer = _event.getToolType(0);
					if (    typeOfPointer == MotionEvent.TOOL_TYPE_FINGER
					     || typeOfPointer == MotionEvent.TOOL_TYPE_STYLUS) {
						m_galeNative.inputEventState(_event.getPointerId(0), false, (float)_event.getX(0), (float)_event.getY(0));
					} else if(typeOfPointer == MotionEvent.TOOL_TYPE_MOUSE) {
						m_galeNative.mouseEventState(_event.getPointerId(0), false, (float)_event.getX(0), (float)_event.getY(0));
					}
				} else {
					m_galeNative.inputEventState(_event.getPointerId(0), false, (float)_event.getX(0), (float)_event.getY(0));
				}
				inputDown2 = false;
			} else {
				if (SDK_VERSION >= 14) {
					final int typeOfPointer = _event.getToolType(0);
					if (    typeOfPointer == MotionEvent.TOOL_TYPE_FINGER
					     || typeOfPointer == MotionEvent.TOOL_TYPE_STYLUS) {
						m_galeNative.inputEventState(_event.getPointerId(0), false, (float)_event.getX(0), (float)_event.getY(0));
					} else if(typeOfPointer == MotionEvent.TOOL_TYPE_MOUSE) {
						m_galeNative.mouseEventState(_event.getPointerId(0), false, (float)_event.getX(0), (float)_event.getY(0));
					}
				} else {
					m_galeNative.inputEventState(_event.getPointerId(0), false, (float)_event.getX(0), (float)_event.getY(0));
				}
				inputDown3 = false;
			}
		}
		return true;
	}
	
	private boolean keyboardEvent(int keyCode, KeyEvent _event, boolean _isDown) {
		int actionDone = _event.getAction();
		Log.i("Surface GL", "get event : " + keyCode + " is down : " + _isDown);
		switch(keyCode) {
			case KeyEvent.KEYCODE_VOLUME_DOWN:
				return m_galeNative.keyboardEventKeySystem(GALE_SYSTEM_KEY_VOLUME_DOWN, _isDown);
			case KeyEvent.KEYCODE_VOLUME_UP:
				return m_galeNative.keyboardEventKeySystem(GALE_SYSTEM_KEY_VOLUME_UP, _isDown);
			case KeyEvent.KEYCODE_MENU:
				return m_galeNative.keyboardEventKeySystem(GALE_SYSTEM_KEY_MENU, _isDown);
			case KeyEvent.KEYCODE_CAMERA:
				return m_galeNative.keyboardEventKeySystem(GALE_SYSTEM_KEY_CAMERA, _isDown);
			case KeyEvent.KEYCODE_HOME:
				return m_galeNative.keyboardEventKeySystem(GALE_SYSTEM_KEY_HOME, _isDown);
			case KeyEvent.KEYCODE_POWER:
				return m_galeNative.keyboardEventKeySystem(GALE_SYSTEM_KEY_POWER, _isDown);
			case KeyEvent.KEYCODE_BACK:
				// the back key is wrapped in the <esc> key to simplify PC validation ...
				return m_galeNative.keyboardEventKeySystem(GALE_SYSTEM_KEY_BACK, _isDown);
				/*
				m_galeNative.keyboardEventKey(GALE_SYSTEM_KEY_BACK, _isDown);
				return false;
				*/
			case KeyEvent.KEYCODE_DEL:
				m_galeNative.keyboardEventKey(GALE_SYSTEM_KEY_DEL, _isDown);
				return true;
				// Joystick event :
			case KeyEvent.KEYCODE_DPAD_UP:    m_galeNative.keyboardEventMove(GALE_MOVE_KEY_UP, _isDown);        return true;
			case KeyEvent.KEYCODE_DPAD_DOWN:  m_galeNative.keyboardEventMove(GALE_MOVE_KEY_DOWN, _isDown);      return true;
			case KeyEvent.KEYCODE_DPAD_LEFT:  m_galeNative.keyboardEventMove(GALE_MOVE_KEY_LEFT, _isDown);      return true;
			case KeyEvent.KEYCODE_DPAD_RIGHT: m_galeNative.keyboardEventMove(GALE_MOVE_KEY_RIGHT, _isDown);     return true;
			case KeyEvent.KEYCODE_PAGE_UP:    m_galeNative.keyboardEventMove(GALE_MOVE_KEY_PAGE_UP, _isDown);   return true;
			case KeyEvent.KEYCODE_PAGE_DOWN:  m_galeNative.keyboardEventMove(GALE_MOVE_KEY_PAGE_DOWN, _isDown); return true;
			case KeyEvent.KEYCODE_MOVE_HOME:  m_galeNative.keyboardEventMove(GALE_MOVE_KEY_START, _isDown);     return true;
			case KeyEvent.KEYCODE_MOVE_END:   m_galeNative.keyboardEventMove(GALE_MOVE_KEY_END, _isDown);       return true;
			case KeyEvent.KEYCODE_SYSRQ:      m_galeNative.keyboardEventMove(GALE_MOVE_KEY_PRINT, _isDown);     return true;
			//case KeyEvent.:  m_galeNative.keyboardEventMove(GALE_MOVE_KEY_STOP_DEFIL, _isDown); return true;
			case KeyEvent.KEYCODE_BREAK:      m_galeNative.keyboardEventMove(GALE_MOVE_KEY_WAIT, _isDown);      return true;
			case KeyEvent.KEYCODE_INSERT:     m_galeNative.keyboardEventMove(GALE_MOVE_KEY_INSERT, _isDown);    return true;
			case KeyEvent.KEYCODE_F1:  m_galeNative.keyboardEventMove(GALE_MOVE_KEY_F1, _isDown);  return true;
			case KeyEvent.KEYCODE_F2:  m_galeNative.keyboardEventMove(GALE_MOVE_KEY_F2, _isDown);  return true;
			case KeyEvent.KEYCODE_F3:  m_galeNative.keyboardEventMove(GALE_MOVE_KEY_F3, _isDown);  return true;
			case KeyEvent.KEYCODE_F4:  m_galeNative.keyboardEventMove(GALE_MOVE_KEY_F4, _isDown);  return true;
			case KeyEvent.KEYCODE_F5:  m_galeNative.keyboardEventMove(GALE_MOVE_KEY_F5, _isDown);  return true;
			case KeyEvent.KEYCODE_F6:  m_galeNative.keyboardEventMove(GALE_MOVE_KEY_F6, _isDown);  return true;
			case KeyEvent.KEYCODE_F7:  m_galeNative.keyboardEventMove(GALE_MOVE_KEY_F7, _isDown);  return true;
			case KeyEvent.KEYCODE_F8:  m_galeNative.keyboardEventMove(GALE_MOVE_KEY_F8, _isDown);  return true;
			case KeyEvent.KEYCODE_F9:  m_galeNative.keyboardEventMove(GALE_MOVE_KEY_F9, _isDown);  return true;
			case KeyEvent.KEYCODE_F10: m_galeNative.keyboardEventMove(GALE_MOVE_KEY_F10, _isDown); return true;
			case KeyEvent.KEYCODE_F11: m_galeNative.keyboardEventMove(GALE_MOVE_KEY_F11, _isDown); return true;
			case KeyEvent.KEYCODE_F12: m_galeNative.keyboardEventMove(GALE_MOVE_KEY_F12, _isDown); return true;
			case KeyEvent.KEYCODE_CAPS_LOCK:   m_galeNative.keyboardEventMove(GALE_MOVE_KEY_CAP_LOCK, _isDown);     return true;
			case KeyEvent.KEYCODE_SHIFT_LEFT:  m_galeNative.keyboardEventMove(GALE_MOVE_KEY_SHIFT_LEFT, _isDown);   return true;
			case KeyEvent.KEYCODE_SHIFT_RIGHT: m_galeNative.keyboardEventMove(GALE_MOVE_KEY_SHIFT_RIGHT, _isDown);  return true;
			case KeyEvent.KEYCODE_CTRL_LEFT:   m_galeNative.keyboardEventMove(GALE_MOVE_KEY_CTRL_LEFT, _isDown);    return true;
			case KeyEvent.KEYCODE_CTRL_RIGHT:  m_galeNative.keyboardEventMove(GALE_MOVE_KEY_CTRL_RIGHT, _isDown);   return true;
			case KeyEvent.KEYCODE_META_LEFT:   m_galeNative.keyboardEventMove(GALE_MOVE_KEY_META_LEFT, _isDown);    return true;
			case KeyEvent.KEYCODE_META_RIGHT:  m_galeNative.keyboardEventMove(GALE_MOVE_KEY_META_RIGHT, _isDown);   return true;
			case KeyEvent.KEYCODE_ALT_LEFT:    m_galeNative.keyboardEventMove(GALE_MOVE_KEY_ALT, _isDown);          return true;
			case KeyEvent.KEYCODE_ALT_RIGHT:   m_galeNative.keyboardEventMove(GALE_MOVE_KEY_ALT_GR, _isDown);       return true;
			case KeyEvent.KEYCODE_NUM_LOCK:    m_galeNative.keyboardEventMove(GALE_MOVE_KEY_NUM_LOCK, _isDown);     return true;
			default:
				break;
			}
		// key wrapping :
		if (    (actionDone == KeyEvent.ACTION_DOWN)
		     || (actionDone == KeyEvent.ACTION_MULTIPLE)
		     || (actionDone == KeyEvent.ACTION_UP)) {
			// convert the key in UniChar to prevent errors ...
			int uchar = _event.getUnicodeChar();
			// pb on the return methode ... in java it is like windows ...
			if (uchar == '\r') {
				uchar = '\n';
			}
			// send it to gale ...
			m_galeNative.keyboardEventKey(uchar, _isDown);
			return true;
		}
		return false;
	}
	
	public boolean onKeyDown(int _keyCode, KeyEvent _event) {
		return keyboardEvent(_keyCode, _event, true);
	}
	
	public boolean onKeyUp(int _keyCode, KeyEvent _event) {
		return keyboardEvent(_keyCode, _event, false);
	}
}
