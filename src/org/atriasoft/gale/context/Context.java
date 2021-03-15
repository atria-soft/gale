package org.atriasoft.gale.context;

import java.util.Vector;

import org.atriasoft.etk.ThreadAbstract;
import org.atriasoft.etk.Uri;
import org.atriasoft.etk.math.Vector2f;
import org.atriasoft.gale.Application;
import org.atriasoft.gale.Fps;
import org.atriasoft.gale.Gale;
import org.atriasoft.gale.Orientation;
import org.atriasoft.gale.backend3d.OpenGL;
import org.atriasoft.gale.internal.Log;
import org.atriasoft.gale.key.KeyKeyboard;
import org.atriasoft.gale.key.KeySpecial;
import org.atriasoft.gale.key.KeyStatus;
import org.atriasoft.gale.key.KeyType;
import org.atriasoft.gale.resource.ResourceManager;

interface ActionToDoInAsyncLoop {
	public void run(Context context);
}

public abstract class Context {
	protected static final int MAX_MANAGE_INPUT = 15;
	private static Context globalContext = null;
	// return true if a flush is needed
	private static int countMemeCheck = 0;
	
	/**
	 * @brief From everyware in the program, we can get the context inteface.
	 * @return current reference on the instance.
	 */
	public static Context getContext() {
		return globalContext;
	}
	
	public static void setContext(final Context context) {
		globalContext = context;
	}
	
	protected ThreadAbstract periodicThread;;
	protected Application application; //!< Application handle
	private final CommandLine commandLine = new CommandLine(); //!< Start command line information;
	private final ResourceManager resourceManager = new ResourceManager(); //!< global resources Manager
	// simulation area:
	private long previousDisplayTime; // this is to limit framerate ... in case...
	private final Vector<ActionToDoInAsyncLoop> msgSystem = new Vector<>();
	private final boolean displayFps = true;
	private final Fps fpsSystemEvent = new Fps("SystemEvent", this.displayFps);
	private final Fps fpsSystemContext = new Fps("SystemContext", this.displayFps);
	private final Fps fpsSystem = new Fps("System", this.displayFps);
	private final Fps fpsFlush = new Fps("Flush", this.displayFps);
	protected Vector2f windowsSize = new Vector2f(0, 0); //!< current size of the system
	protected boolean fullscreen = false;
	protected Vector2f windowsPos; //!< current size of the system
	
	public Context(final Application application, final String[] args) {
		// set a basic
		this.application = application;
		setContext(this);
		Thread.currentThread().setName("galeThread");
		if (this.application == null) {
			Log.critical("Can not start context with no Application ==> rtfm ...");
		}
		this.commandLine.parse(args);
		Log.info(" == > Gale system init (BEGIN)");
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
		//			            || start_with(this.commandLine.get(iii), "--gale")) {
		//				Log.print("gale - help : ");
		//				Log.print("        --gale-fps");
		//				Log.print("                Display the current fps of the display");
		//				Log.print("        -h/--help");
		//				Log.print("                Display this help");
		//				if (start_with(this.commandLine.get(iii), "--gale")) {
		//					Log.error("gale unknow element in parameter: '" << this.commandLine.get(iii) << "'");
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
		
		Log.info("GALE v:" + Gale.getVersion());
		forceOrientation(Orientation.screenAuto);
		postAction((_context) -> {
			final Application appl = _context.getApplication();
			if (appl == null) {
				return;
			}
			appl.onCreate(_context);
			appl.onStart(_context);
			appl.onResume(_context);
			appl.canDraw = true;
		});
		
		// force a recalculation
		requestUpdateSize();
		Log.info(" == > Gale system init (END)");
	}
	
	/**
	 * @brief Inform the Gui that we want to have a copy of the clipboard
	 * @param _clipboardID ID of the clipboard (STD/SELECTION) only apear here
	 */
	public void clipBoardGet(final ClipboardList clipboardID) {
		// just transmit an event , we have the data in the system
		operatingSystemClipBoardArrive(clipboardID);
	}
	
	/**
	 * @brief Inform the Gui that we are the new owner of the clipboard
	 * @param _clipboardID ID of the clipboard (STD/SELECTION) only apear here
	 */
	public void clipBoardSet(final ClipboardList clipboardID) {
		// nothing to do, data is already copyed in the GALE clipborad center
	}
	
