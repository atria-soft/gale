package org.atriasoft.gale.key;

public enum KeyStatus {
	unknown,
	down, // available on Keyboard too
	downRepeat, // available on Keyboard too: the down event us in repeate cycle
	move,
	pressSingle,
	pressDouble,
	pressTriple,
	pressQuad,
	pressQuint,
	up, // available on Keyboard too
	upRepeat, // available on Keyboard too: the up event us in repeate cycle
	upAfter, // mouse input & finger input this appear after the single event (depending on some case...)
	enter,
	leave,
	abort, // Appear when an event is tranfert betwwen widgets (the widget which receive this has lost the events)
	transfer // Appear when an event is tranfert betwwen widgets (the widget which receive this has receive the transfert of the event)
	;
	
	public static KeyStatus pressCount(final int i) {
		return switch (i) {
			case 1 -> pressSingle;
			case 2 -> pressDouble;
			case 3 -> pressTriple;
			case 4 -> pressQuad;
			case 5 -> pressQuint;
			default -> unknown;
		};
	}
}
