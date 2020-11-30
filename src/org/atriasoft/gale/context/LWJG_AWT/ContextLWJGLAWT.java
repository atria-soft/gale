package org.atriasoft.gale.context.LWJG_AWT;

import java.awt.AWTException;
import java.awt.BorderLayout;
import java.awt.Cursor;
import java.awt.Dimension;
import java.awt.Image;
import java.awt.Point;
import java.awt.Rectangle;
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
import java.awt.Robot;

import javax.swing.JFrame;
import javax.swing.SwingUtilities;

import org.atriasoft.etk.Uri;
import org.atriasoft.etk.math.Vector2f;
import org.atriasoft.gale.Application;
import org.atriasoft.gale.DisplayManagerDraw;
import org.atriasoft.gale.Fps;
import org.atriasoft.gale.context.Context;
import org.atriasoft.gale.key.KeyKeyboard;
import org.atriasoft.gale.key.KeySpecial;
import org.atriasoft.gale.key.KeyStatus;
import org.atriasoft.gale.key.KeyType;
import org.atriasoft.gale.internal.Log;
import org.lwjgl.opengl.GL11;
import org.lwjgl.opengl.awt.AWTGLCanvas;
//import org.lwjgl.Version;
//import org.lwjgl.glfw.GLFWErrorCallback;
//import org.lwjgl.glfw.GLFWVidMode;
//import org.lwjgl.opengl.GL;
//import org.lwjgl.system.MemoryStack;
import org.lwjgl.opengl.awt.GLData;

import static org.lwjgl.opengl.GL.createCapabilities;
import static org.lwjgl.opengl.GL11.glClearColor;

public class ContextLWJGLAWT extends Context implements MouseListener, MouseMotionListener, KeyListener, MouseWheelListener {
	private boolean[] inputIsPressed = new boolean[MAX_MANAGE_INPUT];
	private Vector2f decoratedWindowsSize = new Vector2f(0, 0);
	private Vector2f cursorPos = new Vector2f(0, 0);
	private Vector2f cursorSize = new Vector2f(0, 0);
	private static final int WIDTH = 800;
	private static final int HEIGHT = 600;
	private static final String TITLE = "Gale basic UI";
	
	private static long lastFrameTime;
	private static float delta;
	
	private Fps fps = new Fps("Main Loop", true);
	
	private DisplayManagerDraw drawer = null;
	private static double whellOffsetY;
	private static double whellOffsetX;
	private static boolean rightButtonStateDown = false;
	private static boolean leftButtonStateDown = false;
	private static double lastMousePositionX = 0;
	private static double lastMousePositionY = 0;
	private static double currentMousePositionX = 0;
	private static double currentMousePositionY = 0;

	// The window handle
	private long window = 0;
	private KeySpecial guiKeyBoardMode = new KeySpecial();
	
	// Generic UI properties
	private JFrame frame;
	private GLData glData;
	private AWTGLCanvas canvas;
	private Robot robot = null;
	public ContextLWJGLAWT(Application application, String[] args) {
		super(application, args);
		System.out.println("Hello JOGL !");
		initWindows();
		start2ndThreadProcessing();
	}
	
	
	public void setDrawer(DisplayManagerDraw drawer) {
		this.drawer = drawer;
	}
	
	public void unInit() {
		
	}

	@SuppressWarnings("serial")
	private void initWindows() {
		frame = new JFrame("Gale base");
		frame.setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);
		frame.setLayout(new BorderLayout());
		frame.setPreferredSize(new Dimension(800, 600));
		glData = new GLData();
		glData.samples = 4;
		glData.swapInterval = 0;
		frame.add(canvas = new AWTGLCanvas(glData) {
			public void initGL() {
				System.out.println("OpenGL version: " + effective.majorVersion + "." + effective.minorVersion + " (Profile: " + effective.profile + ")");
				createCapabilities();
				glClearColor(0.3f, 0.4f, 0.5f, 1);
				GL11.glBlendFunc(GL11.GL_SRC_ALPHA, GL11.GL_ONE_MINUS_SRC_ALPHA);
			}
			public void paintGL() {
				//Log.warning("Draw ... ");
				int w = getWidth();
				int h = getHeight();
				if (decoratedWindowsSize.x != w || decoratedWindowsSize.y != h) {
					decoratedWindowsSize.x = w;
					decoratedWindowsSize.y = h;
					Rectangle bounds = canvas.getBounds();
					windowsSize.x = bounds.width;
					windowsSize.y = bounds.height;
					operatingSystemResize(windowsSize);
				}
				operatingSystemDraw(true);
				swapBuffers();
			}
		}, BorderLayout.CENTER);
		frame.pack();
		frame.setLocationRelativeTo(null);
		frame.setVisible(true);
		canvas.requestFocus();
		canvas.addMouseListener(this);
		canvas.addMouseMotionListener(this);
		canvas.addKeyListener(this);
		canvas.addMouseWheelListener(this);
		frame.transferFocus();
		
