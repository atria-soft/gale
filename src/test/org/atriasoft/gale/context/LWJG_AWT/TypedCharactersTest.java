package org.atriasoft.gale.context.LWJG_AWT;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.awt.event.InputEvent;
import java.awt.event.KeyEvent;
import java.util.HashSet;
import java.util.Set;

import org.junit.jupiter.api.Test;

/** The character of a key held with Control, Shift or AltGr, without any window (the key codes AWT gives). */
class TypedCharactersTest {

	private static final int NONE = 0;
	private static final int CTRL = InputEvent.CTRL_DOWN_MASK;
	private static final int SHIFT = InputEvent.SHIFT_DOWN_MASK;
	private static final int ALT = InputEvent.ALT_DOWN_MASK;
	private static final int ALT_GRAPH = InputEvent.ALT_GRAPH_DOWN_MASK;

	/** Extended key code AWT gives to a symbol without a key code of its own. */
	private static int unicode(final char symbol) {
		return 0x01000000 + symbol;
	}

	@Test
	void aLetterHeldWithControlStaysTheLetterOfTheLayout() {
		// QWERTY: the key W; AZERTY: the key left of E bears Z and AWT names it VK_Z.
		assertEquals('w', TypedCharacters.typed('\u0017', KeyEvent.VK_W, KeyEvent.VK_W, CTRL));
		assertEquals('z', TypedCharacters.typed('\u001a', KeyEvent.VK_Z, KeyEvent.VK_Z, CTRL));
		assertEquals('q', TypedCharacters.typed('\u0011', KeyEvent.VK_Q, KeyEvent.VK_Q, CTRL));
		assertEquals('d', TypedCharacters.typed('\u0004', KeyEvent.VK_D, KeyEvent.VK_D, CTRL));
	}

	@Test
	void controlLettersThatAreAlsoKeysOfTheirOwnStayLetters() {
		// Ctrl+I types a Tab, Ctrl+M a Return, Ctrl+H a Backspace, Ctrl+[ an Escape.
		assertEquals('i', TypedCharacters.typed('\t', KeyEvent.VK_I, KeyEvent.VK_I, CTRL));
		assertEquals('m', TypedCharacters.typed('\r', KeyEvent.VK_M, KeyEvent.VK_M, CTRL));
		assertEquals('h', TypedCharacters.typed('\b', KeyEvent.VK_H, KeyEvent.VK_H, CTRL));
		assertEquals('[', TypedCharacters.typed('\u001b', KeyEvent.VK_OPEN_BRACKET, KeyEvent.VK_OPEN_BRACKET, CTRL));
		assertEquals('2', TypedCharacters.typed('\0', KeyEvent.VK_2, KeyEvent.VK_2, CTRL));
	}

	@Test
	void theKeysThatTypeAControlCharacterKeepIt() {
		assertEquals('\n', TypedCharacters.typed('\n', KeyEvent.VK_ENTER, KeyEvent.VK_ENTER, CTRL));
		assertEquals('\b', TypedCharacters.typed('\b', KeyEvent.VK_BACK_SPACE, KeyEvent.VK_BACK_SPACE, CTRL));
		assertEquals('\u001b', TypedCharacters.typed('\u001b', KeyEvent.VK_ESCAPE, KeyEvent.VK_ESCAPE, NONE));
		assertEquals('\u001b', TypedCharacters.typed('\u001b', KeyEvent.VK_ESCAPE, KeyEvent.VK_ESCAPE, CTRL));
	}

	@Test
	void shiftAndControlTypeTheCapitalLetter() {
		assertEquals('W', TypedCharacters.typed('\u0017', KeyEvent.VK_W, KeyEvent.VK_W, CTRL | SHIFT));
		// Shift alone: what AWT typed (the sprint held with a key of movement).
		assertEquals('W', TypedCharacters.typed('W', KeyEvent.VK_W, KeyEvent.VK_W, SHIFT));
		assertEquals('Z', TypedCharacters.typed('Z', KeyEvent.VK_Z, KeyEvent.VK_Z, SHIFT));
	}

	@Test
	void altGrTypesTheThirdSymbolOfTheKey() {
		// X11, AZERTY: AltGr (no Control) on E types the euro sign, on à an at sign; the extended key code is the
		// one of the symbol typed.
		assertEquals('€', TypedCharacters.typed('€', KeyEvent.VK_E, KeyEvent.VK_EURO_SIGN, ALT_GRAPH));
		assertEquals('@', TypedCharacters.typed('@', KeyEvent.VK_UNDEFINED, KeyEvent.VK_AT, ALT_GRAPH));
		// Windows: AltGr comes with Control and Alt.
		assertEquals('€', TypedCharacters.typed('€', KeyEvent.VK_E, KeyEvent.VK_E, CTRL | ALT | ALT_GRAPH));
	}

	@Test
	void withAltOrAltGrControlRebuildsNothing() {
		// Windows, AZERTY: AltGr+A has no third symbol and types nothing; it is no a.
		assertEquals(KeyEvent.CHAR_UNDEFINED, TypedCharacters.typed(KeyEvent.CHAR_UNDEFINED, KeyEvent.VK_A,
				KeyEvent.VK_A, CTRL | ALT | ALT_GRAPH));
		// X11: a real Control+Alt+T keeps the control character AWT typed.
		assertEquals('\u0014', TypedCharacters.typed('\u0014', KeyEvent.VK_T, KeyEvent.VK_T, CTRL | ALT));
		// X11: Control and AltGr held on E, the euro sign.
		assertEquals('€', TypedCharacters.typed('€', KeyEvent.VK_E, KeyEvent.VK_EURO_SIGN, CTRL | ALT_GRAPH));
	}