	/**
	 * @brief force the screen orientation (availlable on portable elements ...
	 * @param _orientation Selected orientation.
	 */
	public void forceOrientation(final Orientation orientation) {}
	
	/**
	 * @brief Redraw all the windows
	 */
	public void forceRedrawAll() {
		if (this.application == null) {
			return;
		}
		this.application.onResize(this.windowsSize);
	}
	
	// Called by Consumer
	public synchronized ActionToDoInAsyncLoop getAction() {
		notify();
		while (this.msgSystem.size() == 0) {
			try {
				wait();
			} catch (final InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
				return null;
			} //By executing wait() from a synchronized block, a thread gives up its hold on the lock and goes to sleep.
		}
		final ActionToDoInAsyncLoop message = this.msgSystem.firstElement();
		this.msgSystem.removeElement(message);
		return message;
	}
	
	public Application getApplication() {
		return this.application;
	}
	
	public CommandLine getCmd() {
		return this.commandLine;
	}
	
	public boolean getFullScreen() {
		return this.fullscreen;
	}
	
	/**
	 * @brief The Application request the current position of the windows.
	 * @return Turrent position of the Windows.
	 */
	public Vector2f getPos() {
		return this.windowsPos;
	}
	
	public ResourceManager getResourcesManager() {
		return this.resourceManager;
	}
	
	/**
	 * @brief get the current windows size
	 * @return the current size ...
	 */
	public Vector2f getSize() {
		return this.windowsSize;
	}
	
	/**
	 * @brief get all Keyboard event from the X system (like many time use of META)
	 * @param _status "true" if all the event will be get, false if we want only ours.
	 */
	public void grabKeyboardEvents(final boolean status) {}
	
	/**
	 * @brief get all Mouse/Touch events from the X system
	 * @param _status "true" if all the event will be get, false if we want only ours.
	 * @param _forcedPosition the position where the mouse might be reset at  every events ...
	 */
	public void grabPointerEvents(final boolean status, final Vector2f forcedPosition) {}
	
	/**
	 * @brief The Application request that the Windows will be Hidden.
	 */
	public void hide() {
		Log.info("hide: NOT implemented ...");
	}
	
	public boolean isGrabPointerEvents() {
		return false;
	}
	
	/**
	 * @brief Hide the virtal keyboard (for touch system only)
	 */
	public void keyboardHide() {
		Log.info("keyboardHide: NOT implemented ...");
	}
	
	/**
	 * @brief display the virtal keyboard (for touch system only)
	 */
	public void keyboardShow() {
		Log.info("keyboardShow: NOT implemented ...");
	}
	
	protected void lockContext() {
		
	}
	
	/**
	 * @brief Open an URL on an eternal brother.
	 * @param _url URL to open.
	 */
	public void openURL(final String url) {}
	
	/**
	 * @brief The current context is set in background (framerate is slowing down (max fps)/5 # 4fps)
	 */
	public void operatingSystemBackground() {
		// set the current interface :
		lockContext();
		Log.info("operatingSystemBackground...");
		//		if (this.windowsCurrent != null) {
		//			this.windowsCurrent.onStateBackground();
		//		}
		// release the current interface :
		unLockContext();
	}
	
	/**
	 * @brief Call by the OS when a clipboard arrive to US (previously requested by a widget)
	 * @param Id of the clipboard
	 */
	public void operatingSystemClipBoardArrive(final ClipboardList clipboardID) {
		postAction((context) -> {
			final Application appl = context.getApplication();
			if (appl != null) {
				appl.onClipboardEvent(clipboardID);
			}
		});
	}
	
