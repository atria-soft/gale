/**
 * @author Edouard DUPIN, Kevin BILLONNEAU
 *
 * @copyright 2011, Edouard DUPIN, all right reserved
 *
 * @license MPL v2.0 (see license file)
 */

package org.gale;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.os.Process;

// import the gale package :
/* no need in same package... */
//import org.gale.Gale;

import org.gale.Gale;

/**
 * @brief Class : 
 *
 */
public class GaleRendererGL implements GLSurfaceView.Renderer
{
	private Gale m_galeNative;
	
	public GaleRendererGL(Gale _galeInstance) {
		m_galeNative = _galeInstance;
	}
	public void onSurfaceCreated(GL10 _gl, EGLConfig _config) {
		android.os.Process.setThreadPriority(android.os.Process.THREAD_PRIORITY_URGENT_DISPLAY);
		m_galeNative.renderInit();
	}
	
	public void onSurfaceChanged(GL10 _gl, int _w, int _h) {
		android.os.Process.setThreadPriority(android.os.Process.THREAD_PRIORITY_URGENT_DISPLAY);
		m_galeNative.renderResize(_w, _h);
	}
	
	public void onDrawFrame(GL10 _gl) {
		android.os.Process.setThreadPriority(android.os.Process.THREAD_PRIORITY_URGENT_DISPLAY);
		m_galeNative.renderDraw();
	}
}
