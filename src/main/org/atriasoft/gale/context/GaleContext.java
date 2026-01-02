package org.atriasoft.gale.context;

import java.time.Clock;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

import org.atriasoft.etk.Color;
import org.atriasoft.etk.Dimension2f;
import org.atriasoft.etk.ThreadAbstract;
import org.atriasoft.etk.Uri;
import org.atriasoft.etk.math.Vector2f;
import org.atriasoft.etk.util.AutoUnLock;
import org.atriasoft.gale.Fps;
import org.atriasoft.gale.Gale;
import org.atriasoft.gale.GaleApplication;
import org.atriasoft.gale.Orientation;
import org.atriasoft.gale.backend3d.OpenGL;
import org.atriasoft.gale.key.KeyKeyboard;
import org.atriasoft.gale.key.KeySpecial;
import org.atriasoft.gale.key.KeyStatus;
import org.atriasoft.gale.key.KeyType;
import org.atriasoft.gale.resource.ResourceManager;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

interface ActionToDoInAsyncLoop {
	void run(GaleContext context);
}

enum ApplicationState {
	UNDEFINED, CREATE, RUNNING, DIED
}

public abstract class GaleContext {
	static final Logger LOGGER = LoggerFactory.getLogger(GaleContext.class);
	protected static final int MAX_MANAGE_INPUT = 15;
	private static final String STATIC_ID_RESIZE = "010__RESIZE";
	private static final String STATIC_ID_REDRAW_ALL = "0100__REDRAW_ALL";
	private static GaleContext globalContext = null;
	// return true if a flush is needed
	private static int countMemeCheck = 0;

	/**
	 * From everyware in the program, we can get the context inteface.
	 * @return current reference on the instance.
	 * @note For test create a ``` new GaleContextTest()``` ... this permit to run some test...
	 */
	public static GaleContext getContext() {
		return GaleContext.globalContext;
	}

	public static void setContext(final GaleContext context) {
		GaleContext.globalContext = context;
	}

	protected ThreadAbstract periodicThread;
	protected GaleApplication application; //!< Application handle
	protected ApplicationState applicationState = ApplicationState.UNDEFINED; // state of the application
	private final CommandLine commandLine = new CommandLine(); //!< Start command line information;
	private final ResourceManager resourceManager = new ResourceManager(); //!< global resources Manager
	// simulation area:
	private long previousDisplayTime; // this is to limit framerate ... in case...
	private final boolean displayFps = true;

	private final Lock msgSystemAsyncLock = new ReentrantLock();
	private final MessageSystem msgSystemAsync = new MessageSystem();
	private final MessageSystem msgSystemGui = new MessageSystem();
	private final Fps fpsSystemEvent = new Fps("SystemEvent", this.displayFps);
	private final Fps fpsSystemContext = new Fps("SystemContext", this.displayFps);
	private final Fps fpsSystem = new Fps("System", this.displayFps);
	private final Fps fpsFlush = new Fps("Flush", this.displayFps);
	protected Vector2f windowsSize = Vector2f.ZERO; //!< current size of the system
	protected boolean fullscreen = false;
	protected Vector2f windowsPos; //!< current size of the system
	// note: in the current mode, the management is not able to synchronize it good..
	private final boolean requestSynchronousProcessing = true; //!< this permit to process the event in the global GUI thread instead of the local processing thread.

