package org.atriasoft.gale.context;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.util.ArrayList;
import java.util.List;

import org.atriasoft.etk.math.Vector2f;
import org.atriasoft.gale.GaleApplication;
import org.atriasoft.gale.key.KeyKeyboard;
import org.atriasoft.gale.key.KeySpecial;
import org.atriasoft.gale.key.KeyStatus;
import org.atriasoft.gale.key.KeyType;
import org.junit.jupiter.api.Test;

/** An input event keeps the keys held when it happened, though it is handled later, on the GUI thread. */
class InputModifiersTest {

	/** Records the keys held of each event it receives. */
	private static final class RecordingApplication extends GaleApplication {
		private final List<KeySpecial> keyboard = new ArrayList<>();
		private final List<KeySpecial> pointer = new ArrayList<>();

		@Override
		public void onKeyboard(final KeySpecial special, final KeyKeyboard type, final Character value,
				final KeyStatus state) {
			this.keyboard.add(special);
		}

		@Override
		public void onPointer(final KeySpecial special, final KeyType type, final int pointerID, final Vector2f pos,
				final KeyStatus state) {
			this.pointer.add(special);
		}
	}

	/** A context without window: the events wait in its queue until {@link #handleEvents()}. */
	private static final class QueueContext extends GaleContext {
		QueueContext(final GaleApplication application) {
			super(application, new String[0]);
		}

		void handleEvents() {
			processEventsGui();
		}

		@Override
		public int run() {
			return 0;
		}
	}

	@Test
	void aLetterTypedWithControlKeepsControlOnceControlIsReleased() {
		final RecordingApplication application = new RecordingApplication();
		final QueueContext context = new QueueContext(application);
		// The backend changes one object of the keys held: Ctrl pressed, Z typed, then Ctrl released.
		final KeySpecial backend = new KeySpecial();
		backend.setCtrlLeft(true);
		context.operatingSystemsetKeyboard(backend, KeyKeyboard.CHARACTER, KeyStatus.down, false, 'z');
		backend.setCtrlLeft(false);
		context.operatingSystemsetKeyboard(backend, KeyKeyboard.CTRL_LEFT, KeyStatus.up, false);
		// Both events are handled after the release of Control.
		context.handleEvents();
		assertEquals(2, application.keyboard.size());
		assertTrue(application.keyboard.get(0).getCtrl(), "the z was typed with Control");
		assertFalse(application.keyboard.get(1).getCtrl(), "Control released");
	}

	@Test
	void aWheelTurnedWithControlKeepsControlOnceControlIsReleased() {
		final RecordingApplication application = new RecordingApplication();
		final QueueContext context = new QueueContext(application);
		final KeySpecial backend = new KeySpecial();
		backend.setCtrlLeft(true);
		context.operatingSystemSetInput(backend, KeyType.mouse, KeyStatus.down, 4, Vector2f.ZERO);
		backend.setCtrlLeft(false);
		context.handleEvents();
		assertEquals(1, application.pointer.size());
		assertTrue(application.pointer.get(0).getCtrl(), "the wheel was turned with Control");
	}

	@Test
	void aCopyHoldsTheSameKeys() {
		final KeySpecial keys = new KeySpecial();
		keys.setShiftRight(true);
		keys.setAltRight(true);
		keys.setCapsLock(true);
		keys.setNumLock(true);
		keys.setInsert(true);
		keys.setMetaLeft(true);
		keys.setCtrlRight(true);
		assertEquals(keys.toString(), new KeySpecial(keys).toString());
	}
}
