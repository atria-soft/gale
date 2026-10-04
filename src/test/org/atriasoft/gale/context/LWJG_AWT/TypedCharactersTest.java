package org.atriasoft.gale.context.LWJG_AWT;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.awt.event.KeyEvent;
import java.util.HashSet;
import java.util.Set;

import org.junit.jupiter.api.Test;

/** The character of a key held with Control, Shift or AltGr, without any window (the key codes AWT gives). */
class TypedCharactersTest {

	/** Extended key code AWT gives to a symbol without a key code of its own. */
	private static int unicode(final char symbol) {
		return 0x01000000 + symbol;
	}

	@Test
	void aLetterHeldWithControlStaysTheLetterOfTheLayout() {
		// QWERTY: the key W; AZERTY: the key left of E bears Z and AWT names it VK_Z.
		assertEquals('w', TypedCharacters.typed('\u0017', KeyEvent.VK_W, KeyEvent.VK_W, true, false));
		assertEquals('z', TypedCharacters.typed('\u001a', KeyEvent.VK_Z, KeyEvent.VK_Z, true, false));
		assertEquals('q', TypedCharacters.typed('\u0011', KeyEvent.VK_Q, KeyEvent.VK_Q, true, false));
		assertEquals('d', TypedCharacters.typed('\u0004', KeyEvent.VK_D, KeyEvent.VK_D, true, false));
	}

	@Test
	void controlLettersThatAreAlsoKeysOfTheirOwnStayLetters() {
		// Ctrl+I types a Tab, Ctrl+M a Return, Ctrl+H a Backspace, Ctrl+[ an Escape.
		assertEquals('i', TypedCharacters.typed('\t', KeyEvent.VK_I, KeyEvent.VK_I, true, false));
		assertEquals('m', TypedCharacters.typed('\r', KeyEvent.VK_M, KeyEvent.VK_M, true, false));
		assertEquals('h', TypedCharacters.typed('\b', KeyEvent.VK_H, KeyEvent.VK_H, true, false));
		assertEquals('[', TypedCharacters.typed('\u001b', KeyEvent.VK_OPEN_BRACKET, KeyEvent.VK_OPEN_BRACKET, true,
				false));
		assertEquals('2', TypedCharacters.typed('\0', KeyEvent.VK_2, KeyEvent.VK_2, true, false));
	}

	@Test
	void theKeysThatTypeAControlCharacterKeepIt() {
		assertEquals('\n', TypedCharacters.typed('\n', KeyEvent.VK_ENTER, KeyEvent.VK_ENTER, true, false));
		assertEquals('\b', TypedCharacters.typed('\b', KeyEvent.VK_BACK_SPACE, KeyEvent.VK_BACK_SPACE, true, false));
		assertEquals('\u001b', TypedCharacters.typed('\u001b', KeyEvent.VK_ESCAPE, KeyEvent.VK_ESCAPE, false, false));
		assertEquals('\u001b', TypedCharacters.typed('\u001b', KeyEvent.VK_ESCAPE, KeyEvent.VK_ESCAPE, true, false));
	}

	@Test
	void shiftAndControlTypeTheCapitalLetter() {
		assertEquals('W', TypedCharacters.typed('\u0017', KeyEvent.VK_W, KeyEvent.VK_W, true, true));
		// Shift alone: what AWT typed (the sprint held with a key of movement).
		assertEquals('W', TypedCharacters.typed('W', KeyEvent.VK_W, KeyEvent.VK_W, false, true));
		assertEquals('Z', TypedCharacters.typed('Z', KeyEvent.VK_Z, KeyEvent.VK_Z, false, true));
	}

	@Test
	void altGrTypesTheThirdSymbolOfTheKey() {
		// AZERTY: AltGr+E types the euro sign, AltGr+à an at sign; AltGr does not hold Control on X11.
		assertEquals('€', TypedCharacters.typed('€', KeyEvent.VK_E, KeyEvent.VK_E, false, false));
		assertEquals('@', TypedCharacters.typed('@', KeyEvent.VK_UNDEFINED, unicode('à'), false, false));
		// Windows makes AltGr out of Control and Alt: the symbol typed is no control character, it is kept.
		assertEquals('€', TypedCharacters.typed('€', KeyEvent.VK_E, KeyEvent.VK_E, true, false));
	}

	@Test
	void aSymbolWithoutKeyCodeComesFromItsExtendedCode() {
		// AZERTY: the key 2 of the top row bears é, which has no key code; with Control AWT may type nothing.
		assertEquals('é', TypedCharacters.typed(KeyEvent.CHAR_UNDEFINED, KeyEvent.VK_UNDEFINED,
				unicode('é'), true, false));
		assertEquals('&', TypedCharacters.typed('\0', KeyEvent.VK_AMPERSAND, KeyEvent.VK_AMPERSAND, true, false));
		// An arrow types no symbol: nothing to rebuild.
		assertEquals(KeyEvent.CHAR_UNDEFINED, TypedCharacters.typed(KeyEvent.CHAR_UNDEFINED, KeyEvent.VK_LEFT,
				KeyEvent.VK_LEFT, true, false));
	}

	@Test
	void aKeyIsReleasedWithTheCharacterOfItsPress() {
		final TypedCharacters keys = new TypedCharacters();
		final int standard = KeyEvent.KEY_LOCATION_STANDARD;
		// Z pressed alone, then Control (crouch), then Z released: AWT types Ctrl+Z, the release is still a z.
		assertEquals('z', keys.pressed('z', KeyEvent.VK_Z, KeyEvent.VK_Z, standard, false, false));
		assertEquals('z', keys.released('\u001a', KeyEvent.VK_Z, KeyEvent.VK_Z, standard, true, false));
		// E pressed alone, then AltGr, then E released: AWT types the euro sign, the release is still an e.
		assertEquals('e', keys.pressed('e', KeyEvent.VK_E, KeyEvent.VK_E, standard, false, false));
		assertEquals('e', keys.released('€', KeyEvent.VK_E, KeyEvent.VK_E, standard, false, false));
		// A release without its press (the window got the focus meanwhile): the character it types now.
		assertEquals('w', keys.released('\u0017', KeyEvent.VK_W, KeyEvent.VK_W, standard, true, false));
	}

	@Test
	void keysWithoutKeyCodeAreReleasedEachWithItsOwnCharacter() {
		// AZERTY: é, è, ç, à, ù and ² have no key code (VK_UNDEFINED), each its own extended key code.
		final TypedCharacters keys = new TypedCharacters();
		final int standard = KeyEvent.KEY_LOCATION_STANDARD;
		final int undefined = KeyEvent.VK_UNDEFINED;
		// é pressed, è pressed, é released (with Control, AWT types nothing): é, not è, is released.
		assertEquals('é', keys.pressed('é', undefined, unicode('é'), standard, false, false));
		assertEquals('è', keys.pressed('è', undefined, unicode('è'), standard, false, false));
		assertEquals('é', keys.released(KeyEvent.CHAR_UNDEFINED, undefined, unicode('é'), standard, true, false));
		// Then è released with Shift (AWT types the 7 of the key): still è.
		assertEquals('è', keys.released('7', undefined, unicode('è'), standard, false, true));
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