	public GaleContext(final GaleApplication application, final String[] args) {
		// set a basic
		this.application = application;
		this.applicationState = ApplicationState.CREATE;
		GaleContext.setContext(this);
		Thread.currentThread().setName("galeThread");
		if (this.application == null) {
			LOGGER.error("Can not start context with no Application ==> rtfm ...");
			throw new RuntimeException("Can not start context with no Application ==> rtfm ...");
		}
		this.commandLine.parse(args);
		LOGGER.info(" == > Gale system init (BEGIN)");
		// create thread to manage real periodic event
		this.periodicThread = new PeriodicThread(this);

		// By default we set 2 themes (1 color and 1 shape ...) :
		//theme::setNameDefault("GUI", "shape/square/");
		//theme::setNameDefault("COLOR", "color/black/");

		// parse the debug level:
		//		for(int iii=0; iii<this.commandLine.size(); ++iii) {
		//			if (this.commandLine.get(iii) == "--gale-fps") {
		//				this.displayFps=true;
		//			} else if (    this.commandLine.get(iii) == "-h"
		//			            || this.commandLine.get(iii) == "--help"
		//			            || startwith(this.commandLine.get(iii), "--gale")) {
		//				LOGGER.print("gale - help : ");
		//				LOGGER.print("        --gale-fps");
		//				LOGGER.print("                Display the current fps of the display");
		//				LOGGER.print("        -h/--help");
		//				LOGGER.print("                Display this help");
		//				if (startwith(this.commandLine.get(iii), "--gale")) {
		//					LOGGER.error("gale unknow element in parameter: '" << this.commandLine.get(iii) << "'");
		//					// remove parameter ...
		//				} else {
		//					// this is a global help system does not remove it
		//					continue;
		//				}
		//			} else {
		//				continue;
		//			}
		//			this.commandLine.remove(iii);
		//			--iii;
		//		}
		//cout.setOutputFile(true);

		LOGGER.info("GALE v: {}", Gale.getVersion());
		forceOrientation(Orientation.screenAuto);
		postActionAsync(context -> {
			final GaleApplication appl = context.getApplication();
			if (appl == null) {
				this.applicationState = ApplicationState.UNDEFINED;
				return;
			}
			appl.onCreate(context);
			appl.onStart(context);
			appl.onResume(context);
			this.applicationState = ApplicationState.RUNNING;
			// this is done at the end to perform a full ended rendering.
			context.requestUpdateSize();
		});

		// force a recalculation
		//requestUpdateSize();
		LOGGER.info(" == > Gale system init (END)");
	}

	/**
	 * Inform the Gui that we want to have a copy of the clipboard
	 * @param clipboardID ID of the clipboard (STD/SELECTION) only apear here
	 */
	public void clipBoardGet(final ClipboardList clipboardID) {
		// just transmit an event , we have the data in the system
		operatingSystemClipBoardArrive(clipboardID);
	}

	/**
	 * Inform the Gui that we are the new owner of the clipboard
	 * @param clipboardID ID of the clipboard (STD/SELECTION) only apear here
	 */
	public void clipBoardSet(final ClipboardList clipboardID) {
		// nothing to do, data is already copyed in the GALE clipborad center
	}

	/**
	 * force the screen orientation (availlable on portable elements ...
	 * @param orientation Selected orientation.
	 */
	public final void forceOrientation(final Orientation orientation) {
		postActionToGui(context -> {
			context.forceOrientationThreadGUI(orientation);
		});
	}

	protected void forceOrientationThreadGUI(final Orientation orientation) {
		LOGGER.info("TODO: forceOrientation: not implemented");
	}

	/**
	 * Redraw all the windows
	 */
	public void forceRedrawAll() {
		if (this.application == null) {
			return;
		}
		if (this.windowsSize.equals(Vector2f.ZERO)) {
			return;
		}
		this.application.onResize(this.windowsSize);
	}

	// Called by Consumer
	public ActionToDoInAsyncLoop getAction() {
		return this.msgSystemAsync.getElementWait();
	}

	public GaleApplication getApplication() {
		return this.application;
		/*
		 * pb on this lock ...
		this.msgSystemAsyncLock.lock();
		try {
			return this.application;
		} finally {
			this.msgSystemAsyncLock.unlock();
		}
		*/
	}

	public CommandLine getCmd() {
		return this.commandLine;
	}

	public boolean getFullScreen() {
		return this.fullscreen;
	}

	/**
	 * The Application request the current position of the windows.
	 * @return Turrent position of the Windows.
	 */
	public Vector2f getPos() {
		return this.windowsPos;
	}

	public ResourceManager getResourcesManager() {
		return this.resourceManager;
	}

	/**
	 * get the current windows size
	 * @return the current size ...
	 */
	public Vector2f getSize() {
		return this.windowsSize;
	}

	/**
	 * get all Keyboard event from the X system (like many time use of META)
	 * @param status "true" if all the event will be get, false if we want only ours.
	 */
	public final void grabKeyboardEvents(final boolean status) {
		postActionToGui(context -> {
			context.grabKeyboardEventsThreadGUI(status);
		});
	}

