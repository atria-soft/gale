package org.atriasoft.gale.context;

import java.util.HashMap;
import java.util.Map;
import java.util.Vector;

import org.atriasoft.gale.internal.Log;

public class MessageSystem {
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
		//Log.warning("+++++++++++++++++++++++++++++++++ getElement()");
		final Map.Entry<String, ActionToDoInAsyncLoop> entry = this.dataSingle.entrySet().iterator().next();
		final String key = entry.getKey();
		final ActionToDoInAsyncLoop message = entry.getValue();
		this.dataSingle.remove(key);
		//Log.warning("+++++++++++++++++++++++++++++++++ getElement() ===> done " + message);
		return message;
	}
	
	protected synchronized ActionToDoInAsyncLoop getElementVector() {
		//Log.warning("+++++++++++++++++++++++++++++++++ getElement()");
		final ActionToDoInAsyncLoop message = this.data.firstElement();
		this.data.removeElement(message);
		//Log.warning("+++++++++++++++++++++++++++++++++ getElement() ===> done " + message);
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
	
	public synchronized int getSize() {
		Log.verbose("------------------------------------------------------------");
		Log.verbose("-- nb message: {} + {}", this.data.size(), this.dataSingle.size());
		Log.verbose("------------------------------------------------------------");
		return this.data.size() + this.dataSingle.size();
	}
}