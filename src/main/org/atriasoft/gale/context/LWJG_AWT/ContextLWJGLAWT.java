package org.atriasoft.gale.context.LWJG_AWT;

import java.awt.AWTException;
import java.awt.BorderLayout;
import java.awt.Cursor;
import java.awt.Dimension;
import java.awt.Frame;
import java.awt.Image;
import java.awt.Point;
import java.awt.Rectangle;
import java.awt.Robot;
import java.awt.Toolkit;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseMotionListener;
import java.awt.event.MouseWheelEvent;
import java.awt.event.MouseWheelListener;
import java.awt.image.MemoryImageSource;
import java.util.ArrayList;
import java.util.List;

import javax.swing.JFrame;
import javax.swing.SwingUtilities;
import javax.swing.WindowConstants;

import org.atriasoft.etk.Uri;
import org.atriasoft.etk.math.Vector2f;
import org.atriasoft.gale.DisplayManagerDraw;
import org.atriasoft.gale.Fps;
import org.atriasoft.gale.GaleApplication;
import org.atriasoft.gale.context.GaleContext;
import org.atriasoft.gale.key.KeyKeyboard;
import org.atriasoft.gale.key.KeySpecial;
import org.atriasoft.gale.key.KeyStatus;
import org.atriasoft.gale.key.KeyType;
import org.lwjgl.opengl.GL;
import org.lwjgl.opengl.GL11;
import org.lwjgl.opengl.awt.AWTGLCanvas;
//import org.lwjgl.Version;
//import org.lwjgl.glfw.GLFWErrorCallback;
//import org.lwjgl.glfw.GLFWVidMode;
//import org.lwjgl.opengl.GL;
//import org.lwjgl.system.MemoryStack;
import org.lwjgl.opengl.awt.GLData;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class ContextLWJGLAWT extends GaleContext
		implements MouseListener, MouseMotionListener, KeyListener, MouseWheelListener {
	static final Logger LOGGER = LoggerFactory.getLogger(ContextLWJGLAWT.class);
	private static final int WIDTH = 800;
	private static final int HEIGHT = 600;
	private static final int MAX_MANAGE_INPUT = 15;
	private static final String TITLE = "Gale basic UI";
	private static long lastFrameTime;
	private static float delta;
	private static double whellOffsetY;
	private static double whellOffsetX;

	private static boolean rightButtonStateDown = false;
	private static boolean leftButtonStateDown = false;

	private static double lastMousePositionX = 0;

	private static double lastMousePositionY = 0;
	private static double currentMousePositionX = 0;
	private static double currentMousePositionY = 0;

	public static GaleContext create(final GaleApplication application, final String[] arg) {
		// TODO Auto-generated method stub
		return new ContextLWJGLAWT(application, arg);
	}

	private static long getCurrentTime() {
		return System.currentTimeMillis();
	}

	public static float getFrameTimeSecconds() {
		return ContextLWJGLAWT.delta;
	}

	private final boolean[] inputIsPressed = new boolean[ContextLWJGLAWT.MAX_MANAGE_INPUT];
	private Vector2f decoratedWindowsSize = Vector2f.ZERO;
	private Vector2f cursorPos = Vector2f.ZERO;

	private final Vector2f cursorSize = Vector2f.ZERO;
	private final Fps fps = new Fps("Main Loop", true);

	private DisplayManagerDraw drawer = null;
	// The window handle
	private final long window = 0;
	private final KeySpecial guiKeyBoardMode = new KeySpecial();
	// Generic UI properties
	private JFrame frame;
	private GLData glData;

	private AWTGLCanvas canvas;

	private Robot robot = null;

	private final List<Integer> pressedKey = new ArrayList<>();
	private Boolean isInitialized = false;

	public ContextLWJGLAWT(final GaleApplication application, final String[] args) {
		super(application, args);
		System.out.println("Hello JOGL !");
		initWindows();
		start2ndThreadProcessing();
		this.isInitialized = true;
	}

	private int getUniqueIndex(final KeyEvent e) {
		int internalKeyValue = e.getKeyCode();
		if (e.getKeyLocation() == KeyEvent.KEY_LOCATION_LEFT) {
			internalKeyValue += 100000000;
		} else if (e.getKeyLocation() == KeyEvent.KEY_LOCATION_LEFT) {
			internalKeyValue += 200000000;
		} else if (e.getKeyLocation() == KeyEvent.KEY_LOCATION_NUMPAD) {
			internalKeyValue += 300000000;
		} else if (e.getKeyLocation() == KeyEvent.KEY_LOCATION_STANDARD) {
			internalKeyValue += 400000000;
		}
		return internalKeyValue;
	}

	@Override
	protected void grabPointerEventsThreadGUI(final boolean status, final Vector2f forcedPosition) {
		if (status) {
			try {
				this.robot = new Robot();
				hideCursor();
			} catch (final AWTException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		} else {
			this.robot = null;
			showCursor();
		}
	}

	private void hideCursor() {
		final int[] pixels = new int[16 * 16];
		final Image image = Toolkit.getDefaultToolkit().createImage(new MemoryImageSource(16, 16, pixels, 0, 16));
		final Cursor transparentCursor = Toolkit.getDefaultToolkit().createCustomCursor(image, new Point(0, 0),
				"invisiblecursor");
		this.frame.setCursor(transparentCursor);
	}

	private void initWindows() {
		this.frame = new JFrame("Gale base");
		this.frame.setDefaultCloseOperation(WindowConstants.DISPOSE_ON_CLOSE);
		this.frame.setLayout(new BorderLayout());
		this.frame.setPreferredSize(new Dimension(800, 600));
		this.glData = new GLData();
		this.glData.samples = 4;
		this.glData.swapInterval = 0;
		this.frame.add(this.canvas = new AWTGLCanvas(this.glData) {
			@Override
			public void initGL() {
				System.out.println("OpenGL version: " + this.effective.majorVersion + "." + this.effective.minorVersion
						+ " (Profile: " + this.effective.profile + ")");
				GL.createCapabilities();
				GL11.glClearColor(0.3f, 0.4f, 0.5f, 1);
				GL11.glBlendFunc(GL11.GL_SRC_ALPHA, GL11.GL_ONE_MINUS_SRC_ALPHA);
			}

			@Override
			public void paintGL() {
				final long startRender = System.currentTimeMillis();
				//LOGGER.warn("Draw ... ");
				final int w = getWidth();
				final int h = getHeight();
				if (ContextLWJGLAWT.this.decoratedWindowsSize.x() != w
						|| ContextLWJGLAWT.this.decoratedWindowsSize.y() != h) {
					ContextLWJGLAWT.this.decoratedWindowsSize = new Vector2f(w, h);
					final Rectangle bounds = ContextLWJGLAWT.this.canvas.getBounds();
					final Vector2f tmpWindowsSize = new Vector2f(bounds.width, bounds.height);
					operatingSystemResize(tmpWindowsSize);
				}
				operatingSystemDraw(true);
				swapBuffers();
				/*
				if (Logger.isCriticalOccured()) {
					ContextLWJGLAWT.this.frame.dispose();
				}
				*/
				// Process event from the GUI (specific events...
				processEventsGui();
				/*
				final long stopRender = System.currentTimeMillis();
				try {
					// limit at 60FPS ==> bad to do it here, but it work for now... add a minimum of 10ms to free lock...
					Thread.sleep((int) FMath.max((3000.0f / 60.0f) - (stopRender - startRender), 10)); // This permit to limit the FPS (base 602FPS)
				} catch (final InterruptedException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
				*/
			}
		}, BorderLayout.CENTER);
		this.frame.pack();
		this.frame.setLocationRelativeTo(null);
		this.frame.setVisible(true);
		this.canvas.requestFocus();
		this.canvas.addMouseListener(this);
		this.canvas.addMouseMotionListener(this);
		this.canvas.addKeyListener(this);
		this.canvas.addMouseWheelListener(this);
		this.frame.transferFocus();

		ContextLWJGLAWT.lastFrameTime = ContextLWJGLAWT.getCurrentTime();

	}

	@Override
	public boolean isGrabPointerEvents() {
		return this.robot != null;
	}

	public void keyEvent(final KeyEvent e, final boolean pressed, final boolean thisIsAReapeateKey) {
		//LOGGER.info("event " + thisIsAReapeateKey + "   " + e.getKeyCode() + "   " + e);
		boolean find = true;
		KeyKeyboard keyInput = KeyKeyboard.UNKNOWN;
		//LOGGER.error("keyboard input " + e.getWhen() + "  " + e.getKeyCode() + "  " + e.getKeyLocation());
		switch (e.getKeyCode()) {
			//case 328: // keypad
			case KeyEvent.VK_UP:
				keyInput = KeyKeyboard.UP;
				break;
			//case 324: // keypad
			case KeyEvent.VK_LEFT:
				keyInput = KeyKeyboard.LEFT;
				break;
			//case 326: // keypad
			case KeyEvent.VK_RIGHT:
				keyInput = KeyKeyboard.RIGHT;
				break;
			//case 323: // keypad
			case KeyEvent.VK_DOWN:
				keyInput = KeyKeyboard.DOWN;
				break;
			//case 329: // keypad
			case KeyEvent.VK_PAGE_UP:
				keyInput = KeyKeyboard.PAGE_UP;
				break;
			//case 323: // keypad
			case KeyEvent.VK_PAGE_DOWN:
				keyInput = KeyKeyboard.PAGE_DOWN;
				break;
			//case 327: // keypad
			case KeyEvent.VK_HOME:
				keyInput = KeyKeyboard.START;
				break;
			//case 321: // keypad
			case KeyEvent.VK_END:
				keyInput = KeyKeyboard.END;
				break;
			case KeyEvent.VK_PRINTSCREEN:
				keyInput = KeyKeyboard.STOP_DEFIL;
				break;
			case KeyEvent.VK_PAUSE:
				keyInput = KeyKeyboard.WAIT;
				break;
			//case 320: // keypad
			case KeyEvent.VK_INSERT:
				keyInput = KeyKeyboard.INSERT;
				if (!pressed) {
					if (this.guiKeyBoardMode.getInsert()) {
						this.guiKeyBoardMode.setInsert(false);
					} else {
						this.guiKeyBoardMode.setInsert(true);
					}
				}
				break;
			//case 84:  keyInput = KeyboardCenter; break; // Keypad
			case KeyEvent.VK_F1:
				keyInput = KeyKeyboard.F1;
				break;
			case KeyEvent.VK_F2:
				keyInput = KeyKeyboard.F2;
				break;
			case KeyEvent.VK_F3:
				keyInput = KeyKeyboard.F3;
				break;
			case KeyEvent.VK_F4:
				keyInput = KeyKeyboard.F4;
				break;
			case KeyEvent.VK_F5:
				keyInput = KeyKeyboard.F5;
				break;
			case KeyEvent.VK_F6:
				keyInput = KeyKeyboard.F6;
				break;
			case KeyEvent.VK_F7:
				keyInput = KeyKeyboard.F7;
				break;
			case KeyEvent.VK_F8:
				keyInput = KeyKeyboard.F8;
				break;
			case KeyEvent.VK_F9:
				keyInput = KeyKeyboard.F9;
				break;
			case KeyEvent.VK_F10:
				keyInput = KeyKeyboard.F10;
				break;
			case KeyEvent.VK_F11:
				keyInput = KeyKeyboard.F11;
				break;
			case KeyEvent.VK_F12:
				keyInput = KeyKeyboard.F12;
				break;
			case KeyEvent.VK_CAPS_LOCK:
				keyInput = KeyKeyboard.CAP_LOCK;
				this.guiKeyBoardMode.setCapsLock(pressed);
				break;
			case KeyEvent.VK_SHIFT:
				if (e.getKeyLocation() == KeyEvent.KEY_LOCATION_LEFT) {
					keyInput = KeyKeyboard.SHIFT_LEFT;
					this.guiKeyBoardMode.setShiftLeft(pressed);
					break;
				}
				if (e.getKeyLocation() == KeyEvent.KEY_LOCATION_RIGHT) {
					keyInput = KeyKeyboard.SHIFT_LEFT;
					this.guiKeyBoardMode.setShiftRight(pressed);
					break;
				}
			case KeyEvent.VK_CONTROL:
				if (e.getKeyLocation() == KeyEvent.KEY_LOCATION_LEFT) {
					keyInput = KeyKeyboard.CTRL_LEFT;
					this.guiKeyBoardMode.setCtrlLeft(pressed);
					break;
				}
				if (e.getKeyLocation() == KeyEvent.KEY_LOCATION_RIGHT) {
					keyInput = KeyKeyboard.CTRL_RIGHT;
					this.guiKeyBoardMode.setCtrlRight(pressed);
					break;
				}
			case KeyEvent.VK_META:
				if (e.getKeyLocation() == KeyEvent.KEY_LOCATION_LEFT) {
					keyInput = KeyKeyboard.META_LEFT;
					this.guiKeyBoardMode.setMetaLeft(pressed);
					break;
				}
				if (e.getKeyLocation() == KeyEvent.KEY_LOCATION_RIGHT) {
					keyInput = KeyKeyboard.META_RIGHT;
					this.guiKeyBoardMode.setMetaRight(pressed);
					break;
				}
			case KeyEvent.VK_ALT:
				keyInput = KeyKeyboard.ALT_LEFT;
				this.guiKeyBoardMode.setAltLeft(pressed);
				break;
			case KeyEvent.VK_ALT_GRAPH:
				keyInput = KeyKeyboard.ALT_RIGHT;
				this.guiKeyBoardMode.setAltRight(pressed);
				break;
			case KeyEvent.VK_CONTEXT_MENU:
				keyInput = KeyKeyboard.CONTEXT_MENU;
				break;
			case KeyEvent.VK_NUM_LOCK:
				keyInput = KeyKeyboard.NUM_LOCK;
				this.guiKeyBoardMode.setNumLock(pressed);
				break;
			case KeyEvent.VK_DELETE: // Suppr on keypad
				find = false;
				if (this.guiKeyBoardMode.getNumLock()) {
					if (thisIsAReapeateKey) {
						operatingSystemsetKeyboard(this.guiKeyBoardMode, KeyKeyboard.CHARACTER,
								(!pressed ? KeyStatus.down : KeyStatus.up), thisIsAReapeateKey, '.');
					}
					operatingSystemsetKeyboard(this.guiKeyBoardMode, KeyKeyboard.CHARACTER,
							(pressed ? KeyStatus.down : KeyStatus.up), thisIsAReapeateKey, '.');
				} else {
					if (thisIsAReapeateKey) {
						operatingSystemsetKeyboard(this.guiKeyBoardMode, KeyKeyboard.CHARACTER,
								(!pressed ? KeyStatus.down : KeyStatus.up), thisIsAReapeateKey, (char) 0x7F);
					}
					operatingSystemsetKeyboard(this.guiKeyBoardMode, KeyKeyboard.CHARACTER,
							(pressed ? KeyStatus.down : KeyStatus.up), thisIsAReapeateKey, (char) 0x7F);
				}
				break;
			case KeyEvent.VK_TAB: // special case for TAB
				find = false;
				if (thisIsAReapeateKey) {
					operatingSystemsetKeyboard(this.guiKeyBoardMode, KeyKeyboard.CHARACTER,
							(!pressed ? KeyStatus.down : KeyStatus.up), thisIsAReapeateKey, (char) 0x09);
				}
				operatingSystemsetKeyboard(this.guiKeyBoardMode, KeyKeyboard.CHARACTER,
						(pressed ? KeyStatus.down : KeyStatus.up), thisIsAReapeateKey, (char) 0x09);
				break;
			default:
				find = false;
				if (thisIsAReapeateKey) {
					operatingSystemsetKeyboard(this.guiKeyBoardMode, KeyKeyboard.CHARACTER,
							(!pressed ? KeyStatus.down : KeyStatus.up), thisIsAReapeateKey, e.getKeyChar());
				}
				operatingSystemsetKeyboard(this.guiKeyBoardMode, KeyKeyboard.CHARACTER,
						(pressed ? KeyStatus.down : KeyStatus.up), thisIsAReapeateKey, e.getKeyChar());
		}
		if (find) {
			if (thisIsAReapeateKey) {
				operatingSystemsetKeyboard(this.guiKeyBoardMode, keyInput, (!pressed ? KeyStatus.down : KeyStatus.up),
						thisIsAReapeateKey);
			}
			operatingSystemsetKeyboard(this.guiKeyBoardMode, keyInput, (pressed ? KeyStatus.down : KeyStatus.up),
					thisIsAReapeateKey);
		}
	}

	@Override
	public void keyPressed(final KeyEvent e) {
		final int internalKeyValue = getUniqueIndex(e);
		final int index = this.pressedKey.indexOf(internalKeyValue);
		if (index == -1) {
			this.pressedKey.add(internalKeyValue);
		}
		keyEvent(e, true, index != -1);
	}

	@Override
	public void keyReleased(final KeyEvent e) {
		final int internalKeyValue = getUniqueIndex(e);
		final int index = this.pressedKey.indexOf(internalKeyValue);
		if (index == -1) {
			this.pressedKey.remove(internalKeyValue);
		}
		keyEvent(e, false, false);
	}

	@Override
	public void keyTyped(final KeyEvent e) {
		// not needed with my model ...
		//LOGGER.info(" typed " + e.getKeyChar() + "  " + e);
	}

	@Override
	public void mouseClicked(final MouseEvent e) {
		//		System.out.println(e.getX());
		//		System.out.println(e.getY());
		LOGGER.debug("Mouse clicked: {} {}", e.getX(), e.getY());
	}

	@Override
	public void mouseDragged(final MouseEvent e) {
		//LOGGER.error("mouse drag ... " + e);
		mouseMoved(e);
	}

	@Override
	public void mouseEntered(final MouseEvent e) {
		// TODO Auto-generated method stub
		//LOGGER.info("Mouse entered:" + e.getX() + " " + e.getY());
		this.cursorPos = new Vector2f(e.getX(), this.decoratedWindowsSize.y() - e.getY());
		operatingSystemSetInput(this.guiKeyBoardMode, KeyType.mouse, KeyStatus.enter, 0, this.cursorPos);
	}

	@Override
	public void mouseExited(final MouseEvent e) {
		// TODO Auto-generated method stub
		//LOGGER.info("Mouse exited:" + e.getX() + " " + e.getY());
		this.cursorPos = new Vector2f(e.getX(), this.decoratedWindowsSize.y() - e.getY());
		operatingSystemSetInput(this.guiKeyBoardMode, KeyType.mouse, KeyStatus.leave, 0, this.cursorPos);
		//this.frame.mouseMove(e, 200, 200);
	}

	@Override
	public void mouseMoved(final MouseEvent e) {
		LOGGER.trace("Mouse moved: {} {} {}", e.getX(), e.getY(), e);
		if (this.robot != null) {
			final Rectangle bounds = this.frame.getBounds();
			//LOGGER.error("         " + bounds + " windows=" + windowsSize + " deco= " + decoratedWindowsSize);
			final float refPosX = bounds.x + bounds.width / 2.0f;
			final float refPosY = bounds.y + bounds.height / 2.0f;
			if (e.getXOnScreen() == (int) refPosX && e.getYOnScreen() == (int) refPosY) {
				this.cursorPos = Vector2f.ZERO;
				return;
			}
			//LOGGER.error("         " + bounds + "  windows=" + windowsSize + " deco= " + decoratedWindowsSize);
			this.cursorPos = new Vector2f(-(e.getXOnScreen() - refPosX), (e.getYOnScreen() - refPosY));
			//this.cursorPos = new Vector2f(-(e.getXOnScreen() - refPosX), refPosY);
			this.robot.mouseMove((int) refPosX, (int) refPosY);
			LOGGER.debug("delta moved: {}", this.cursorPos);
		} else {
			// TODO use real size ... !!!!
			this.cursorPos = new Vector2f(e.getX(), this.decoratedWindowsSize.y() + (this.cursorSize.y() - e.getY()));
		}
		// For compatibility of the Android system :
		boolean findOne = false;
		for (int iii = 0; iii < ContextLWJGLAWT.MAX_MANAGE_INPUT; iii++) {
			if (this.inputIsPressed[iii]) {
				//LOGGER.debug("X11 event: bt=" << iii << " " << event.type << " = \"MotionNotify\" (" << m_cursorEventX << "," << m_cursorEventY << ")");
				operatingSystemSetInput(this.guiKeyBoardMode, KeyType.mouse, KeyStatus.move, iii, this.cursorPos);
				findOne = true;
			}
		}
		if (!findOne) {
			//X11_DEBUG("X11 event: bt=" << 0 << " " << event.type << " = \"MotionNotify\" (" << m_cursorEventX << "," << m_cursorEventY << ")");
			operatingSystemSetInput(this.guiKeyBoardMode, KeyType.mouse, KeyStatus.move, 0, this.cursorPos);
		}
	}

	@Override
	public void mousePressed(final MouseEvent e) {
		LOGGER.debug("Mouse pressed: {} {}", e.getX(), e.getY());
		final int button = e.getButton();
		this.cursorPos = new Vector2f(e.getX(), this.decoratedWindowsSize.y() - e.getY());
		if (button < ContextLWJGLAWT.MAX_MANAGE_INPUT) {
			this.inputIsPressed[button] = true;
		}
		operatingSystemSetInput(this.guiKeyBoardMode, KeyType.mouse, KeyStatus.down, button, this.cursorPos);
	}

	@Override
	public void mouseReleased(final MouseEvent e) {
		//LOGGER.info("Mouse release:" + e.getX() + " " + e.getY());
		//		LOGGER.info("mouse value: GLFW_RELEASE" + action + " bt=" + button);
		final int button = e.getButton();
		this.cursorPos = new Vector2f(e.getX(), this.decoratedWindowsSize.y() - e.getY());
		if (button < ContextLWJGLAWT.MAX_MANAGE_INPUT) {
			this.inputIsPressed[button] = false;
		}
		operatingSystemSetInput(this.guiKeyBoardMode, KeyType.mouse, KeyStatus.up, button, this.cursorPos);
	}

	@Override
	public void mouseWheelMoved(final MouseWheelEvent e) {
		//LOGGER.info("wheel_event : " + e);
		this.cursorPos = new Vector2f(e.getX(), this.decoratedWindowsSize.y() - e.getY());
		if (e.getWheelRotation() < 0) {
			this.inputIsPressed[5] = true;
			operatingSystemSetInput(this.guiKeyBoardMode, KeyType.mouse, KeyStatus.down, 5, this.cursorPos);
			this.inputIsPressed[5] = false;
			operatingSystemSetInput(this.guiKeyBoardMode, KeyType.mouse, KeyStatus.up, 5, this.cursorPos);
		} else if (e.getWheelRotation() > 0) {
			this.inputIsPressed[4] = true;
			operatingSystemSetInput(this.guiKeyBoardMode, KeyType.mouse, KeyStatus.down, 4, this.cursorPos);
			this.inputIsPressed[4] = false;
			operatingSystemSetInput(this.guiKeyBoardMode, KeyType.mouse, KeyStatus.up, 4, this.cursorPos);
		}
	}

	@Override
	public int run() {
		final Runnable renderLoop = new Runnable() {
			@Override
			public void run() {
				//				fps.tic();
				if (!ContextLWJGLAWT.this.canvas.isValid()) {
					System.exit(0);
					return;
				}
				if (ContextLWJGLAWT.this.isInitialized) {
					ContextLWJGLAWT.this.canvas.render();
				}
				//				fps.toc();
				//				fps.draw();
				SwingUtilities.invokeLater(this);
			}
		};
		SwingUtilities.invokeLater(renderLoop);

		//		while (canvas != null && canvas.isValid()) {
		//			canvas.render();
		//			try {
		//				Thread.sleep(10);
		//			} catch (InterruptedException e) {
		//				// TODO Auto-generated catch block
		//				e.printStackTrace();
		//			}
		//		}

		// Run the rendering loop until the user has attempted to close
		// the window or has pressed the ESCAPE key.
		//		while ( !glfwWindowShouldClose(window) ) {
		//			/*
		//			fps.tic();
		//			long currentFrameTime = getCurrentTime();
		//			delta = (currentFrameTime-lastFrameTime)/1000f;
		//			lastFrameTime = currentFrameTime;
		//			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear the framebuffer
		//			if (this.drawer != null) {
		//				fps.incrementCounter();
		//				this.drawer.draw();
		//			}
		//			lastMousePositionX = currentMousePositionX;
		//			lastMousePositionY = currentMousePositionY;
		//			whellOffsetY = 0;
		//			whellOffsetY = 0;
		//			glfwSwapBuffers(window); // swap the color buffers
		//			// Poll for window events. The key callback above will only be
		//			// invoked during this call.
		//			glfwPollEvents();
		//			fps.toc();
		//			fps.draw();
		//			*/
		//
		//			glfwSwapBuffers(window); // swap the color buffers
		//			glfwPollEvents();
		//			/*
		//			if (specialEventThatNeedARedraw) {
		//				X11_INFO("specialEventThatNeedARedraw = " << specialEventThatNeedARedraw);
		//			}
		//			hasDisplay = operatingSystemDraw(specialEventThatNeedARedraw);
		//			if (hasDisplay) {
		//				// need to request it every time needed to have a redrawing (this can take some time if the application filter the drfaw periodicity)
		//				specialEventThatNeedARedraw = false;
		//			}
		//			*/
		//		}
		//System.exit(0);
		return 0;
	}

	public void setDrawer(final DisplayManagerDraw drawer) {
		this.drawer = drawer;
	}

	@Override
	public void setFullScreenThreadGUI(final boolean status) {
		super.setFullScreen(status);
		if (status) {
			this.frame.setExtendedState(Frame.MAXIMIZED_BOTH);
			this.frame.setUndecorated(true);
		} else {
			this.frame.setExtendedState(Frame.NORMAL);
			this.frame.setUndecorated(false);
		}
	}

	@Override
	public void setIconThreadGUI(final Uri inputFile) {

	}

	/****************************************************************************************/
	@Override
	public void setTitleThreadGUI(final String title) {
		this.frame.setTitle(title);
	}

	private void showCursor() {
		this.frame.setCursor(Cursor.getPredefinedCursor(Cursor.DEFAULT_CURSOR));
	}

	public void unInit() {

	}

}
