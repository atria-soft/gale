package org.atriasoft.gale;

import org.atriasoft.etk.Uri;
import org.atriasoft.etk.math.Vector2f;
import org.atriasoft.gale.context.ClipboardList;
import org.atriasoft.gale.context.Context;
import org.atriasoft.gale.context.Cursor;
import org.atriasoft.gale.internal.Log;
import org.atriasoft.gale.key.KeyKeyboard;
import org.atriasoft.gale.key.KeySpecial;
import org.atriasoft.gale.key.KeyStatus;
import org.atriasoft.gale.key.KeyType;

public class Application {
	public boolean canDraw = false;
	private boolean needRedraw = true;
	private String title = "gale";
	private Uri iconName = new Uri("");
	private Cursor cursor = Cursor.arrow;
	private Orientation orientation = Orientation.screenAuto;
	Vector2f windowsSize = new Vector2f(800,600);
	
	public Application() {
		Log.verbose("Constructor Gale Application");
	}
	/**
	 * @brief The application is created.
	 * @param context Current gale context.
	 */
	public void onCreate(Context context) {
		Log.verbose("Create Gale Application");
	}
	/**
	 * @brief The application is started.
	 * @param context Current gale context.
	 */
	public void onStart(Context context){
		Log.verbose("Start Gale Application");
	}
	/**
	 * @brief The application is resumed (now visible).
	 * @param context Current gale context.
	 */
	public void onResume(Context context){
		Log.verbose("Start Gale Application");
	}
	/**
	 * @brief call application to precalculate drawing.
	 * @param context Current gale context.
	 */
	public void onRegenerateDisplay(Context context) {
		//Log.verbose("Regenerate Gale Application");
		markDrawingIsNeeded();
	}

	/**
	 * @brief Real draw of the application
	 * @param context Current gale context.
	 */
	public void onDraw(Context context) {
		Log.verbose("draw Gale Application");
	}
	/**
	 * @brief The application is Hide / not visible.
	 * @param context Current gale context.
	 */
	public void onPause(Context context) {
		Log.verbose("Pause Gale Application");
	}
	/**
	 * @brief The application is stopped.
	 * @param context Current gale context.
	 */
	public void onStop(Context context) {
		Log.verbose("Stop Gale Application");
	}
	/**
	 * @brief The application is removed (call destructor just adter it.).
	 * @param context Current gale context.
	 */
	public void onDestroy(Context context) {
		Log.verbose("Destroy Gale Application");
	}
	/**
	 * @brief The user request application removing.
	 * @param context Current gale context.
	 */
	public void onKillDemand(Context context) {
		Log.info("Gale request auto destroy ==> no applification specification");
		System.exit(0);
	}
	/**
	 * @brief Exit the application (not availlable on IOs, ==> the user will not understand the comportement. He will think the application has crashed (Apple philosophie))
	 * @param value value to return on the program
	 */
	public void exit(int value) {
		Log.verbose("Exit Requested " + value);
		Gale.getContext().stop();
	}
	

