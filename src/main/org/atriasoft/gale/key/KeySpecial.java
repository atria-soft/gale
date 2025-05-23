package org.atriasoft.gale.key;

public class KeySpecial {
	private boolean valueCapLock = false;
	private boolean valueShiftLeft = false;
	private boolean valueShiftRight = false;
	private boolean valueCtrlLeft = false;
	private boolean valueCtrlRight = false;
	private boolean valueMetaLeft = false;
	private boolean valueMetaRight = false;
	private boolean valueAltLeft = false;
	private boolean valueAltRight = false;
	private boolean valueNumLock = false;
	private boolean valueInsert = false;
	
	/**
	 * Main ructor
	 */
	public KeySpecial() {
		
	}
	
	/**
	 * Get the value with the input moving key.
	 * @param move Moving key.
	 * @return true The key is pressed.
	 * @return false The key is released.
	 */
	public boolean get(final KeyKeyboard move) {
		switch (move) {
			case INSERT:
				return getInsert();
			case CAP_LOCK:
				return getCapsLock();
			case SHIFT_LEFT:
				return getShiftLeft();
			case SHIFT_RIGHT:
				return getShiftRight();
			case CTRL_LEFT:
				return getCtrlLeft();
			case CTRL_RIGHT:
				return getCtrlRight();
			case META_LEFT:
				return getMetaLeft();
			case META_RIGHT:
				return getMetaRight();
			case ALT_LEFT:
				return getAltLeft();
			case ALT_RIGHT:
				return getAltRight();
			case NUM_LOCK:
				return getNumLock();
			default:
				break;
		}
		return false;
	}
	
	/**
	 * Get the current Alt key status
	 * @return The Alt value
	 */
	public boolean getAlt() {
		return this.valueAltLeft || this.valueAltRight;
	}
	
	/**
	 * Get the current Alt-Gr key status
	 * @return The Alt-gr value (does not exist on MacOs)
	 */
	public boolean getAltGr() {
		return getAltRight();
	}
	
	/**
	 * Get the current Alt left key status
	 * @return The Alt value
	 */
	public boolean getAltLeft() {
		return this.valueAltLeft;
	}
	
	/**
	 * Get the current Alt right key status (alt-gr)
	 * @return The Alt value
	 */
	public boolean getAltRight() {
		return this.valueAltRight;
	}
	
	/**
	 * get the current CapLock Status
	 * @return The CapLock value
	 */
	public boolean getCapsLock() {
		return this.valueCapLock;
	}
	
	/**
	 * Get the Current Control key status
	 * @return The Control value
	 */
	public boolean getCtrl() {
		return this.valueCtrlLeft || this.valueCtrlRight;
	}
	
	/**
	 * Get the Current Control left key status
	 * @return The Control value
	 */
	public boolean getCtrlLeft() {
		return this.valueCtrlLeft;
	}
	
	/**
	 * Get the Current Control right key status
	 * @return The Control value
	 */
	public boolean getCtrlRight() {
		return this.valueCtrlRight;
	}
	
	/**
	 * Get the current Intert key status
	 * @return The Insert value
	 */
	public boolean getInsert() {
		return this.valueInsert;
	}
	
	/**
	 * Get the current Meta key status (also named windows or apple key)
	 * @return The Meta value (name Windows key, apple key, command key ...)
	 */
	public boolean getMeta() {
		return this.valueMetaLeft || this.valueMetaRight;
	}
	
	/**
	 * Get the current Meta left key status (also named windows or apple key)
	 * @return The Meta value (name Windows key, apple key, command key ...)
	 */
	public boolean getMetaLeft() {
		return this.valueMetaLeft;
	}
	
	/**
	 * Get the current Meta right key status (also named windows or apple key)
	 * @return The Meta value (name Windows key, apple key, command key ...)
	 */
	public boolean getMetaRight() {
		return this.valueMetaRight;
	}
	
	/**
	 * Get the current Ver-num key status
	 * @return The Numerical Lock value
	 */
	public boolean getNumLock() {
		return this.valueNumLock;
	}
	
	/**
	 * Get the current Shift key status
	 * @return The Shift value
	 */
	public boolean getShift() {
		return this.valueShiftLeft || this.valueShiftRight;
	}
	
