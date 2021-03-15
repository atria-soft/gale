package org.atriasoft.gale.key;

public enum KeyStatus {
	unknow,
	down, // availlable on Keyboard too
	downRepeate, // availlable on Keyboard too: the down event us in repeate cycle
	move,
	pressSingle,
	pressDouble,
	pressTriple,
	pressQuad,
	pressQuinte,
	up, // availlable on Keyboard too
	upRepeate, // availlable on Keyboard too: the up event us in repeate cycle
	upAfter, // mouse input & finger input this appear after the single event (depending on some case...)
	enter,
	leave,
	abort, // Appeare when an event is tranfert betwwen widgets (the widget which receive this has lost the events)
	transfert // Appeare when an event is tranfert betwwen widgets (the widget which receive this has receive the transfert of the event)
	;
	
	public static KeyStatus pressCount(final int i) {
		switch (i) {
			case 1:
				return pressSingle;
			case 2:
				return pressDouble;
			case 3:
				return pressTriple;
			case 4:
				return pressQuad;
			case 5:
				return pressQuinte;
		}
		return unknow;
	}
}