	public void markDrawingIsNeeded() {
		this.needRedraw = true;
	}
	public boolean isDrawingNeeded() {
		boolean tmp = this.needRedraw;
		this.needRedraw = false;
		return tmp;
	}
	/**
	 * @brief Get touch/mouse/... event.
	 * @param type Type of pointer event
	 * @param pointerID Pointer id of the touch event.
	 * @param pos Position of the event (can be <0 if out of window).
	 * @param state Key state (up/down/move)
	 */
	public void onPointer(KeySpecial special,
			KeyType type,
			int pointerID,
			Vector2f pos,
			KeyStatus state) {
		
	}
	/**
	 * @brief Get keyborad value input.
	 * @param special Current special key status (ctrl/alt/shift ...).
	 * @param type Type of the event.
	 * @param value Unicode value of the char pushed (viable only if type==gale::key::keyboard::character).
	 * @param state State of the key (up/down/upRepeate/downRepeate)
	 */
	public void onKeyboard(KeySpecial special,
	                       KeyKeyboard type,
	                       Character value,
	                       KeyStatus state) {
		
	}
	/**
	 * @brief Show the virtal keyboard (if possible : only on iOs/Android)
	 */
	public void keyboardShow() {
		Context context = Gale.getContext();
		if (context == null) {
			return;
		}
		context.keyboardShow();
	}
	/**
	 * @brief Hide the virtal keyboard (if possible : only on iOs/Android)
	 */
	public void keyboardHide() {
		Context context = Gale.getContext();
		if (context == null) {
			return;
		}
		context.keyboardHide();
	}
	/**
	 * @brief Event generated when user change the size of the window.
	 * @param size New size of the window.
	 */
	public void onResize(Vector2f size) {
		if (size == null) {
			Log.error("Try to set a null size ...");
			return;
		}
		windowsSize = size;
	}
	/**
	 * @brief Set the size of the window (if possible: Android and Ios does not support it)
	 * @param size New size of the window.
	 * @return 
	 */
	public void setSize(Vector2f size) {
		windowsSize = size;
		Context context = Gale.getContext();
		if (context == null) {
			return;
		}
		context.setSize(size);
	}
	/**
	 * @brief Get the size of the window.
	 * @return Current size of the window.
	 */
	public Vector2f getSize() {
		return windowsSize;
	}
	public float getAspectRatio() {
		return windowsSize.x/windowsSize.y;
	}

	/**
	 * @brief Event generated when user change the position of the window.
	 * @param size New position of the window.
	 */
	public void onMovePosition(Vector2f size) {
		
	}
	/**
	 * @brief Set the position of the window (if possible: Android and Ios does not support it)
	 * @param size New position of the window.
	 */
	public void setPosition(Vector2f size) {
		
	}
	/**
	 * @brief Get the position of the window.
	 * @return Current position of the window.
	 */
	public Vector2f getPosition() {
		return new Vector2f(0,0);
	}

	/**
	 * @brief Set the title of the application
	 * @param title New title to set at the application (if possible: Android and Ios does not support it)
	 */
	public void setTitle(String title) {
		this.title = title;
		Context context = Gale.getContext();
		if (context == null) {
			return;
		}
		context.setTitle(this.title);
	}
	/**
	 * @brief Get the current title of the application
	 * @return Current title
	 */
	public String getTitle() {
		return this.title;
	}
	/**
	 * @brief set the Icon of the application.
	 * @param iconFile File name icon (.bmp/.png).
	 */
	public void setIcon(Uri iconFile) {
		this.iconName = iconFile;
		Gale.getContext().setIcon(this.iconName);
	}
	/**
	 * @brief Get the current filename of the application.
	 * @return Filename of the icon.
	 */
	public Uri getIcon() {
		return this.iconName;
	}

	/**
	 * @brief Set the cursor type.
	 * @param newCursor Selected cursor.
	 */
	public void setCursor(Cursor newCursor) {
		Gale.getContext().setCursor(this.cursor);
	}
	/**
	 * @brief Get the cursor type.
	 * @return the current cursor.
	 */
	public Cursor getCursor() {
		return this.cursor;
	}
	/**
	 * @brief set the screen orientation (if possible : only on iOs/Android)
	 * @param orientation New orientation.
	 */
	public void setOrientation(Orientation orientation) {
		this.orientation = orientation;
		Gale.getContext().forceOrientation(this.orientation);
	}
	/**
	 * @brief get the screen orientation (if possible : only on iOs/Android)
	 * @return Current orientation.
	 */
	public Orientation getOrientation() {
		return this.orientation;
	}

	/**
	 * @brief A clipboard data is back (apear after a request of a new clipboard).
	 * @param clipboardId Id of the clipboard.
	 */
	public void onClipboardEvent(ClipboardList clipboardId) {
		
	}

	/**
	 * @brief Call every time a draw is called (not entirely periodic, but faster at we can ...
	 * @param time Current time of the call;
	 */
	public void onPeriod(long time) {};
}