	/**
	 * Get the current Shift left key status
	 * @return The Shift value
	 */
	public boolean getShiftLeft() {
		return this.valueShiftLeft;
	}
	
	/**
	 * Get the current Shift right key status
	 * @return The Shift value
	 */
	public boolean getShiftRight() {
		return this.valueShiftRight;
	}
	
	/**
	 * Set the current Alt-Gr key status
	 * @param value The new Alt-gr value (does not exist on MacOs)
	 */
	public void setAltGr(final boolean value) {
		setAltRight(value);
	}
	
	/**
	 * Set the current Alt left key status
	 * @param value The new Alt value
	 */
	public void setAltLeft(final boolean value) {
		this.valueAltLeft = value;
	}
	
	/**
	 * Set the current Alt right key status (alt-gr)
	 * @param value The new Alt value
	 */
	public void setAltRight(final boolean value) {
		this.valueAltRight = value;
	}
	
	/**
	 * set the current CapLock Status
	 * @param value The new CapLock value
	 */
	public void setCapsLock(final boolean value) {
		this.valueCapLock = value;
	}
	
	/**
	 * Set the Current Control left key status
	 * @param value The new Control value
	 */
	public void setCtrlLeft(final boolean value) {
		this.valueCtrlLeft = value;
	}
	
	/**
	 * Set the Current Control right key status
	 * @param value The new Control value
	 */
	public void setCtrlRight(final boolean value) {
		this.valueCtrlRight = value;
	}
	
	/**
	 * Set the current Intert key status
	 * @param value The new Insert value
	 */
	public void setInsert(final boolean value) {
		this.valueInsert = value;
	}
	
	/**
	 * Set the current Meta left key status (also named windows or apple key)
	 * @param value The new Meta value (name Windows key, apple key, command key ...)
	 */
	public void setMetaLeft(final boolean value) {
		this.valueMetaLeft = value;
	}
	
	/**
	 * Set the current Meta right key status (also named windows or apple key)
	 * @param value The new Meta value (name Windows key, apple key, command key ...)
	 */
	public void setMetaRight(final boolean value) {
		this.valueMetaRight = value;
	}
	
	/**
	 * Set the current Ver-num key status
	 * @param value The new Numerical Lock value
	 */
	public void setNumLock(final boolean value) {
		this.valueNumLock = value;
	}
	
	/**
	 * Set the current Shift left key status
	 * @param value The new Shift value
	 */
	public void setShiftLeft(final boolean value) {
		this.valueShiftLeft = value;
	}
	
	/**
	 * Set the current Shift right key status
	 * @param value The new Shift value
	 */
	public void setShiftRight(final boolean value) {
		this.valueShiftRight = value;
	}
	
	@Override
	public String toString() {
		return "Special [CapLock=" + this.valueCapLock + ", Shift=(" + this.valueShiftLeft + "," + this.valueShiftRight + "), Ctrl=(" + this.valueCtrlLeft + "," + this.valueCtrlRight + "), Meta=("
				+ this.valueMetaLeft + "," + this.valueMetaRight + "), Alt=(" + this.valueAltLeft + "," + this.valueAltRight + "), NumLock=" + this.valueNumLock + ", Insert=" + this.valueInsert + "]";
	}
	
	/**
	 * Update the internal value with the input moving key.
	 * @param move Moving key.
	 * @param isDown The key is pressed or not.
	 */
	public void update(final KeyKeyboard move, final boolean isDown) {
		switch (move) {
			case INSERT:
				setInsert(isDown);
				break;
			case CAP_LOCK:
				setCapsLock(isDown);
				break;
			case SHIFT_LEFT:
				setShiftLeft(isDown);
				break;
			case SHIFT_RIGHT:
				setShiftRight(isDown);
				break;
			case CTRL_LEFT:
				setCtrlLeft(isDown);
				break;
			case CTRL_RIGHT:
				setCtrlRight(isDown);
				break;
			case META_LEFT:
				setMetaLeft(isDown);
				break;
			case META_RIGHT:
				setMetaRight(isDown);
				break;
			case ALT_LEFT:
				setAltLeft(isDown);
				break;
			case ALT_RIGHT:
				setAltRight(isDown);
				break;
			case NUM_LOCK:
				setNumLock(isDown);
				break;
			default:
				break;
		}
	}
}
