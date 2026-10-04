package org.atriasoft.gale.context.LWJG_AWT;

import java.awt.event.InputEvent;
import java.awt.event.KeyEvent;
import java.util.HashMap;
import java.util.Map;

/**
 * The character a key of the keyboard stands for, as gale hands it to the
 * application with a {@code KeyKeyboard.CHARACTER} event.
 * <p>
 * AWT types control characters for a letter held with Control (Ctrl+W is
 * {@code '\u0017'}, Ctrl+Z {@code '\u001a'}, Ctrl+I a Tab, Ctrl+M a Return):
 * an application that bound {@code w} (a game moving forward while it
 * crouches on Ctrl) or {@code ctrl+c} (a copy shortcut) never saw its key.
 * Here a key held with Control, and neither Alt nor AltGr, types the
 * character of the key itself, rebuilt from its key code: AWT gives the code
 * of the symbol the key bears in the keyboard layout in use (on an AZERTY
 * keyboard the key left of E is {@code VK_Z}), never of its position on a US
 * keyboard; a symbol without a code of its own comes as its extended key code
 * (Unicode + 0x01000000). The keys that type a control character by
 * themselves (Return, Backspace, Escape...) keep it. With Alt or AltGr held,
 * the character is what AWT typed: AltGr types the third symbol of a key
 * ({@code @}, {@code €}...), and where AltGr comes with Control (Windows)
 * a key without third symbol types nothing, which must not become its
 * letter; a real Control+Alt chord keeps its control character.
 * <p>
 * A release names the character its press named: a letter pressed alone and
 * released once Control (or AltGr) is down is still released, and the
 * application does not keep it held for ever. Each key of the keyboard has
 * its own entry ({@link #keyOf}), the keys AWT has no key code for included.
 */
public final class TypedCharacters {

	/** AWT's {@link KeyEvent#CHAR_UNDEFINED}. */
	private static final char UNDEFINED = KeyEvent.CHAR_UNDEFINED;
	/** Added by AWT to the Unicode value of a symbol that has no key code of its own. */
	private static final int UNICODE_KEY_CODE = 0x01000000;
	/** Gap between the key numbers of two locations: above every key code and extended key code. */
	private static final int LOCATION_STEP = 100_000_000;
	/** Alt and AltGr: with either, the character is what AWT typed. */
	private static final int ALT_OR_ALT_GRAPH = InputEvent.ALT_DOWN_MASK | InputEvent.ALT_GRAPH_DOWN_MASK;
	private static final char FIRST_PRINTABLE = ' ';
	private static final char DELETE = '\u007f';
	private static final Map<Integer, Character> SYMBOLS = symbols();

	/** The character each key held down typed when it was pressed, by key ({@link #keyOf}). */
	private final Map<Integer, Character> typedAtPress = new HashMap<>();

	/**
	 * The number of a key of the keyboard: the same at its press and at its
	 * release, another for each key. AWT gives no key code
	 * ({@link KeyEvent#VK_UNDEFINED}) to a key whose symbol has none (é, è,
	 * ç, à, ù and ² on an AZERTY keyboard), but each its own extended key
	 * code; the location tells the left Shift from the right one.
	 *
	 * @param keyCode         its key code ({@link KeyEvent#getKeyCode()})
	 * @param extendedKeyCode its extended key code ({@link KeyEvent#getExtendedKeyCode()}), 0 if unknown
	 * @param location        its location ({@link KeyEvent#getKeyLocation()})
	 */
	public static int keyOf(final int keyCode, final int extendedKeyCode, final int location) {
		final int code = extendedKeyCode != KeyEvent.VK_UNDEFINED ? extendedKeyCode : keyCode;
		return location * LOCATION_STEP + code;
	}

	/**
	 * The character of a key pressed.
	 *
	 * @param keyChar         the character AWT typed ({@link KeyEvent#getKeyChar()})
	 * @param keyCode         its key code ({@link KeyEvent#getKeyCode()})
	 * @param extendedKeyCode its extended key code ({@link KeyEvent#getExtendedKeyCode()})
	 * @param location        its location ({@link KeyEvent#getKeyLocation()})
	 * @param modifiers       the keys held ({@link KeyEvent#getModifiersEx()})
	 */
	public char pressed(final char keyChar, final int keyCode, final int extendedKeyCode, final int location,
			final int modifiers) {
		final char typed = typed(keyChar, keyCode, extendedKeyCode, modifiers);
		this.typedAtPress.put(keyOf(keyCode, extendedKeyCode, location), typed);
		return typed;
	}

	/**
	 * The character of a key released: the one its press typed, else (its
	 * press came before the window had the focus) the one it types now.
	 *
	 * @see #pressed
	 */
	public char released(final char keyChar, final int keyCode, final int extendedKeyCode, final int location,
			final int modifiers) {
		final Character atPress = this.typedAtPress.remove(keyOf(keyCode, extendedKeyCode, location));
		return atPress != null ? atPress : typed(keyChar, keyCode, extendedKeyCode, modifiers);
	}

