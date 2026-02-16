package org.atriasoft.gale.context;

import java.util.HashMap;
import java.util.Map;
import java.util.Vector;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class MessageSystem {
	static final Logger LOGGER = LoggerFactory.getLogger(MessageSystem.class);
	private final Vector<ActionToDoInAsyncLoop> data = new Vector<>();
	private final Map<String, ActionToDoInAsyncLoop> dataSingle = new HashMap<>();

	public synchronized void addElement(final ActionToDoInAsyncLoop data2) {
		this.data.addElement(data2);
		notifyAll();
	}

	public synchronized void addElement(final String uniqueID, final ActionToDoInAsyncLoop data2) {
		this.dataSingle.put(uniqueID, data2);
		notifyAll();
	}

	protected synchronized ActionToDoInAsyncLoop getElementSingle() {
		//LOGGER.warn("+++++++++++++++++++++++++++++++++ getElement()");
		final Map.Entry<String, ActionToDoInAsyncLoop> entry = this.dataSingle.entrySet().iterator().next();
		final String key = entry.getKey();
		final ActionToDoInAsyncLoop message = entry.getValue();
		this.dataSingle.remove(key);
		//LOGGER.warn("+++++++++++++++++++++++++++++++++ getElement() ===> done " + message);
		return message;
	}

	protected synchronized ActionToDoInAsyncLoop getElementVector() {
		//LOGGER.warn("+++++++++++++++++++++++++++++++++ getElement()");
		final ActionToDoInAsyncLoop message = this.data.firstElement();
		this.data.removeElement(message);
		//LOGGER.warn("+++++++++++++++++++++++++++++++++ getElement() ===> done " + message);
		return message;
	}

	public synchronized ActionToDoInAsyncLoop getElementWait() {
		if (this.data.isEmpty() && this.dataSingle.isEmpty()) {
			try {
				wait();
			} catch (final InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
				return null;
			}
		}
		if (!this.data.isEmpty()) {
			return getElementVector();
		}
		if (!this.dataSingle.isEmpty()) {
			return getElementSingle();
		}
		return null;
	}

	/**
	 * Wait until an event arrives or timeout expires.
	 * @param timeoutMs maximum time to wait in milliseconds
	 * @return true if there are events to process, false if timeout expired
	 */
	public synchronized boolean waitForEvent(final long timeoutMs) {
		if (!this.data.isEmpty() || !this.dataSingle.isEmpty()) {
			return true;
		}
		if (timeoutMs <= 0) {
			return false;
		}
		try {
			wait(timeoutMs);
		} catch (final InterruptedException e) {
			Thread.currentThread().interrupt();
			return false;
		}
		return !this.data.isEmpty() || !this.dataSingle.isEmpty();
	}

	public synchronized int getSize() {
		LOGGER.trace("------------------------------------------------------------");
		LOGGER.trace("-- nb message: {} + {}", this.data.size(), this.dataSingle.size());
		LOGGER.trace("------------------------------------------------------------");
		return this.data.size() + this.dataSingle.size();
	}
}