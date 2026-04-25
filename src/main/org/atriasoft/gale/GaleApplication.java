package org.atriasoft.gale;

import java.time.Clock;

import org.atriasoft.etk.Uri;
import org.atriasoft.etk.math.Vector2f;
import org.atriasoft.gale.context.ClipboardList;
import org.atriasoft.gale.context.Cursor;
import org.atriasoft.gale.context.GaleContext;
import org.atriasoft.gale.key.KeyKeyboard;
import org.atriasoft.gale.key.KeySpecial;
import org.atriasoft.gale.key.KeyStatus;
import org.atriasoft.gale.key.KeyType;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class GaleApplication {
	static final Logger LOGGER = LoggerFactory.getLogger(GaleApplication.class);
	private boolean needRedraw = true;
	private String title = "gale";
	private Uri iconName = null;
	private final Cursor cursor = Cursor.arrow;
	private Orientation orientation = Orientation.screenAuto;
	private Vector2f windowsSize = new Vector2f(800, 600);
	private Vector2f windowsPosition = null; //!< null means "let the system center the window"
	private int monitorIndex = -1; //!< -1 means "use the system default / primary monitor"

	public GaleApplication() {
		LOGGER.trace("Constructor Gale Application");
	}

	/**
	 * Exit the application (not availlable on IOs, ==> the user will not understand the comportement. He will think the application has crashed (Apple philosophie))
	 * @param value value to return on the program
	 */
	public void exit(final int value) {
		LOGGER.trace("Exit Requested {}", value);
		Gale.getContext().stop();
	}

	public float getAspectRatio() {
		return this.windowsSize.x() / this.windowsSize.y();
	}

	/**
	 * Get the cursor type.
	 * @return the current cursor.
	 */
	public Cursor getCursor() {
		return this.cursor;
	}

	/**
	 * Get the current filename of the application.
	 * @return Filename of the icon.
	 */
	public Uri getIcon() {
		return this.iconName;
	}

	/**
	 * get the screen orientation (if possible : only on iOs/Android)
	 * @return Current orientation.
	 */
	public Orientation getOrientation() {
		return this.orientation;
	}

	/**
	 * Get the position of the window.
	 * @return Current position of the window (top-left corner, in absolute screen coordinates),
	 *         or null if no position has been set yet.
	 */
	public Vector2f getPosition() {
		return this.windowsPosition;
	}

	/**
	 * Get the monitor index where the window is currently placed (or where it should be placed at startup).
	 * @return monitor index, or -1 for system default / primary monitor.
	 */
	public int getMonitorIndex() {
		return this.monitorIndex;
	}

	/**
	 * Get the size of the window.
	 * @return Current size of the window.
	 */
	public Vector2f getSize() {
		return this.windowsSize;
	}

	/**
	 * Get the current title of the application
	 * @return Current title
	 */
	public String getTitle() {
		return this.title;
	}

	public boolean isDrawingNeeded() {
		final boolean tmp = this.needRedraw;
		this.needRedraw = false;
		return tmp;
	}

	/**
	 * Hide the virtal keyboard (if possible : only on iOs/Android)
	 */
	public void keyboardHide() {
		final GaleContext context = Gale.getContext();
		if (context == null) {
			return;
		}
		context.keyboardHide();
	}

	/**
	 * Show the virtal keyboard (if possible : only on iOs/Android)
	 */
	public void keyboardShow() {
		final GaleContext context = Gale.getContext();
		if (context == null) {
			return;
		}
		context.keyboardShow();
	}

	public void markDrawingIsNeeded() {
		this.needRedraw = true;
	}

	/**
	 * A clipboard data is back (apear after a request of a new clipboard).
	 * @param clipboardId Id of the clipboard.
	 */
	public void onClipboardEvent(final ClipboardList clipboardId) {

	}

	/**
	 * The application is created.
	 * @param context Current gale context.
	 */
	public void onCreate(final GaleContext context) {
		LOGGER.trace("Create Gale Application");
	}

	/**
	 * The application is removed (call destructor just adter it.).
	 * @param context Current gale context.
	 */
	public void onDestroy(final GaleContext context) {
		LOGGER.trace("Destroy Gale Application");
	}

	/**
	 * Real draw of the application
	 * @param context Current gale context.
	 */
	public void onDraw(final GaleContext context) {
		LOGGER.trace("draw Gale Application");
	}

	/**
	 * Get keyborad value input.
	 * @param special Current special key status (ctrl/alt/shift ...).
	 * @param type Type of the event.
	 * @param value Unicode value of the char pushed (viable only if type==gale::key::keyboard::character).
	 * @param state State of the key (up/down/upRepeate/downRepeate)
	 */
	public void onKeyboard(
			final KeySpecial special,
			final KeyKeyboard type,
			final Character value,
			final KeyStatus state) {

	}

	/**
	 * The user request application removing.
	 * @param context Current gale context.
	 */
	public void onKillDemand(final GaleContext context) {
		LOGGER.info("Gale request auto destroy ==> no applification specification");
		System.exit(0);
	}

	/**
	 * Event generated when user change the position of the window.
	 * @param pos New position of the window (top-left corner, absolute screen coordinates).
	 */
	public void onMovePosition(final Vector2f pos) {
		this.windowsPosition = pos;
	}

	/**
	 * Event generated when the window changes monitor (because the user dragged it across screens
	 * or because the monitor configuration changed).
	 * @param monitorIndex New monitor index (0-based).
	 */
	public void onMonitorChange(final int monitorIndex) {
		this.monitorIndex = monitorIndex;
	}

	/**
	 * The application is Hide / not visible.
	 * @param context Current gale context.
	 */
	public void onPause(final GaleContext context) {
		LOGGER.trace("Pause Gale Application");
	}

	/**
	 * Call when contrext finish process event in the buffer and add a latency of 10ms between calls
	 * @param clock Current time of the call;
	 * @param time time of the program in nanoseconds (monotonic) ==> need restart application approximately every 292 years
	 */
	public void onPeriod(final Clock clock, final long time) {}

	/**
	 * Get touch/mouse/... event.
	 * @param type Type of pointer event
	 * @param pointerID Pointer id of the touch event.
	 * @param pos Position of the event (can be <0 if out of window).
	 * @param state Key state (up/down/move)
	 */
	public void onPointer(
			final KeySpecial special,
			final KeyType type,
			final int pointerID,
			final Vector2f pos,
			final KeyStatus state) {

	}

	/**
	 * call application to precalculate drawing.
	 * @param context Current gale context.
	 */
	public void onRegenerateDisplay(final GaleContext context) {
		//LOGGER.trace("Regenerate Gale Application");
		markDrawingIsNeeded();
	}

	/**
	 * Event generated when user change the size of the window.
	 * @param size New size of the window.
	 */
	public void onResize(final Vector2f size) {
		if (size == null) {
			LOGGER.error("Try to set a null size ...");
			return;
		}
		this.windowsSize = size;
		markDrawingIsNeeded();
	}

	/**
	 * The application is resumed (now visible).
	 * @param context Current gale context.
	 */
	public void onResume(final GaleContext context) {
		LOGGER.trace("Start Gale Application");
	}

	/**
	 * The application is started.
	 * @param context Current gale context.
	 */
	public void onStart(final GaleContext context) {
		LOGGER.trace("Start Gale Application");
	}

	/**
	 * The application is stopped.
	 * @param context Current gale context.
	 */
	public void onStop(final GaleContext context) {
		LOGGER.trace("Stop Gale Application");
	}

	/**
	 * Set the cursor type.
	 * @param newCursor Selected cursor.
	 */
	public void setCursor(final Cursor newCursor) {
		Gale.getContext().setCursor(this.cursor);
	}

	void setForceRedraw() {
		this.needRedraw = true;
	}

	/**
	 * set the Icon of the application.
	 * @param iconFile File name icon (.bmp/.png).
	 */
	public void setIcon(final Uri iconFile) {
		this.iconName = iconFile;
		Gale.getContext().setIcon(this.iconName);
	}

	/**
	 * set the screen orientation (if possible : only on iOs/Android)
	 * @param orientation New orientation.
	 */
	public void setOrientation(final Orientation orientation) {
		this.orientation = orientation;
		Gale.getContext().forceOrientation(this.orientation);
	}

	/**
	 * Set the position of the window (if possible: Android and Ios does not support it).
	 * If called BEFORE the GaleContext is created (i.e. before {@link Gale#run}), the value is
	 * stored as a startup preference and applied when the window is shown.
	 * If called AFTER, the window is moved live.
	 * @param pos New position of the window (top-left corner, absolute screen coordinates).
	 */
	public void setPosition(final Vector2f pos) {
		this.windowsPosition = pos;
		final GaleContext context = Gale.getContext();
		if (context == null) {
			// Pre-init: just remember the value; ContextLWJGLAWT will read it in initWindows().
			return;
		}
		context.setPos(pos);
	}

	/**
	 * Set the monitor where the window should be placed.
	 * If called BEFORE the GaleContext is created, the value is stored as a startup preference.
	 * If called AFTER, the window is moved to that monitor (centered on it, keeping current size).
	 * @param index Monitor index (0-based). Use -1 for system default / primary monitor.
	 */
	public void setMonitorIndex(final int index) {
		this.monitorIndex = index;
		final GaleContext context = Gale.getContext();
		if (context == null) {
			return;
		}
		context.setMonitor(index);
	}

	/**
	 * Set the size of the window (if possible: Android and Ios does not support it).
	 * If called BEFORE the GaleContext is created (i.e. before {@link Gale#run}), the value is
	 * stored as a startup preference and the window is created at this size.
	 * If called AFTER, the window is resized live.
	 * @param size New size of the window.
	 */
	public void setSize(final Vector2f size) {
		if (size.x() <= 0 || size.y() <= 0) {
			LOGGER.error("Wrong windows size: {}", size);
			return;
		}
		this.windowsSize = size;
		final GaleContext context = Gale.getContext();
		if (context == null) {
			// Pre-init: just remember the value; ContextLWJGLAWT will read it in initWindows().
			return;
		}
		context.setSize(size);
	}

	/**
	 * Set the title of the application
	 * @param title New title to set at the application (if possible: Android and Ios does not support it)
	 */
	public void setTitle(final String title) {
		this.title = title;
		final GaleContext context = Gale.getContext();
		if (context == null) {
			return;
		}
		context.setTitle(this.title);
	}
}