	/**
	 * The character a key types for the application (see the class comment):
	 * what AWT typed, unless Control, without Alt nor AltGr, turned it into a
	 * control character, then the symbol of the key, in upper case with Shift.
	 *
	 * @param keyChar         the character AWT typed
	 * @param keyCode         the key code of the key
	 * @param extendedKeyCode the extended key code of the key
	 * @param modifiers       the keys held ({@link KeyEvent#getModifiersEx()})
	 */
	public static char typed(final char keyChar, final int keyCode, final int extendedKeyCode, final int modifiers) {
		final boolean controlAlone = (modifiers & InputEvent.CTRL_DOWN_MASK) != 0
				&& (modifiers & ALT_OR_ALT_GRAPH) == 0;
		if (!controlAlone || !madeByControl(keyChar)) {
			return keyChar;
		}
		final char symbol = symbolOf(keyCode, extendedKeyCode);
		if (symbol == UNDEFINED) {
			return keyChar;
		}
		return (modifiers & InputEvent.SHIFT_DOWN_MASK) != 0 ? Character.toUpperCase(symbol) : symbol;
	}

	/** Whether Control may have made {@code keyChar}: a control character, or none. */
	private static boolean madeByControl(final char keyChar) {
		return keyChar < FIRST_PRINTABLE || keyChar == DELETE || keyChar == UNDEFINED;
	}

	/**
	 * The symbol a key bears in the layout in use, in lower case, from its key
	 * codes; {@link KeyEvent#CHAR_UNDEFINED} for a key that types no symbol
	 * (Return, Escape, an arrow...).
	 */
	static char symbolOf(final int keyCode, final int extendedKeyCode) {
		if (keyCode >= KeyEvent.VK_A && keyCode <= KeyEvent.VK_Z) {
			return (char) ('a' + keyCode - KeyEvent.VK_A);
		}
		if (keyCode >= KeyEvent.VK_0 && keyCode <= KeyEvent.VK_9) {
			return (char) ('0' + keyCode - KeyEvent.VK_0);
		}
		if (keyCode >= KeyEvent.VK_NUMPAD0 && keyCode <= KeyEvent.VK_NUMPAD9) {
			return (char) ('0' + keyCode - KeyEvent.VK_NUMPAD0);
		}
		final Character symbol = SYMBOLS.get(keyCode);
		if (symbol != null) {
			return symbol;
		}
		if (extendedKeyCode > UNICODE_KEY_CODE && extendedKeyCode - UNICODE_KEY_CODE < UNDEFINED) {
			return Character.toLowerCase((char) (extendedKeyCode - UNICODE_KEY_CODE));
		}
		return UNDEFINED;
	}

	/** The key codes AWT gives to the keys that bear a symbol other than a letter or a digit. */
	private static Map<Integer, Character> symbols() {
		final Map<Integer, Character> symbols = new HashMap<>();
		symbols.put(KeyEvent.VK_SPACE, ' ');
		symbols.put(KeyEvent.VK_COMMA, ',');
		symbols.put(KeyEvent.VK_MINUS, '-');
		symbols.put(KeyEvent.VK_PERIOD, '.');
		symbols.put(KeyEvent.VK_SLASH, '/');
		symbols.put(KeyEvent.VK_SEMICOLON, ';');
		symbols.put(KeyEvent.VK_EQUALS, '=');
		symbols.put(KeyEvent.VK_OPEN_BRACKET, '[');
		symbols.put(KeyEvent.VK_BACK_SLASH, '\\');
		symbols.put(KeyEvent.VK_CLOSE_BRACKET, ']');
		symbols.put(KeyEvent.VK_QUOTE, '\'');
		symbols.put(KeyEvent.VK_BACK_QUOTE, '`');
		symbols.put(KeyEvent.VK_AMPERSAND, '&');
		symbols.put(KeyEvent.VK_ASTERISK, '*');
		symbols.put(KeyEvent.VK_QUOTEDBL, '"');
		symbols.put(KeyEvent.VK_LESS, '<');
		symbols.put(KeyEvent.VK_GREATER, '>');
		symbols.put(KeyEvent.VK_BRACELEFT, '{');
		symbols.put(KeyEvent.VK_BRACERIGHT, '}');
		symbols.put(KeyEvent.VK_AT, '@');
		symbols.put(KeyEvent.VK_COLON, ':');
		symbols.put(KeyEvent.VK_CIRCUMFLEX, '^');
		symbols.put(KeyEvent.VK_DOLLAR, '$');
		symbols.put(KeyEvent.VK_EURO_SIGN, '€');
		symbols.put(KeyEvent.VK_EXCLAMATION_MARK, '!');
		symbols.put(KeyEvent.VK_INVERTED_EXCLAMATION_MARK, '¡');
		symbols.put(KeyEvent.VK_LEFT_PARENTHESIS, '(');
		symbols.put(KeyEvent.VK_RIGHT_PARENTHESIS, ')');
		symbols.put(KeyEvent.VK_NUMBER_SIGN, '#');
		symbols.put(KeyEvent.VK_PLUS, '+');
		symbols.put(KeyEvent.VK_UNDERSCORE, '_');
		symbols.put(KeyEvent.VK_MULTIPLY, '*');
		symbols.put(KeyEvent.VK_ADD, '+');
		symbols.put(KeyEvent.VK_SUBTRACT, '-');
		symbols.put(KeyEvent.VK_DIVIDE, '/');
		symbols.put(KeyEvent.VK_DECIMAL, '.');
		return Map.copyOf(symbols);
	}
}
