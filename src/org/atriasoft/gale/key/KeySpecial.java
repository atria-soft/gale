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
	 * @brief Main ructor
	 */
	public KeySpecial() {
		
	}
	/**
	 * @brief get the current CapLock Status
	 * @return The CapLock value
	 */
	public boolean getCapsLock() {
		return valueCapLock;
	}
	/**
	 * @brief set the current CapLock Status
	 * @param value The new CapLock value
	 */
	public void setCapsLock(boolean value) {
		valueCapLock = value;
	}
	/**
	 * @brief Get the current Shift key status
	 * @return The Shift value
	 */
	public boolean getShift() {
		return valueShiftLeft || valueShiftRight;
	}
	/**
	 * @brief Get the current Shift left key status
	 * @return The Shift value
	 */
	public boolean getShiftLeft() {
		return valueShiftLeft;
	}
	/**
	 * @brief Get the current Shift right key status
	 * @return The Shift value
	 */
	public boolean getShiftRight() {
		return valueShiftRight;
	}
	/**
	 * @brief Set the current Shift left key status
	 * @param value The new Shift value
	 */
	public void setShiftLeft(boolean value) {
		valueShiftLeft = value;
	}
	/**
	 * @brief Set the current Shift right key status
	 * @param value The new Shift value
	 */
	public void setShiftRight(boolean value) {
		valueShiftRight = value;
	}
	/**
	 * @brief Get the Current Control key status
	 * @return The Control value
	 */
	public boolean getCtrl() {
		return valueCtrlLeft || valueCtrlRight;
	}
	/**
	 * @brief Get the Current Control left key status
	 * @return The Control value
	 */
	public boolean getCtrlLeft() {
		return valueCtrlLeft;
	}
	/**
	 * @brief Get the Current Control right key status
	 * @return The Control value
	 */
	public boolean getCtrlRight() {
		return valueCtrlRight;
	}
	/**
	 * @brief Set the Current Control left key status
	 * @param value The new Control value
	 */
	public void setCtrlLeft(boolean value){
		valueCtrlLeft = value;
	}
	/**
	 * @brief Set the Current Control right key status
	 * @param value The new Control value
	 */
	public void setCtrlRight(boolean value) {
		valueCtrlRight = value;
	}
	/**
	 * @brief Get the current Meta key status (also named windows or apple key)
	 * @return The Meta value (name Windows key, apple key, command key ...)
	 */
	public boolean getMeta() {
		return valueMetaLeft || valueMetaRight;
	}
	/**
	 * @brief Get the current Meta left key status (also named windows or apple key)
	 * @return The Meta value (name Windows key, apple key, command key ...)
	 */
	public boolean getMetaLeft() {
		return valueMetaLeft;
	}
	/**
	 * @brief Get the current Meta right key status (also named windows or apple key)
	 * @return The Meta value (name Windows key, apple key, command key ...)
	 */
	public boolean getMetaRight() {
		return valueMetaRight;
	}
	/**
	 * @brief Set the current Meta left key status (also named windows or apple key)
	 * @param value The new Meta value (name Windows key, apple key, command key ...)
	 */
	public void setMetaLeft(boolean value) {
		valueMetaLeft = value;
	}
	/**
	 * @brief Set the current Meta right key status (also named windows or apple key)
	 * @param value The new Meta value (name Windows key, apple key, command key ...)
	 */
	public void setMetaRight(boolean value) {
		valueMetaRight = value;
	}
	/**
	 * @brief Get the current Alt key status
	 * @return The Alt value
	 */
	public boolean getAlt() {
		return valueAltLeft || valueAltRight;
	}
	/**
	 * @brief Get the current Alt left key status
	 * @return The Alt value
	 */
	public boolean getAltLeft() {
		return valueAltLeft;
	}
	/**
	 * @brief Get the current Alt right key status (alt-gr)
	 * @return The Alt value
	 */
	public boolean getAltRight() {
		return valueAltRight;
	}
	/**
	 * @brief Set the current Alt left key status
	 * @param value The new Alt value
	 */
	public void setAltLeft(boolean value) {
		valueAltLeft = value;
	}
	/**
	 * @brief Set the current Alt right key status (alt-gr)
	 * @param value The new Alt value
	 */
	public void setAltRight(boolean value) {
		valueAltRight = value;
	}
	/**
	 * @brief Get the current Alt-Gr key status
	 * @return The Alt-gr value (does not exist on MacOs)
	 */
	public boolean getAltGr()  {
		return getAltRight();
	}
	/**
	 * @brief Set the current Alt-Gr key status
	 * @param value The new Alt-gr value (does not exist on MacOs)
	 */
	public void setAltGr(boolean value) {
		setAltRight(value);
	}
	/**
	 * @brief Get the current Ver-num key status
	 * @return The Numerical Lock value
	 */
	public boolean getNumLock()  {
		return valueNumLock;
	}
	/**
	 * @brief Set the current Ver-num key status
	 * @param value The new Numerical Lock value
	 */
	public void setNumLock(boolean value) {
		valueNumLock = value;
	}
	/**
	 * @brief Get the current Intert key status
	 * @return The Insert value
	 */
	public boolean getInsert() {
		return valueInsert;
	}
	/**
	 * @brief Set the current Intert key status
	 * @param value The new Insert value
	 */
	public void setInsert(boolean value) {
		valueInsert = value;
	}
	/**
	 * @brief Update the internal value with the input moving key.
	 * @param move Moving key.
	 * @param isDown The key is pressed or not.
	 */
	public void update(KeyKeyboard move, boolean isDown) {
		switch (move) {
			case insert:
				setInsert(isDown);
				break;
			case capLock:
				setCapsLock(isDown);
				break;
			case shiftLeft:
				setShiftLeft(isDown);
				break;
			case shiftRight:
				setShiftRight(isDown);
				break;
			case ctrlLeft:
				setCtrlLeft(isDown);
				break;
			case ctrlRight:
				setCtrlRight(isDown);
				break;
			case metaLeft:
				setMetaLeft(isDown);
				break;
			case metaRight:
				setMetaRight(isDown);
				break;
			case altLeft:
				setAltLeft(isDown);
				break;
			case altRight:
				setAltRight(isDown);
				break;
			case numLock:
				setNumLock(isDown);
				break;
			default:
				break;
		}
	}
	/**
	 * @brief Get the value with the input moving key.
	 * @param move Moving key.
	 * @return true The key is pressed.
	 * @return false The key is released.
	 */
	public boolean get(KeyKeyboard move) {
		switch (move) {
			case insert:
				return getInsert();
			case capLock:
				return getCapsLock();
			case shiftLeft:
				return getShiftLeft();
			case shiftRight:
				return getShiftRight();
			case ctrlLeft:
				return getCtrlLeft();
			case ctrlRight:
				return getCtrlRight();
			case metaLeft:
				return getMetaLeft();
			case metaRight:
				return getMetaRight();
			case altLeft:
				return getAltLeft();
			case altRight:
				return getAltRight();
			case numLock:
				return getNumLock();
			default:
				break;
		}
		return false;
	}
	@Override
	public String toString() {
		return "Special [CapLock=" + valueCapLock + ", Shift=(" + valueShiftLeft + ","
				+ valueShiftRight + "), Ctrl=(" + valueCtrlLeft + "," + valueCtrlRight
				+ "), Meta=(" + valueMetaLeft + "," + valueMetaRight + "), Alt=("
				+ valueAltLeft + "," + valueAltRight + "), NumLock=" + valueNumLock
				+ ", Insert=" + valueInsert + "]";
	}
}