	protected void grabKeyboardEventsThreadGUI(final boolean status) {
		LOGGER.info("grabKeyboardEvents: NOT implemented ...");
	}

	/**
	 * get all Mouse/Touch events from the X system
	 * @param status "true" if all the event will be get, false if we want only ours.
	 * @param forcedPosition the position where the mouse might be reset at  every events ...
	 */
	public final void grabPointerEvents(final boolean status, final Vector2f forcedPosition) {
		postActionToGui(context -> {
			context.grabPointerEventsThreadGUI(status, forcedPosition);
		});
	}

	protected void grabPointerEventsThreadGUI(final boolean status, final Vector2f forcedPosition) {
		LOGGER.info("grabPointerEvents: NOT implemented ...");

	}

	/**
	 * The Application request that the Windows will be Hidden.
	 */
	public final void hide() {
		postActionToGui(context -> {
			context.hideThreadGUI();
		});
	}

	protected void hideThreadGUI() {
		LOGGER.info("hide: NOT implemented ...");
	}

	public boolean isGrabPointerEvents() {
		return false;
	}

	/**
	 * Hide the virtual keyboard (for touch system only)
	 */
	public final void keyboardHide() {
		postActionToGui(context -> {
			context.keyboardHideThreadGUI();
		});
	}

	protected void keyboardHideThreadGUI() {
		LOGGER.info("keyboardHide: NOT implemented ...");
	}

	/**
	 * display the virtual keyboard (for touch system only)
	 */
	public final void keyboardShow() {
		postActionToGui(context -> {
			context.keyboardShowThreadGUI();
		});
	}

	protected void keyboardShowThreadGUI() {
		LOGGER.info("keyboardShow: NOT implemented ...");
	}

	/**
	 * Open an URL on an eternal brother.
	 * @param url URL to open.
	 */
	public final void openURL(final String url) {
		postActionToGui(context -> {
			context.openURLThreadGUI(url);
		});
	}

	protected void openURLThreadGUI(final String url) {
		LOGGER.info("openURL: NOT implemented ...");
	}

	/**
	 * The current context is set in background (framerate is slowing down (max fps)/5 # 4fps)
	 */
	public void operatingSystemBackground() {
		// set the current interface :
		try (AutoUnLock autoUnlock = AutoUnLock.lock(this.msgSystemAsyncLock)) {
			LOGGER.info("operatingSystemBackground...");
			//		if (this.windowsCurrent != null) {
			//			this.windowsCurrent.onStateBackground();
			//		}
			// release the current interface :
		}
	}

	/**
	 * Call by the OS when a clipboard arrive to US (previously requested by a widget)
	 * @param clipboardID of the clipboard
	 */
	public void operatingSystemClipBoardArrive(final ClipboardList clipboardID) {
		postActionAsync(context -> {
			final GaleApplication appl = context.getApplication();
			if (appl != null) {
				appl.onClipboardEvent(clipboardID);
			}
		});
	}

