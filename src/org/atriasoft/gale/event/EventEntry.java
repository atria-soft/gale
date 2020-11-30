package org.atriasoft.gale.event;

import org.atriasoft.gale.key.KeyKeyboard;
import org.atriasoft.gale.key.KeyStatus;
import org.atriasoft.gale.key.KeySpecial;

public class EventEntry {
	private KeySpecial specialKey; //!< input key status (prevent change in time..)
	private KeyKeyboard type; //!< type of hardware event
	private KeyStatus status; //!< status of hardware event
	private Character unicodeData; //!< Unicode data (in some case)
	public EventEntry(KeySpecial specialKey,
			KeyKeyboard type,
			KeyStatus status,
			Character charValue) {
	  this.type = type;
	  this.status = status;
	  this.specialKey = specialKey;
	  this.unicodeData = charValue;
	}
	public void setType(KeyKeyboard type) {
		this.type = type;
	}
	public KeyKeyboard getType()  {
		return this.type;
	}
	public void setStatus(KeyStatus status) {
		this.status = status;
	}
	public KeyStatus getStatus()  {
		return this.status;
	};
	public void setSpecialKey(KeySpecial specialKey) {
		this.specialKey = specialKey;
	}
	public KeySpecial getSpecialKey()  {
		return this.specialKey;
	}
	public void setChar(Character charValue) {
		this.unicodeData = charValue;
	}
	public Character getChar()  {
		return this.unicodeData;
	}
	@Override
	public String toString() {
		return "EventEntry [type=" + type + ", status=" + status + ", unicodeData="
				+ unicodeData + ", specialKey=" + specialKey + "]";
	}
}