	public boolean operatingSystemDraw(final boolean displayEveryTime) {
		if (countMemeCheck++ >= 10 * 16) {
			countMemeCheck = 0;
		}
		//Log.verbose("Call draw");
		final long currentTime = System.currentTimeMillis();
		//echrono::Time currentTime2 = echrono::Time::now();
		//Log.warning("Time = " << currentTime << "         " << currentTime2);
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
		if (this.displayFps == true) {
			this.fpsSystemEvent.tic();
		}
		boolean needRedraw = false;
		//! Event management section ...
		{
			// set the current interface:
			lockContext();
			/*
			Lock the event processing
			
			Wait end of current processing
			
			Display ...
			
			Release the event processing
			
			*/
			if (this.application != null) {
				// Redraw all needed elements
				//Log.debug("Regenerate Display");
				this.application.onRegenerateDisplay(this);
				needRedraw = this.application.isDrawingNeeded();
			}
			if (this.displayFps) {
				this.fpsSystemEvent.incrementCounter();
				this.fpsSystemEvent.toc();
			}
			// release the current interface :
			unLockContext();
		}
		boolean hasDisplayDone = false;
		//! drawing section:
		{
			// Lock openGl context:
			OpenGL.lock();
			if (this.displayFps == true) {
				this.fpsSystemContext.tic();
			}
			if (needRedraw = true || displayEveryTime == true) {
				//Log.debug("  ==> real Draw");
				lockContext();
				this.resourceManager.updateContext();
				unLockContext();
				if (this.displayFps == true) {
					this.fpsSystemContext.incrementCounter();
				}
			}
			if (this.displayFps == true) {
				this.fpsSystemContext.toc();
				this.fpsSystem.tic();
			}
			if (this.application != null) {
				if (needRedraw == true || displayEveryTime == true) {
					this.fpsSystem.incrementCounter();
					// set the current interface :
					lockContext();
					if (this.application.canDraw == true) {
						this.application.onDraw(this);
					}
					unLockContext();
					hasDisplayDone = true;
				}
			}
			if (this.displayFps == true) {
				this.fpsSystem.toc();
				this.fpsFlush.tic();
			}
			if (hasDisplayDone == true) {
				//Log.info("lklklklklk " << _displayEveryTime);
				if (this.displayFps == true) {
					this.fpsFlush.incrementCounter();
				}
				OpenGL.flush();
			}
			if (this.displayFps == true) {
				this.fpsFlush.toc();
			}
			// release open GL Context
			OpenGL.unLock();
		}
		if (this.displayFps == true) {
			this.fpsSystemEvent.draw();
			this.fpsSystemContext.draw();
			this.fpsSystem.draw();
			this.fpsFlush.draw();
		}
		{
			// set the current interface:
			lockContext();
			// release open GL Context
			OpenGL.lock();
			// while The Gui is drawing in OpenGl, we do some not realTime things
			this.resourceManager.updateContext();
			// release open GL Context
			OpenGL.unLock();
			// TODO this.objectManager.cleanInternalRemoved();
			this.resourceManager.cleanInternalRemoved();
			// release the current interface:
			unLockContext();
		}
		OpenGL.threadHasNoMoreContext();
		return hasDisplayDone;
	};
	
	/**
	 * @brief The current context is set in foreground (framerate is maximum speed)
	 */
	public void operatingSystemForeground() {
		// set the current interface :
		lockContext();
		Log.info("operatingSystemForeground...");
		
		//		if (this.windowsCurrent != null) {
		//			this.windowsCurrent.onStateForeground();
		//		}
		// release the current interface :
		unLockContext();
	}
	
	/**
	 * @brief The OS inform that the Windows is now Hidden.
	 */
	public void operatingSystemHide() {
		postAction((context) -> {
			/*
			Application> appl = _context.getApplication();
			if (appl == null) {
				return;
			}
			appl.onKeyboard(_special,
			                 _type,
			                 _char,
			                 _state);
			*/
			Log.todo("HIDE ... ");
		});
	};
	
	/**
	 * @brief The OS inform that the current windows has change his position.
	 * @param _pos New position of the Windows.
	 */
	public void operatingSystemMove(final Vector2f _pos) {
		if (this.windowsPos.isEqual(_pos)) {
			return;
		}
		postAction((context) -> {
			Log.debug("Receive MSG : THREAD_MOVE : " + context.windowsPos + " ==> " + _pos);
			context.windowsPos = _pos;
			final Application appl = context.getApplication();
			if (appl == null) {
				return;
			}
			appl.onMovePosition(context.windowsPos);
		});
	}
	
	/**
	 * @brief The OS inform that the openGL ext has been destroy  == > use to automaticly reload the texture and other thinks ...
	 */
	public void operatingSystemOpenGlContextDestroy() {
		this.resourceManager.contextHasBeenDestroyed();
	};
	
