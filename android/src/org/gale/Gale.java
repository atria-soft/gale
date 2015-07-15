/**
 * @author Edouard DUPIN, Kevin BILLONNEAU
 *
 * @copyright 2011, Edouard DUPIN, all right reserved
 *
 * @license APACHE v2.0 (see license file)
 */

package org.gale;

import android.app.Activity;
import android.service.wallpaper.WallpaperService;
import android.service.wallpaper.WallpaperService.Engine;
import android.util.Log;

public class Gale {
	private int m_instanceID = -1; // local and private instance ID
	private boolean m_hardKeyboardHidden = true;
	
	public <T extends GaleCallback> Gale(T _activityInstance, int _typeApplication) {
		m_instanceID = -1;
		m_hardKeyboardHidden = true;
		m_instanceID = EWsetJavaVirtualMachineStart(_activityInstance, _typeApplication);
		Log.d("Gale", "new : " + m_instanceID);
	}
	
	public void setJavaVirtualMachineStop() {
		EWsetJavaVirtualMachineStop(m_instanceID);
	}
	public void paramSetArchiveDir(int _mode, String _myString) {
		EWparamSetArchiveDir(m_instanceID, _mode, _myString);
	}
	
	public boolean getHardKeyboardHidden() {
		return m_hardKeyboardHidden;
	}
	public void setHardKeyboardHidden(boolean _val) {
		m_hardKeyboardHidden = _val;
	}
	
	// activity status
	public void onCreate() {
		EWonCreate(m_instanceID);
	}
	public void onStart() {
		EWonStart(m_instanceID);
	}
	public void onReStart() {
		EWonReStart(m_instanceID);
	}
	public void onResume() {
		EWonResume(m_instanceID);
	}
	public void onPause() {
		EWonPause(m_instanceID);
	}
	public void onStop() {
		EWonStop(m_instanceID);
	}
	public void onDestroy() {
		EWonDestroy(m_instanceID);
	}
	// set display properties :
	public void displayPropertyMetrics(float _ratioX, float _ratioY) {
		EWdisplayPropertyMetrics(m_instanceID, _ratioX, _ratioY);
	}
	// IO native function :
	// Specific for the type of input : TOOL_TYPE_FINGER and TOOL_TYPE_STYLUS (work as the same)
	public void inputEventMotion(int _pointerID, float _x, float _y) {
		EWinputEventMotion(m_instanceID, _pointerID, _x, _y);
	}
	public void inputEventState(int _pointerID, boolean _isDown, float _x, float _y) {
		EWinputEventState(m_instanceID, _pointerID, _isDown, _x, _y);
	}
	// Specific for the type of input : TOOL_TYPE_MOUSE
	public void mouseEventMotion(int _pointerID, float _x, float _y) {
		EWmouseEventMotion(m_instanceID, _pointerID, _x, _y);
	}
	public void mouseEventState(int _pointerID, boolean _isDown, float _x, float _y) {
		EWmouseEventState(m_instanceID, _pointerID, _isDown, _x, _y);
	}
	// other unknow event ...
	public void unknowEvent(int _eventID) {
		EWunknowEvent(m_instanceID, _eventID);
	}
	
	public void keyboardEventMove(int _type, boolean _isDown) {
		EWkeyboardEventMove(m_instanceID, _type, _isDown);
	}
	public void keyboardEventKey(int _uniChar, boolean _isDown) {
		EWkeyboardEventKey(m_instanceID, _uniChar, _isDown);
	}
	
	public boolean keyboardEventKeySystem(int _keyVal, boolean _isDown) {
		return EWkeyboardEventKeySystem(m_instanceID, _keyVal, _isDown);
	}
	// renderer Event : 
	public void renderInit() {
		EWrenderInit(m_instanceID);
	}
	public void renderResize(int _w, int _h) {
		EWrenderResize(m_instanceID, _w, _h);
	}
	public void renderDraw() {
		EWrenderDraw(m_instanceID);
	}
	
	
	private native <T extends GaleCallback> int EWsetJavaVirtualMachineStart(T _activityInstance, int _typeApplication);
	private native void EWsetJavaVirtualMachineStop(int _instanceId);
	private native void EWparamSetArchiveDir(int _instanceId, int _mode, String _myString);
	
	// activity status
	private native void EWonCreate(int _instanceId);
	private native void EWonStart(int _instanceId);
	private native void EWonReStart(int _instanceId);
	private native void EWonResume(int _instanceId);
	private native void EWonPause(int _instanceId);
	private native void EWonStop(int _instanceId);
	private native void EWonDestroy(int _instanceId);
	// set display properties :
	private native void EWdisplayPropertyMetrics(int _instanceId, float _ratioX, float _ratioY);
	// IO native function :
	// Specific for the type of input : TOOL_TYPE_FINGER and TOOL_TYPE_STYLUS (work as the same)
	private native void EWinputEventMotion(int _instanceId, int _pointerID, float _x, float _y);
	private native void EWinputEventState(int _instanceId, int _pointerID, boolean _isDown, float _x, float _y);
	// Specific for the type of input : TOOL_TYPE_MOUSE
	private native void EWmouseEventMotion(int _instanceId, int _pointerID, float _x, float _y);
	private native void EWmouseEventState(int _instanceId, int _pointerID, boolean _isDown, float _x, float _y);
	// other unknow event ...
	private native void EWunknowEvent(int _instanceId, int _eventID);
	
	private native void EWkeyboardEventMove(int _instanceId, int _type, boolean _isDown);
	private native void EWkeyboardEventKey(int _instanceId, int _uniChar, boolean _isDown);
	
	private native boolean EWkeyboardEventKeySystem(int _instanceId, int _keyVal, boolean _isDown);
	// renderer Event : 
	private native void EWrenderInit(int _instanceId);
	private native void EWrenderResize(int _instanceId, int _w, int _h);
	private native void EWrenderDraw(int _instanceId);
}

