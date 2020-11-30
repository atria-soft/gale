package org.atriasoft.gale.event;

import org.atriasoft.gale.key.KeyType;
import org.atriasoft.gale.key.KeyStatus;
import org.atriasoft.etk.math.Vector2f;
import org.atriasoft.gale.key.KeySpecial;

public class EventInput {
	private KeyType type;
	private KeyStatus status;
	private int inputId;
	private Vector2f position;
	private KeySpecial specialKey; //!< input key status (prevent change in time..)
	public EventInput(KeyType type, KeyStatus status, int inputId, Vector2f position, KeySpecial specialKey) {
		this.type = type;
		this.status = status;
		this.inputId = inputId;
		this.position = position;
		this.specialKey = specialKey;
	}
	public KeyType getType() {
		return type;
	}
	public void setType(KeyType type) {
		this.type = type;
	}
	public KeyStatus getStatus() {
		return status;
	}
	public void setStatus(KeyStatus status) {
		this.status = status;
	}
	public int getInputId() {
		return inputId;
	}
	public void setInputId(int inputId) {
		this.inputId = inputId;
	}
	public Vector2f getPosition() {
		return position;
	}
	public void setPosition(Vector2f position) {
		this.position = position;
	}
	public KeySpecial getSpecialKey() {
		return specialKey;
	}
	public void setSpecialKey(KeySpecial specialKey) {
		this.specialKey = specialKey;
	}
	@Override
	public String toString() {
		return "EventInput [type=" + type + ", status=" + status + ", inputId=" + inputId + ", position=" + position
				+ ", specialKey=" + specialKey + "]";
	}
	
}