	/**
	 * @brief The OS inform that the current windows has change his size.
	 * @param _size new size of the windows.
	 */
	public void operatingSystemResize(final Vector2f _size) {
		if (this.windowsSize == _size) {
			return;
		}
		// TODO Better in the thread ...  ==> but generate some init error ...
		//gale::Dimension::setPixelWindowsSize(_size);
		postAction((context) -> {
			Log.debug("Receive MSG : THREAD_RESIZE : " + context.windowsSize + " ==> " + _size);
			context.windowsSize = _size;
			//gale::Dimension::setPixelWindowsSize(_context.windowsSize);
			final Application tmpAppl = context.getApplication();
			if (tmpAppl != null) {
				tmpAppl.onResize(context.windowsSize);
			}
			// call application inside ..
			context.forceRedrawAll();
		});
	};
	
	/**
	 * @brief The current context is resumed
	 */
	public void operatingSystemResume() {
		// set the current interface :
		lockContext();
		Log.info("operatingSystemResume...");
		this.previousDisplayTime = System.currentTimeMillis();
		// TODO this.objectManager.timeCallResume(this.previousDisplayTime);
		//		if (this.windowsCurrent != null) {
		//			this.windowsCurrent.onStateResume();
		//		}
		// release the current interface :
		unLockContext();
	}
	
	public void operatingSystemSetInput(final KeySpecial special, final KeyType type, final KeyStatus status, final int pointerID, final Vector2f pos) {
		postAction((context) -> {
			final Application appl = context.getApplication();
			if (appl == null) {
				return;
			}
			appl.onPointer(special, type, pointerID, pos, status);
		});
	}
	
	public void operatingSystemsetKeyboard(final KeySpecial special, final KeyKeyboard type, final KeyStatus state, final boolean isARepeateKey) {
		operatingSystemsetKeyboard(special, type, state, isARepeateKey, (char) 0);
	}
	
	public void operatingSystemsetKeyboard(final KeySpecial special, final KeyKeyboard type, final KeyStatus state, final boolean isARepeateKey, final Character charValue) {
		KeyStatus tmpState = state;
		if (isARepeateKey == true) {
			if (tmpState == KeyStatus.down) {
				tmpState = KeyStatus.downRepeate;
			} else {
				tmpState = KeyStatus.upRepeate;
			}
		}
		operatingSystemsetKeyboard2(special, type, state, charValue);
	}
	
	public void operatingSystemsetKeyboard2(final KeySpecial special, final KeyKeyboard type, final KeyStatus state, final Character charValue) {
		postAction((context) -> {
			final Application appl = context.getApplication();
			if (appl == null) {
				return;
			}
			appl.onKeyboard(special, type, charValue, state);
		});
	}
	
	/**
	 * @brief The OS inform that the Windows is now visible.
	 */
	public void operatingSystemShow() {
		postAction((context) -> {
			/*
			Application> appl = _context.getApplication();
			if (appl == null) {
				return;
			}
			appl.onKeyboard(_special,
			                 _type,
			                 _char,
			                 _state);
			*/
			Log.todo("SHOW ... ");
		});
	};
	
	/**
	 * @brief The OS Inform that the Window has been killed
	 */
	public void operatingSystemStop() {
		// set the current interface :
		lockContext();
		Log.info("operatingSystemStop...");
		if (this.application == null) {
			stop();
			return;
		}
		this.application.onKillDemand(this);
		// release the current interface :
		unLockContext();
	}
	
	/**
	 * @brief The current context is suspended
	 */
	public void operatingSystemSuspend() {
		// set the current interface :
		lockContext();
		Log.info("operatingSystemSuspend...");
		this.previousDisplayTime = 0;
		
		//		if (this.windowsCurrent != null) {
		//			this.windowsCurrent.onStateSuspend();
		//		}
		// release the current interface :
		unLockContext();
	}
	
	private synchronized void postAction(final ActionToDoInAsyncLoop data) {
		this.msgSystem.addElement(data);
		notify();
		//Later, when the necessary event happens, the thread that is running it calls notify() from a block synchronized on the same object.
	}
	
	/**
	 * @brief Processing all the event arrived ... (commoly called in draw function)
	 */
	public void processEvents() {
		int nbEvent = 0;
		//Log.debug(" ********  Event " << this.msgSystem.count());
		while (this.msgSystem.size() > 0) {
			nbEvent++;
			//Log.verbose("    [" << nbEvent << "] event ...");
			final ActionToDoInAsyncLoop func = getAction();
			if (func == null) {
				continue;
			}
			func.run(this);
		}
	}
	