		lastFrameTime = getCurrentTime();
		
	}
	@Override
	public void mouseClicked(MouseEvent e) {
//		System.out.println(e.getX());
//		System.out.println(e.getY());
		Log.info("Mouse clicked:" + e.getX() + " " + e.getY());
	}
	@Override
	public void mouseEntered(MouseEvent e) {
		// TODO Auto-generated method stub
		//Log.info("Mouse entered:" + e.getX() + " " + e.getY());
		cursorPos.x = e.getX();
		cursorPos.y = e.getY();
		operatingSystemSetInput(guiKeyBoardMode,
				KeyType.mouse,
				KeyStatus.enter,
				0,
				cursorPos);
	}
	@Override
	public void mouseExited(MouseEvent e) {
		// TODO Auto-generated method stub
		//Log.info("Mouse exited:" + e.getX() + " " + e.getY());
		cursorPos.x = e.getX();
		cursorPos.y = e.getY();
		operatingSystemSetInput(guiKeyBoardMode,
				KeyType.mouse,
				KeyStatus.leave,
				0,
				cursorPos);
		//this.frame.mouseMove(e, 200, 200);
	}
	@Override
	public void mousePressed(MouseEvent e) {
		Log.info("Mouse pressed:" + e.getX() + " " + e.getY());
		int button = e.getButton();
		cursorPos.x = e.getX();
		cursorPos.y = e.getY();
		if (button < MAX_MANAGE_INPUT) {
			inputIsPressed[button] = true;
		}
		operatingSystemSetInput(guiKeyBoardMode,
				KeyType.mouse,
				KeyStatus.down,
				button,
				cursorPos);
	}
	@Override
	public void mouseReleased(MouseEvent e) {
		//Log.info("Mouse release:" + e.getX() + " " + e.getY());
//		Log.info("mouse value: GLFW_RELEASE" + action + " bt=" + button);
		int button = e.getButton();
		cursorPos.x = e.getX();
		cursorPos.y = e.getY();
		if (button < MAX_MANAGE_INPUT) {
			inputIsPressed[button] = false;
		}
		operatingSystemSetInput(guiKeyBoardMode,
				KeyType.mouse,
				KeyStatus.up,
				button,
				cursorPos);
	}

	@Override
	public void mouseDragged(MouseEvent e) {
		//Log.error("mouse drag ... " + e);
		mouseMoved(e);
	}
	public void mouseMoved(MouseEvent e) {
		//Log.info("Mouse moved:" + e.getX() + " " + e.getY() + " " + e);
		if (this.robot != null) {
			Rectangle bounds = frame.getBounds();
			//Log.error("         " + bounds + " windows=" + windowsSize + " deco= " + decoratedWindowsSize);
			float refPosX = bounds.x + bounds.width/2;
			float refPosY = bounds.y + bounds.height/2;
			if (e.getXOnScreen() == (int)refPosX
					&& e.getYOnScreen() == (int)refPosY) {
				cursorPos.x = 0;
				cursorPos.y = 0;
				return;
			} else {
				//Log.error("         " + bounds + "  windows=" + windowsSize + " deco= " + decoratedWindowsSize);
				cursorPos.x = -((float)e.getXOnScreen() - refPosX);
				cursorPos.y = (float)e.getYOnScreen() - refPosY;
				robot.mouseMove((int)refPosX, (int)refPosY);
			}
			Log.info("delta moved:" + cursorPos);
		} else {
			// TODO use real size ... !!!!
			cursorPos.x = (float)e.getX();
			cursorPos.y = cursorSize.y - (float)e.getY();
		}
		// For compatibility of the Android system : 
		boolean findOne = false;
		for (int iii=0; iii<MAX_MANAGE_INPUT; iii++) {
			if (inputIsPressed[iii] == true) {
				//Log.debug("X11 event: bt=" << iii << " " << event.type << " = \"MotionNotify\" (" << m_cursorEventX << "," << m_cursorEventY << ")");
				operatingSystemSetInput(guiKeyBoardMode,
						KeyType.mouse,
				            KeyStatus.move,
				            iii,
				            cursorPos);
				findOne = true;
			}
		}
		if (findOne == false) {
			//X11_DEBUG("X11 event: bt=" << 0 << " " << event.type << " = \"MotionNotify\" (" << m_cursorEventX << "," << m_cursorEventY << ")");
			operatingSystemSetInput(guiKeyBoardMode,
					KeyType.mouse,
		            KeyStatus.move,
		            0,
		            cursorPos);
		}
	}
	private List<Integer> pressedKey = new ArrayList<Integer>();
	
	private int getUniqueIndex(KeyEvent e) {
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
	public void keyPressed(KeyEvent e) {
		int internalKeyValue = getUniqueIndex(e);
		int index = pressedKey.indexOf(internalKeyValue);
		if (index == -1) {
			pressedKey.add(internalKeyValue);
		}
		keyEvent(e, true, index != -1);
	}
	public void keyReleased(KeyEvent e) {
		int internalKeyValue = getUniqueIndex(e);
		int index = pressedKey.indexOf(internalKeyValue);
		if (index == -1) {
			pressedKey.remove(internalKeyValue);
		}
		keyEvent(e, false, false);
	}
	public void keyEvent(KeyEvent e, boolean pressed, boolean thisIsAReapeateKey) {
		//Log.info("event " + thisIsAReapeateKey + "   " + e.getKeyCode() + "   " + e);
		boolean find = true;
		KeyKeyboard keyInput = KeyKeyboard.unknow;
		//Log.error("keyboard input " + e.getWhen() + "  " + e.getKeyCode() + "  " + e.getKeyLocation());
		switch (e.getKeyCode()) {
			//case 328: // keypad
			case KeyEvent.VK_UP:         keyInput = KeyKeyboard.up;            break;
			//case 324: // keypad
			case KeyEvent.VK_LEFT:       keyInput = KeyKeyboard.left;          break;
			//case 326: // keypad
			case KeyEvent.VK_RIGHT:      keyInput = KeyKeyboard.right;         break;
			//case 323: // keypad
			case KeyEvent.VK_DOWN:       keyInput = KeyKeyboard.down;          break;
			//case 329: // keypad
			case KeyEvent.VK_PAGE_UP:    keyInput = KeyKeyboard.pageUp;        break;
			//case 323: // keypad
			case KeyEvent.VK_PAGE_DOWN:  keyInput = KeyKeyboard.pageDown;      break;
			//case 327: // keypad
			case KeyEvent.VK_HOME:       keyInput = KeyKeyboard.start;         break;
			//case 321: // keypad
			case KeyEvent.VK_END:        keyInput = KeyKeyboard.end;           break;
			case KeyEvent.VK_PRINTSCREEN:keyInput = KeyKeyboard.stopDefil;     break;
			case KeyEvent.VK_PAUSE:      keyInput = KeyKeyboard.wait;          break;
			//case 320: // keypad
			case KeyEvent.VK_INSERT:
				keyInput = KeyKeyboard.insert;
				if(pressed == false) {
					if (guiKeyBoardMode.getInsert() == true) {
						guiKeyBoardMode.setInsert(false);
					} else {
						guiKeyBoardMode.setInsert(true);
					}
				}
				break;
			//case 84:  keyInput = KeyboardCenter; break; // Keypad
			case KeyEvent.VK_F1:     keyInput = KeyKeyboard.f1; break;
			case KeyEvent.VK_F2:     keyInput = KeyKeyboard.f2; break;
			case KeyEvent.VK_F3:     keyInput = KeyKeyboard.f3; break;
			case KeyEvent.VK_F4:     keyInput = KeyKeyboard.f4; break;
			case KeyEvent.VK_F5:     keyInput = KeyKeyboard.f5; break;
			case KeyEvent.VK_F6:     keyInput = KeyKeyboard.f6; break;
			case KeyEvent.VK_F7:     keyInput = KeyKeyboard.f7; break;
			case KeyEvent.VK_F8:     keyInput = KeyKeyboard.f8; break;
			case KeyEvent.VK_F9:     keyInput = KeyKeyboard.f9; break;
			case KeyEvent.VK_F10:    keyInput = KeyKeyboard.f10; break;
			case KeyEvent.VK_F11:    keyInput = KeyKeyboard.f11; break;
			case KeyEvent.VK_F12:    keyInput = KeyKeyboard.f12; break;
			case KeyEvent.VK_CAPS_LOCK:
				keyInput = KeyKeyboard.capLock;
				guiKeyBoardMode.setCapsLock(pressed == true);
				break;
			case KeyEvent.VK_SHIFT:
				if (e.getKeyLocation() == KeyEvent.KEY_LOCATION_LEFT) {
					keyInput = KeyKeyboard.shiftLeft;   guiKeyBoardMode.setShiftLeft (pressed == true); break;
				} else if (e.getKeyLocation() == KeyEvent.KEY_LOCATION_RIGHT) {
					keyInput = KeyKeyboard.shiftLeft;   guiKeyBoardMode.setShiftRight (pressed == true); break;
				}
			case KeyEvent.VK_CONTROL:
				if (e.getKeyLocation() == KeyEvent.KEY_LOCATION_LEFT) {
					keyInput = KeyKeyboard.ctrlLeft;   guiKeyBoardMode.setCtrlLeft (pressed == true); break;
				} else if (e.getKeyLocation() == KeyEvent.KEY_LOCATION_RIGHT) {
					keyInput = KeyKeyboard.ctrlRight;   guiKeyBoardMode.setCtrlRight (pressed == true); break;
				}
			case KeyEvent.VK_META:
				if (e.getKeyLocation() == KeyEvent.KEY_LOCATION_LEFT) {
					keyInput = KeyKeyboard.metaLeft;   guiKeyBoardMode.setMetaLeft (pressed == true); break;
				} else if (e.getKeyLocation() == KeyEvent.KEY_LOCATION_RIGHT) {
					keyInput = KeyKeyboard.metaRight;   guiKeyBoardMode.setMetaRight (pressed == true); break;
				}
			case KeyEvent.VK_ALT:
				keyInput = KeyKeyboard.altLeft;
				guiKeyBoardMode.setAltLeft(pressed == true);
				break;
			case KeyEvent.VK_ALT_GRAPH:
				keyInput = KeyKeyboard.altRight;
				guiKeyBoardMode.setAltRight(pressed == true);
				break;
			case KeyEvent.VK_CONTEXT_MENU:
				keyInput = KeyKeyboard.contextMenu;
				break;
			case KeyEvent.VK_NUM_LOCK:
				keyInput = KeyKeyboard.numLock;
				guiKeyBoardMode.setNumLock(pressed == true);
				break;
			case KeyEvent.VK_DELETE: // Suppr on keypad
				find = false;
				if(guiKeyBoardMode.getNumLock() == true){
					if (thisIsAReapeateKey == true) {
						operatingSystemsetKeyboard(guiKeyBoardMode,
						               KeyKeyboard.character,
						               (pressed != true?KeyStatus.down:KeyStatus.up),
						               thisIsAReapeateKey,
						               '.');
					}
					operatingSystemsetKeyboard(guiKeyBoardMode,
					               KeyKeyboard.character,
					               (pressed == true?KeyStatus.down:KeyStatus.up),
					               thisIsAReapeateKey,
					               '.');
				} else {
					if (thisIsAReapeateKey == true) {
						operatingSystemsetKeyboard(guiKeyBoardMode,
						               KeyKeyboard.character,
						               (pressed != true?KeyStatus.down:KeyStatus.up),
						               thisIsAReapeateKey,
						               (char)0x7F);
					}
					operatingSystemsetKeyboard(guiKeyBoardMode,
					               KeyKeyboard.character,
					               (pressed == true?KeyStatus.down:KeyStatus.up),
					               thisIsAReapeateKey,
					               (char)0x7F);
				}
				break;
			case KeyEvent.VK_TAB: // special case for TAB
				find = false;
				if (thisIsAReapeateKey == true) {
					operatingSystemsetKeyboard(guiKeyBoardMode,
					               KeyKeyboard.character,
					               (pressed==false?KeyStatus.down:KeyStatus.up),
					               thisIsAReapeateKey,
					               (char)0x09);
				}
				operatingSystemsetKeyboard(guiKeyBoardMode,
				               KeyKeyboard.character,
				               (pressed == true?KeyStatus.down:KeyStatus.up),
				               thisIsAReapeateKey,
				               (char)0x09);
				break;
			default:
				find = false;
				if (thisIsAReapeateKey == true) {
					operatingSystemsetKeyboard(guiKeyBoardMode,
							KeyKeyboard.character,
							(pressed==false?KeyStatus.down:KeyStatus.up),
							thisIsAReapeateKey,
							e.getKeyChar());
				}
				operatingSystemsetKeyboard(guiKeyBoardMode,
						KeyKeyboard.character,
						(pressed==true?KeyStatus.down:KeyStatus.up),
						thisIsAReapeateKey,
						e.getKeyChar());
		}
		if (find == true) {
			if (thisIsAReapeateKey == true) {
				operatingSystemsetKeyboard(guiKeyBoardMode,
						keyInput,
						(pressed==false?KeyStatus.down:KeyStatus.up),
						thisIsAReapeateKey);
			}
			operatingSystemsetKeyboard(guiKeyBoardMode,
					keyInput,
					(pressed == true?KeyStatus.down:KeyStatus.up),
					thisIsAReapeateKey);
		}
	}

	public void keyTyped(KeyEvent e) {
		// not needed with my model ...
		//Log.info(" typed " + e.getKeyChar() + "  " + e);
	}



	@Override
	public void mouseWheelMoved(MouseWheelEvent e) {
		//Log.info("wheel_event : " + e);
		cursorPos.x = e.getX();
		cursorPos.y = e.getY();
		if (e.getWheelRotation()<0) {
			inputIsPressed[5] = true;
			operatingSystemSetInput(guiKeyBoardMode,
					KeyType.mouse,
					KeyStatus.down,
					5,
					cursorPos);
			inputIsPressed[5] = false;
			operatingSystemSetInput(guiKeyBoardMode,
					KeyType.mouse,
					KeyStatus.up,
					5,
					cursorPos);
		} else if (e.getWheelRotation()>0) {
			inputIsPressed[4] = true;
			operatingSystemSetInput(guiKeyBoardMode,
					KeyType.mouse,
					KeyStatus.down,
					4,
					cursorPos);
			inputIsPressed[4] = false;
			operatingSystemSetInput(guiKeyBoardMode,
					KeyType.mouse,
					KeyStatus.up,
					4,
					cursorPos);
		} 
	}
	public static float getFrameTimeSecconds() {
		return delta;
	}
	
	private static long getCurrentTime() {
		return System.currentTimeMillis();
	}

	@Override
	public int run() {
		Runnable renderLoop = new Runnable() {
			public void run() {
//				fps.tic();
				if (!canvas.isValid()) {
					System.exit(0);
					return;
				}
				canvas.render();
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
	/****************************************************************************************/
	@Override
	public void setTitle(String title) {
		this.frame.setTitle(title);
	}
	@Override
	public void setIcon(Uri inputFile) {
		
	}
	private void hideCursor() {
		int[] pixels = new int[16 * 16];
		Image image = Toolkit.getDefaultToolkit().createImage(new MemoryImageSource(16, 16, pixels, 0, 16));
		Cursor transparentCursor = Toolkit.getDefaultToolkit().createCustomCursor(image, new Point(0, 0), "invisiblecursor");
		frame.setCursor(transparentCursor);
	}
	private void showCursor() {
		frame.setCursor(Cursor.getPredefinedCursor(Cursor.DEFAULT_CURSOR));
	}
	@Override
	public void grabPointerEvents(boolean _status,  Vector2f _forcedPosition) {
		if (_status == true) {
			try {
				this.robot = new Robot();
				hideCursor();
			} catch (AWTException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		} else {
			this.robot = null;
			showCursor();
		}
	};
	@Override
	public boolean isGrabPointerEvents() { return this.robot != null; };
	@Override
	public void setFullScreen(boolean status){
		super.setFullScreen(status);
		if (status == true) {
			frame.setExtendedState(JFrame.MAXIMIZED_BOTH); 
			frame.setUndecorated(true);
		} else {
			frame.setExtendedState(JFrame.NORMAL); 
			frame.setUndecorated(false);
		}
	};
	
	public static Context create(Application application, String[] arg) {
		// TODO Auto-generated method stub
		return new ContextLWJGLAWT(application, arg);
	}



}
