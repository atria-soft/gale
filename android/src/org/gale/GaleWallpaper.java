/**
 * @author Edouard DUPIN
 *
 * @copyright 2011, Edouard DUPIN, all right reserved
 *
 * @license APACHE v2.0 (see license file)
 */

package org.gale;

import android.app.ActivityManager;
import android.content.pm.ApplicationInfo;
import android.content.pm.ConfigurationInfo;
import android.content.pm.PackageManager;
import android.content.pm.PackageManager.NameNotFoundException;
import android.content.Context;
import android.opengl.GLSurfaceView;
import android.opengl.GLSurfaceView.Renderer;
import android.os.Build;
import android.service.wallpaper.WallpaperService;
import android.util.Log;
import android.view.SurfaceHolder;
import org.gale.GaleSurfaceViewGL;
import android.view.MotionEvent;

import android.net.Uri;
import android.content.Intent;
import android.content.ActivityNotFoundException;

import org.gale.Gale;

public abstract class GaleWallpaper extends WallpaperService implements GaleCallback, GaleConstants
{
	private GLEngine mGLView;
	private Gale m_galeNative;
	
	protected void initApkPath(String _org, String _vendor, String _project, String _applName) {
		StringBuilder sb = new StringBuilder();
		sb.append(_org).append(".");
		sb.append(_vendor).append(".");
		sb.append(_project);
		String apkFilePath = null;
		ApplicationInfo appInfo = null;
		PackageManager packMgmr = getPackageManager();
		try {
			appInfo = packMgmr.getApplicationInfo(sb.toString(), 0);
		} catch (NameNotFoundException e) {
			e.printStackTrace();
			throw new RuntimeException("Unable to locate assets, aborting...");
		}
		apkFilePath = appInfo.sourceDir;
		m_galeNative.paramSetArchiveDir(0, apkFilePath, _applName);
	}
	
	@Override public Engine onCreateEngine() {
		// set the java evironement in the C sources :
		m_galeNative = new Gale(this, GALE_APPL_TYPE_WALLPAPER);
		
		// Load the application directory
		m_galeNative.paramSetArchiveDir(1, getFilesDir().toString(), "");
		m_galeNative.paramSetArchiveDir(2, getCacheDir().toString(), "");
		// to enable extarnal storage: add in the manifest the restriction needed ...
		//packageManager.checkPermission("android.permission.READ_SMS", myPackage) == PERMISSION_GRANTED; 
		//Gale.paramSetArchiveDir(3, getExternalCacheDir().toString());
		
		
		//! DisplayMetrics metrics = new DisplayMetrics();
		//! getWindowManager().getDefaultDisplay().getMetrics(metrics);
		//! m_galeNative.displayPropertyMetrics(metrics.xdpi, metrics.ydpi);
		
		// call C init ...
		m_galeNative.onCreate();
		
		// create bsurface system
		mGLView = new GLEngine(m_galeNative);
		
		return mGLView;
	}
	
	public class GLEngine extends Engine {
		private Gale m_galeNative;
		public GLEngine(Gale _galeInstance) {
			m_galeNative = _galeInstance;
		}
		
		class WallpaperGLSurfaceView extends GaleSurfaceViewGL {
			private static final String TAG = "WallpaperGLSurfaceView";
			WallpaperGLSurfaceView(Context _context, Gale _galeInstance) {
				super(_context, _galeInstance);
				Log.d(TAG, "WallpaperGLSurfaceView(" + _context + ")");
			}
			@Override public SurfaceHolder getHolder() {
				Log.d(TAG, "getHolder(): returning " + getSurfaceHolder());
				return getSurfaceHolder();
			}
			public void onDestroy() {
				Log.d(TAG, "onDestroy()");
				super.onDetachedFromWindow();
			}
		}
		
		private static final String TAG = "GLEngine";
		private WallpaperGLSurfaceView glSurfaceView;
		
		@Override public void onCreate(SurfaceHolder _surfaceHolder) {
			Log.d(TAG, "onCreate(" + _surfaceHolder + ")");
			super.onCreate(_surfaceHolder);
			
			glSurfaceView = new WallpaperGLSurfaceView(GaleWallpaper.this, m_galeNative);
			
			// Check if the system supports OpenGL ES 2.0.
			final ActivityManager activityManager = (ActivityManager) getSystemService(Context.ACTIVITY_SERVICE);
			final ConfigurationInfo configurationInfo = activityManager.getDeviceConfigurationInfo();
			final boolean supportsEs2 = configurationInfo.reqGlEsVersion >= 0x20000;
			
			if (supportsEs2 == false) {
				Log.d("LiveWallpaper", "does not support board with only open GL ES 1");
				return;
			}
			// Request an OpenGL ES 2.0 compatible context.
			//setEGLContextClientVersion(2);
			
			// On Honeycomb+ devices, this improves the performance when
			// leaving and resuming the live wallpaper.
			//setPreserveEGLContextOnPause(true);
			
		}
		
		@Override public void onTouchEvent(MotionEvent _event) {
			glSurfaceView.onTouchEvent(_event);
		}
		
		@Override public void onVisibilityChanged(boolean _visible) {
			Log.d(TAG, "onVisibilityChanged(" + _visible + ")");
			super.onVisibilityChanged(_visible);
			if (_visible == true) {
				glSurfaceView.onResume();
				// call C
				m_galeNative.onResume();
			} else {
				glSurfaceView.onPause();
				// call C
				m_galeNative.onPause();
			}
		}
		
		@Override public void onDestroy() {
			Log.d(TAG, "onDestroy()");
			super.onDestroy();
			// call C
			m_galeNative.onStop();
			m_galeNative.onDestroy();
			glSurfaceView.onDestroy();
		}
		
		protected void setPreserveEGLContextOnPause(boolean _preserve) {
			if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.HONEYCOMB) {
				Log.d(TAG, "setPreserveEGLContextOnPause(" + _preserve + ")");
				glSurfaceView.setPreserveEGLContextOnPause(_preserve);
			}
		}
		
		protected void setEGLContextClientVersion(int _version) {
			Log.d(TAG, "setEGLContextClientVersion(" + _version + ")");
			glSurfaceView.setEGLContextClientVersion(_version);
		}
	}
	
	public void keyboardUpdate(boolean _show) {
		// never display keyboard on wallpaer...
		Log.d("GaleCallback", "KEABOARD UPDATE is not implemented ...");
	}
	
	public void eventNotifier(String[] _args) {
		// just for the test ...
		Log.d("GaleCallback", "EVENT NOTIFIER is not implemented ...");
		
	}
	
	public void orientationUpdate(int _screenMode) {
		Log.d("GaleCallback", "SET ORIENTATION is not implemented ...");
	}
	
	public void titleSet(String _value) {
		// no title in the wallpaper ...
		Log.d("GaleCallback", "SET TITLE is not implemented ...");
	}
	
	public void openURI(String _uri) {
		try {
			Intent myIntent = new Intent(Intent.ACTION_VIEW, Uri.parse(_uri));
			startActivity(myIntent);
		} catch (ActivityNotFoundException e) {
			Log.e("GaleActivity", "Can not request an URL");
		}
	}
	
	public void stop() {
		Log.d("GaleCallback", "STOP is not implemented ...");
	}
}