	//	gale::Context::~Context() {
	//		Log.info(" == > Gale system Un-Init (BEGIN)");
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
	//		Log.info("List of all widget of this context must be equal at 0 ==> otherwise some remove is missing");
	//		//this.objectManager.displayListObject();
	//		// Resource is an lower element as objects ...
	//		this.resourceManager.unInit();
	//		// now All must be removed !!!
	//		//this.objectManager.unInit();
	//		// release the current interface :
	//		unLockContext();
	//		Log.info(" == > Gale system Un-Init (END)");
	//		if (this.simulationActive) {
	//			// in simulation case:
	//			this.simulationFile.close();
	//		}
	//	}
	public void requestUpdateSize() {
		postAction((context) -> {
			//Log.debug("Receive MSG : THREAD_RESIZE");
			context.forceRedrawAll();
		});
	}
	
	/**
	 * @brief reset event management for the IO like Input ou Mouse or keyborad
	 */
	public void resetIOEvent() {
		// TODO this.input.newLayerSet();
	}
	
	/**
	 * @brief Internal API to run the processing of the event loop ...
	 * @return The Exit value of the program
	 * @note INTERNAL API
	 */
	public abstract int run();
	
	/**
	 * @brief set the cursor display type.
	 * @param NewCursor selected new cursor.
	 */
	public void setCursor(final Cursor newCursor) {}
	
	/**
	 * @brief The application request a change of his current size force the fullscreen mode.
	 * @param _status status of the fullscreen mode.
	 */
	public void setFullScreen(final boolean status) {
		this.fullscreen = status;
	}
	
	/**
	 * @brief set the Icon of the program
	 * @param _inputFile new filename icon of the current program.
	 */
	public void setIcon(final Uri inputFile) {};
	
	/**
	 * @brief The Application request that the current windows will change his position.
	 * @param _pos New position of the Windows requested.
	 */
	public void setPos(final Vector2f pos) {
		Log.info("setPos: NOT implemented ...");
	};
	
	/**
	 * @brief The application request a change of his current size.
	 * @param _size new Requested size of the windows.
	 */
	public void setSize(final Vector2f size) {
		Log.info("setSize: NOT implemented ...");
	}
	
	/**
	 * @brief set the new title of the windows
	 * @param title New desired title
	 */
	public void setTitle(final String title) {
		Log.info("setTitle: NOT implemented ...");
	};
	
	/**
	 * @brief Enable or Disable the decoration on the Windows (availlable only on Desktop)
	 * @param _status "true" to enable decoration / false otherwise
	 */
	public void setWindowsDecoration(final boolean status) {};
	
	/**
	 * @brief The Application request that the Windows will be visible.
	 */
	public void show() {
		Log.info("show: NOT implemented ...");
	};
	
	/**
	 * @brief StartProcessing (2nd thread).
	 * @note to call when all the Context is started
	 */
	public void start2ndThreadProcessing() {
		// set the current interface:
		lockContext();
		this.periodicThread.threadStart();
		try {
			Thread.sleep(1);
		} catch (final InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		// release the current interface:
		unLockContext();
	};
	
	/**
	 * @brief The application request that the Window will be killed
	 */
	public void stop() {
		Log.warning("stop: NOT implemented for this platform...");
	};
	
	protected void unLockContext() {
		
	}
	
};

class PeriodicThread extends ThreadAbstract {
	private final Context context;
	
	public PeriodicThread(final Context context) {
		super("Galethread 2");
		this.context = context;
	}
	
	@Override
	protected void birth() {
		// TODO Auto-generated method stub
	}
	
	@Override
	protected void death() {
		// TODO Auto-generated method stub
	}
	
	@Override
	protected void runPeriodic() {
		try {
			Thread.sleep(10);
		} catch (final InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			return;
		}
		synchronized (this.context) {
			this.context.processEvents();
			// call all the application for periodic request (the application manage multiple instance )...
			final Application appl = this.context.getApplication();
			if (appl != null) {
				appl.onPeriod(System.currentTimeMillis());
			}
		}
	}
}