	@Test
	void aSymbolWithoutKeyCodeComesFromItsExtendedCode() {
		// AZERTY: the key 2 of the top row bears é, which has no key code; with Control AWT may type nothing.
		assertEquals('é', TypedCharacters.typed(KeyEvent.CHAR_UNDEFINED, KeyEvent.VK_UNDEFINED,
				unicode('é'), CTRL));
		assertEquals('&', TypedCharacters.typed('\0', KeyEvent.VK_AMPERSAND, KeyEvent.VK_AMPERSAND, CTRL));
		// An arrow types no symbol: nothing to rebuild.
		assertEquals(KeyEvent.CHAR_UNDEFINED, TypedCharacters.typed(KeyEvent.CHAR_UNDEFINED, KeyEvent.VK_LEFT,
				KeyEvent.VK_LEFT, CTRL));
	}

	@Test
	void aKeyIsReleasedWithTheCharacterOfItsPress() {
		final TypedCharacters keys = new TypedCharacters();
		final int standard = KeyEvent.KEY_LOCATION_STANDARD;
		// Z pressed alone, then Control (crouch), then Z released: AWT types Ctrl+Z, the release is still a z.
		assertEquals('z', keys.pressed('z', KeyEvent.VK_Z, KeyEvent.VK_Z, standard, NONE));
		assertEquals('z', keys.released('\u001a', KeyEvent.VK_Z, KeyEvent.VK_Z, standard, CTRL));
		// E pressed alone, then AltGr, then E released: AWT types the euro sign, the release is still an e.
		assertEquals('e', keys.pressed('e', KeyEvent.VK_E, KeyEvent.VK_E, standard, NONE));
		assertEquals('e', keys.released('€', KeyEvent.VK_E, KeyEvent.VK_E, standard, NONE));
		// A release without its press (the window got the focus meanwhile): the character it types now.
		assertEquals('w', keys.released('\u0017', KeyEvent.VK_W, KeyEvent.VK_W, standard, CTRL));
	}

	@Test
	void keysWithoutKeyCodeAreReleasedEachWithItsOwnCharacter() {
		// AZERTY: é, è, ç, à, ù and ² have no key code (VK_UNDEFINED), each its own extended key code.
		final TypedCharacters keys = new TypedCharacters();
		final int standard = KeyEvent.KEY_LOCATION_STANDARD;
		final int undefined = KeyEvent.VK_UNDEFINED;
		// é pressed, è pressed, é released (with Control, AWT types nothing): é, not è, is released.
		assertEquals('é', keys.pressed('é', undefined, unicode('é'), standard, NONE));
		assertEquals('è', keys.pressed('è', undefined, unicode('è'), standard, NONE));
		assertEquals('é', keys.released(KeyEvent.CHAR_UNDEFINED, undefined, unicode('é'), standard, CTRL));
		// Then è released with Shift (AWT types the 7 of the key): still è.
		assertEquals('è', keys.released('7', undefined, unicode('è'), standard, SHIFT));
	}

	@Test
	void eachKeyHasItsOwnNumber() {
		final int standard = KeyEvent.KEY_LOCATION_STANDARD;
		final int undefined = KeyEvent.VK_UNDEFINED;
		final Set<Integer> numbers = new HashSet<>();
		for (final char symbol : new char[] { 'é', 'è', 'ç', 'à', 'ù', '²' }) {
			assertTrue(numbers.add(TypedCharacters.keyOf(undefined, unicode(symbol), standard)), "key " + symbol);
		}
		assertTrue(numbers.add(TypedCharacters.keyOf(KeyEvent.VK_Z, KeyEvent.VK_Z, standard)));
		// Left and right Shift, the digit 1 of the top row and of the keypad.
		assertTrue(numbers.add(TypedCharacters.keyOf(KeyEvent.VK_SHIFT, KeyEvent.VK_SHIFT, KeyEvent.KEY_LOCATION_LEFT)));
		assertTrue(numbers.add(TypedCharacters.keyOf(KeyEvent.VK_SHIFT, KeyEvent.VK_SHIFT, KeyEvent.KEY_LOCATION_RIGHT)));
		assertTrue(numbers.add(TypedCharacters.keyOf(KeyEvent.VK_1, KeyEvent.VK_1, standard)));
		assertTrue(numbers.add(TypedCharacters.keyOf(KeyEvent.VK_1, KeyEvent.VK_1, KeyEvent.KEY_LOCATION_NUMPAD)));
		// The same key at its press and at its release; a key event built by hand has no extended key code.
		assertEquals(TypedCharacters.keyOf(undefined, unicode('é'), standard),
				TypedCharacters.keyOf(undefined, unicode('é'), standard));
		assertEquals(TypedCharacters.keyOf(KeyEvent.VK_Z, KeyEvent.VK_Z, standard),
				TypedCharacters.keyOf(KeyEvent.VK_Z, undefined, standard));
	}
}