	public boolean operatingSystemDraw(final boolean displayEveryTime) {
		if (GaleContext.countMemeCheck++ >= 10 * 16) {
			GaleContext.countMemeCheck = 0;
		}
		LOGGER.trace("Call draw");
		final long currentTime = System.nanoTime();
		//LOGGER.warn("Time = " << currentTime << "         " << currentTime2);
		// TODO Review this ...
		// this is to prevent the multiple display at the a high frequency ...
		if (currentTime - this.previousDisplayTime < 8) {
			try {
				Thread.sleep(1);
			} catch (final InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			return false;
		}
		this.previousDisplayTime = currentTime;
		OpenGL.threadHasContext();
		OpenGL.resetFlagState();
		// process the events
		if (this.displayFps) {
			this.fpsSystemEvent.tic();
		}
		boolean needRedraw = false;
		//! Event management section ...
		{
			// set the current interface:
			try (AutoUnLock autoUnlock = AutoUnLock.lock(this.msgSystemAsyncLock)) {
				/*
				Lock the event processing

				Wait end of current processing

				Display ...

				Release the event processing

				*/
				if (this.application != null) {
					if (this.applicationState == ApplicationState.RUNNING) {
						// Redraw all needed elements
						//LOGGER.debug("Regenerate Display");
						this.application.onRegenerateDisplay(this);
						needRedraw = this.application.isDrawingNeeded();
					} else {
						needRedraw = true;
					}
				}
				if (this.displayFps) {
					this.fpsSystemEvent.incrementCounter();
					this.fpsSystemEvent.toc();
				}
			}
		}
		boolean hasDisplayDone = false;
		//! drawing section:
		{
			// Lock openGl context:
			OpenGL.lock();
			if (this.displayFps) {
				this.fpsSystemContext.tic();
			}
			if (needRedraw || displayEveryTime) {
				//LOGGER.debug("  ==> real Draw");
				try (AutoUnLock autoUnlock = AutoUnLock.lock(this.msgSystemAsyncLock)) {
					this.resourceManager.updateContext();
				}
				if (this.displayFps) {
					this.fpsSystemContext.incrementCounter();
				}
			}
			if (this.displayFps) {
				this.fpsSystemContext.toc();
				this.fpsSystem.tic();
			}
			if (this.application != null) {
				if (needRedraw || displayEveryTime) {
					this.fpsSystem.incrementCounter();
					// set the current interface :
					try (AutoUnLock autoUnlock = AutoUnLock.lock(this.msgSystemAsyncLock)) {
						if (this.applicationState == ApplicationState.RUNNING) {
							this.application.onDraw(this);
						} else {
							OpenGL.setViewPort(new Vector2f(0, 0), this.application.getSize());
							final Color bgColor = new Color(0.8f, 0.5f, 0.8f, 1.0f);
							OpenGL.clearColor(bgColor);
							//LOGGER.info("==> appl clear ==> not created ...");
						}
					}
					hasDisplayDone = true;
				}
			}
			if (this.displayFps) {
				this.fpsSystem.toc();
				this.fpsFlush.tic();
			}
			if (hasDisplayDone) {
				//LOGGER.info("lklklklklk " << displayEveryTime);
				if (this.displayFps) {
					this.fpsFlush.incrementCounter();
				}
				OpenGL.flush();
			}
			if (this.displayFps) {
				this.fpsFlush.toc();
			}
			// release open GL Context
			OpenGL.unLock();
		}
		if (this.displayFps) {
			this.fpsSystemEvent.draw();
			this.fpsSystemContext.draw();
			this.fpsSystem.draw();
			this.fpsFlush.draw();
		}
		{
			// set the current interface:
			try (AutoUnLock autoUnlock = AutoUnLock.lock(this.msgSystemAsyncLock)) {
				// release open GL Context
				OpenGL.lock();
				// while The Gui is drawing in OpenGl, we do some not realTime things
				this.resourceManager.updateContext();
				// release open GL Context
				OpenGL.unLock();
				// TODO this.objectManager.cleanInternalRemoved();
				this.resourceManager.cleanInternalRemoved();
			}
		}
		OpenGL.threadHasNoMoreContext();
		return hasDisplayDone;
	}

	/**
	 * The current context is set in foreground (framerate is maximum speed)
	 */
	public void operatingSystemForeground() {
		// set the current interface :
		try (AutoUnLock autoUnlock = AutoUnLock.lock(this.msgSystemAsyncLock)) {
			LOGGER.info("operatingSystemForeground...");

			//		if (this.windowsCurrent != null) {
			//			this.windowsCurrent.onStateForeground();
			//		}
		}
	}

	/**
	 * The OS inform that the Windows is now Hidden.
	 */
	public void operatingSystemHide() {
		postActionAsync(context -> {
			/*
			Application> appl = context.getApplication();
			if (appl == null) {
				return;
			}
			appl.onKeyboard(special,
			                 type,
			                 char,
			                 state);
			*/
			LOGGER.info("TODO: HIDE ... ");
		});
	}

	/**
	 * The OS inform that the current windows has change his position.
	 * @param pos New position of the Windows.
	 */
	public void operatingSystemMove(final Vector2f pos) {
		if (this.windowsPos.isEqual(pos)) {
			return;
		}
		postActionAsync(context -> {
			LOGGER.debug("Receive MSG : THREADMOVE : {} ==> {}", context.windowsPos, pos);
			context.windowsPos = pos;
			final GaleApplication appl = context.getApplication();
			if (appl == null) {
				return;
			}
			appl.onMovePosition(context.windowsPos);
		});
	}

	/**
	 * The OS inform that the openGL ext has been destroy  == > use to automaticly reload the texture and other thinks ...
	 */
	public void operatingSystemOpenGlContextDestroy() {
		this.resourceManager.contextHasBeenDestroyed();
	}

	/**
	 * The OS inform that the current windows has change his size.
	 * @param size new size of the windows.
	 */
	public void operatingSystemResize(final Vector2f size) {
		LOGGER.warn("Resize request={} previous={}", size, this.windowsSize);
		if (this.windowsSize.equals(size)) {
			return;
		}
		postActionAsync(GaleContext.STATIC_ID_RESIZE, context -> {
			LOGGER.debug("Receive MSG : THREAD_RESIZE : {} ==> {}", context.windowsSize, size);
			context.windowsSize = size;
			Dimension2f.setPixelWindowsSize(context.windowsSize);
			final GaleApplication tmpAppl = context.getApplication();
			if (tmpAppl != null) {
				tmpAppl.onResize(context.windowsSize);
			}
			// call application inside ..
			context.forceRedrawAll();
		});
	}

	/**
	 * The current context is resumed
	 */
	public void operatingSystemResume() {
		// set the current interface :
		try (AutoUnLock autoUnlock = AutoUnLock.lock(this.msgSystemAsyncLock)) {
			LOGGER.info("operatingSystemResume...");
			this.previousDisplayTime = System.currentTimeMillis();
			// TODO this.objectManager.timeCallResume(this.previousDisplayTime);
			//		if (this.windowsCurrent != null) {
			//			this.windowsCurrent.onStateResume();
			//		}
		}
	}

	public void operatingSystemSetInput(
			final KeySpecial special,
			final KeyType type,
			final KeyStatus status,
			final int pointerID,
			final Vector2f pos) {
		LOGGER.trace("Position motion: {}", pos);
		postActionAsync(context -> {
			final GaleApplication appl = context.getApplication();
			if (appl == null) {
				return;
			}
			appl.onPointer(special, type, pointerID, pos, status);
		});
	}

	public void operatingSystemsetKeyboard(
			final KeySpecial special,
			final KeyKeyboard type,
			final KeyStatus state,
			final boolean isARepeateKey) {
		operatingSystemsetKeyboard(special, type, state, isARepeateKey, (char) 0);
	}

	public void operatingSystemsetKeyboard(
			final KeySpecial special,
			final KeyKeyboard type,
			final KeyStatus state,
			final boolean isARepeateKey,
			final Character charValue) {
		KeyStatus tmpState = state;
		if (isARepeateKey) {
			if (tmpState == KeyStatus.down) {
				tmpState = KeyStatus.downRepeat;
			} else {
				tmpState = KeyStatus.upRepeat;
			}
		}
		operatingSystemsetKeyboard2(special, type, state, charValue);
	}

	public void operatingSystemsetKeyboard2(
			final KeySpecial special,
			final KeyKeyboard type,
			final KeyStatus state,
			final Character charValue) {
		postActionAsync(context -> {
			final GaleApplication appl = context.getApplication();
			if (appl == null) {
				return;
			}
			appl.onKeyboard(special, type, charValue, state);
		});
	}

	/**
	 * The OS inform that the Windows is now visible.
	 */
	public void operatingSystemShow() {
		postActionAsync(context -> {
			/*
			Application> appl = context.getApplication();
			if (appl == null) {
				return;
			}
			appl.onKeyboard(special,
			                 type,
			                 char,
			                 state);
			*/
			LOGGER.info("TODO: SHOW ... ");
		});
	}

	/**
	 * The OS Inform that the Window has been killed
	 */
	public void operatingSystemStop() {
		// set the current interface :
		try (AutoUnLock autoUnlock = AutoUnLock.lock(this.msgSystemAsyncLock)) {
			LOGGER.info("operatingSystemStop...");
			if (this.application == null) {
				stop();
				return;
			}
			this.application.onKillDemand(this);
		}
	}

	/**
	 * The current context is suspended
	 */
	public void operatingSystemSuspend() {
		// set the current interface :
		try (AutoUnLock autoUnlock = AutoUnLock.lock(this.msgSystemAsyncLock)) {
			LOGGER.info("operatingSystemSuspend...");
			this.previousDisplayTime = 0;

			//		if (this.windowsCurrent != null) {
			//			this.windowsCurrent.onStateSuspend();
			//		}
		}
	}

	protected void postActionAsync(final ActionToDoInAsyncLoop data) {
		if (this.requestSynchronousProcessing) {
			this.msgSystemGui.addElement(data);
		} else {
			this.msgSystemAsync.addElement(data);
		}
		//Later, when the necessary event happens, the thread that is running it calls notify() from a block synchronized on the same object.
	}

	protected void postActionAsync(final String uniqueID, final ActionToDoInAsyncLoop data) {
		if (this.requestSynchronousProcessing) {
			this.msgSystemGui.addElement(uniqueID, data);
		} else {
			this.msgSystemAsync.addElement(uniqueID, data);
		}
		//Later, when the necessary event happens, the thread that is running it calls notify() from a block synchronized on the same object.
	}

	protected void postActionToGui(final ActionToDoInAsyncLoop data) {
		this.msgSystemGui.addElement(data);
		//Later, when the necessary event happens, the thread that is running it calls notify() from a block synchronized on the same object.
	}

	/**
	 * Processing all the event arrived ... (commonly called in draw function)
	 */
	public void processEventsAsync(final Clock clock, final long time) {
		if (!this.msgSystemAsyncLock.tryLock()) {
			return;
		}
		try {
			int nbEvent = 0;
			while (this.msgSystemAsync.getSize() > 0) {
				LOGGER.trace("    [{}] event ...", nbEvent);
				nbEvent++;
				final ActionToDoInAsyncLoop func = this.msgSystemAsync.getElementWait();
				if (func == null) {
					continue;
				}
				func.run(this);
			}
		} catch (final Exception e) {
			LOGGER.error("Catch exception in main event Loop ...", e);
			throw e;
		} finally {
			this.msgSystemAsyncLock.unlock();
		}
	}

	/**
	 * Process event in the GUI thread ==> prevent dead lock on the global GUI interface
	 */
	protected void processEventsGui() {
		if (!this.msgSystemAsyncLock.tryLock()) {
			return;
		}
		try {
			while (this.msgSystemGui.getSize() > 0) {
				final ActionToDoInAsyncLoop func = this.msgSystemGui.getElementWait();
				if (func == null) {
					continue;
				}
				func.run(this);
			}
		} catch (final Exception e) {
			LOGGER.error("Catch exception in main event Loop ...", e);
			throw e;
		} finally {
			this.msgSystemAsyncLock.unlock();
		}
	}

	//	gale::Context::~Context() {
	//		LOGGER.info(" == > Gale system Un-Init (BEGIN)");
	//		this.periodicThread.threadStart();
	//		getResourcesManager().applicationExiting();
	//		// TODO Clean the message list ...
	//		// set the current interface:
	//		lockContext();
	//		// clean all widget and sub widget with their resources:
	//		//this.objectManager.cleanInternalRemoved();
	//		// call application to uninit
	//		this.application.canDraw = false;
	//		this.application.onPause(*this);
	//		this.application.onStop(*this);
	//		this.application.onDestroy(*this);
	//		this.application.reset();
	//		// clean all messages
	//		this.msgSystem.clean();
	//		// internal clean elements
	//		//this.objectManager.cleanInternalRemoved();
	//		this.resourceManager.cleanInternalRemoved();
	//
	//		LOGGER.info("List of all widget of this context must be equal at 0 ==> otherwise some remove is missing");
	//		//this.objectManager.displayListObject();
	//		// Resource is an lower element as objects ...
	//		this.resourceManager.unInit();
	//		// now All must be removed !!!
	//		//this.objectManager.unInit();
	//		// release the current interface :
	//		unLockContext();
	//		LOGGER.info(" == > Gale system Un-Init (END)");
	//		if (this.simulationActive) {
	//			// in simulation case:
	//			this.simulationFile.close();
	//		}
	//	}
	public void requestUpdateSize() {
		postActionAsync(this.STATIC_ID_REDRAW_ALL, context -> {
			//LOGGER.debug("Receive MSG : THREADRESIZE");
			context.forceRedrawAll();
		});
	}

	/**
	 * reset event management for the IO like Input ou Mouse or keyborad
	 */
	public void resetIOEvent() {
		// TODO this.input.newLayerSet();
	}

	/**
	 * Internal API to run the processing of the event loop ...
	 * @return The Exit value of the program
	 * @note INTERNAL API
	 */
	public abstract int run();

	/**
	 * set the cursor display type.
	 * @param newCursor selected new cursor.
	 */
	public final void setCursor(final Cursor newCursor) {
		postActionToGui(context -> {
			context.setCursorThreadGUI(newCursor);
		});
	}

	public void setCursorThreadGUI(final Cursor newCursor) {
		LOGGER.info("setCursor: NOT implemented ...");
	}

	/**
	 * The application request a change of his current size force the fullscreen mode.
	 * @param status status of the fullscreen mode.
	 */
	public final void setFullScreen(final boolean status) {
		postActionToGui(context -> {
			this.fullscreen = status;
			context.setFullScreenThreadGUI(status);
		});
	}

	protected void setFullScreenThreadGUI(final boolean status) {
		LOGGER.info("setFullScreen: NOT implemented ...");
	}

	/**
	 * set the Icon of the program
	 * @param inputFile new filename icon of the current program.
	 */
	public final void setIcon(final Uri inputFile) {
		postActionToGui(context -> {
			context.setIconThreadGUI(inputFile);
		});
	}

	public void setIconThreadGUI(final Uri inputFile) {
		LOGGER.info("setIcon: NOT implemented ...");
	}

	/**
	 * The Application request that the current windows will change his position.
	 * @param pos New position of the Windows requested.
	 */
	public final void setPos(final Vector2f pos) {
		postActionToGui(context -> {
			context.setPosThreadGUI(pos);
		});
	}

	protected void setPosThreadGUI(final Vector2f pos) {
		LOGGER.info("setPos: NOT implemented ...");
	}

	/**
	 * The application request a change of his current size.
	 * @param size new Requested size of the windows.
	 */
	public final void setSize(final Vector2f size) {
		postActionToGui(context -> {
			context.setSizeThreadGUI(size);
		});
	}

	protected void setSizeThreadGUI(final Vector2f size) {
		LOGGER.info("setSize: NOT implemented ...");
	}

	/**
	 * set the new title of the windows
	 * @param title New desired title
	 */
	public final void setTitle(final String title) {
		postActionToGui(context -> {
			context.setTitleThreadGUI(title);
		});
	}

	/**
	 * set the new title of the windows
	 * @param title New desired title
	 */
	protected void setTitleThreadGUI(final String title) {
		LOGGER.info("setTitle: NOT implemented ...");
	}

	/**
	 * Enable or Disable the decoration on the Windows (availlable only on Desktop)
	 * @param status "true" to enable decoration / false otherwise
	 */
	public final void setWindowsDecoration(final boolean status) {
		postActionToGui(context -> {
			context.setWindowsDecorationThreadGUI(status);
		});
	}

	protected void setWindowsDecorationThreadGUI(final boolean status) {
		LOGGER.info("setWindowsDecoration: NOT implemented ...");
	}

	/**
	 * The Application request that the Windows will be visible.
	 */
	public void show() {
		postActionToGui(context -> {
			context.showThreadGUI();
		});
	}

	public void showThreadGUI() {
		LOGGER.info("show: NOT implemented ...");
	}

	/**
	 * StartProcessing (2nd thread).
	 * @note to call when all the Context is started
	 */
	public void start2ndThreadProcessing() {
		// set the current interface:
		try (AutoUnLock autoUnlock = AutoUnLock.lock(this.msgSystemAsyncLock)) {
			this.periodicThread.threadStart();
			try {
				Thread.sleep(1);
			} catch (final InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
	}

	/**
	 * The application request that the Window will be killed
	 */
	public void stop() {
		LOGGER.warn("stop: NOT implemented for this platform...");
	}

}
