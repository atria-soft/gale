package org.atriasoft.gale.context.LWJG_AWT;

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
 * Here a key held with Control types the character of the key itself, rebuilt
 * from its key code: AWT gives the code of the symbol the key bears in the
 * keyboard layout in use (on an AZERTY keyboard the key left of E is
 * {@code VK_Z}), never of its position on a US keyboard; a symbol without a
 * code of its own comes as its extended key code (Unicode + 0x01000000). The
 * keys that type a control character by themselves (Return, Backspace,
 * Escape...) keep it; Shift, AltGr and Alt keep what AWT types (AltGr types
 * the third symbol of a key: {@code @}, {@code €}...).
 * <p>
 * A release names the character its press named: a letter pressed alone and
 * released once Control (or AltGr) is down is still released, and the
 * application does not keep it held for ever.
 */
public final class TypedCharacters {

	/** AWT's {@link KeyEvent#CHAR_UNDEFINED}. */
	private static final char UNDEFINED = KeyEvent.CHAR_UNDEFINED;
	/** Added by AWT to the Unicode value of a symbol that has no key code of its own. */
	private static final int UNICODE_KEY_CODE = 0x01000000;
	private static final char FIRST_PRINTABLE = ' ';
	private static final char DELETE = '\u007f';
	private static final Map<Integer, Character> SYMBOLS = symbols();

	/** The character each key held down typed when it was pressed, by key. */
	private final Map<Integer, Character> typedAtPress = new HashMap<>();

	/**
	 * The character of a key pressed.
	 *
	 * @param key             the key, the same number for its press and its release (code and location)
	 * @param keyChar         the character AWT typed ({@link KeyEvent#getKeyChar()})
	 * @param keyCode         its key code ({@link KeyEvent#getKeyCode()})
	 * @param extendedKeyCode its extended key code ({@link KeyEvent#getExtendedKeyCode()})
	 * @param control         whether Control is held
	 * @param shift           whether Shift is held
	 */
	public char pressed(final int key, final char keyChar, final int keyCode, final int extendedKeyCode,
			final boolean control, final boolean shift) {
		final char typed = typed(keyChar, keyCode, extendedKeyCode, control, shift);
		this.typedAtPress.put(key, typed);
		return typed;
	}

	/**
	 * The character of a key released: the one its press typed, else (its
	 * press came before the window had the focus) the one it types now.
	 *
	 * @see #pressed
	 */
	public char released(final int key, final char keyChar, final int keyCode, final int extendedKeyCode,
			final boolean control, final boolean shift) {
		final Character atPress = this.typedAtPress.remove(key);
		return atPress != null ? atPress : typed(keyChar, keyCode, extendedKeyCode, control, shift);
	}

	/**
	 * The character a key types for the application (see the class comment):
	 * what AWT typed, unless Control turned it into a control character, then
	 * the symbol of the key, in upper case with Shift.
	 *
	 * @param keyChar         the character AWT typed
	 * @param keyCode         the key code of the key
	 * @param extendedKeyCode the extended key code of the key
	 * @param control         whether Control is held
	 * @param shift           whether Shift is held
	 */
	public static char typed(final char keyChar, final int keyCode, final int extendedKeyCode,
			final boolean control, final boolean shift) {
		if (!control || !madeByControl(keyChar)) {
			return keyChar;
		}
		final char symbol = symbolOf(keyCode, extendedKeyCode);
		if (symbol == UNDEFINED) {
			return keyChar;
		}
		return shift ? Character.toUpperCase(symbol) : symbol;
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
