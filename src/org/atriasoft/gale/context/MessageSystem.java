package org.atriasoft.gale.context;

import java.util.Vector;

public class MessageSystem {
	private final Vector<ActionToDoInAsyncLoop> data = new Vector<>();
	
	public synchronized void addElement(final ActionToDoInAsyncLoop data2) {
		this.data.addElement(data2);
		notifyAll();
	}
	
	public synchronized ActionToDoInAsyncLoop getElement() {
		//Log.warning("+++++++++++++++++++++++++++++++++ getElement()");
		final ActionToDoInAsyncLoop message = this.data.firstElement();
		this.data.removeElement(message);
		//Log.warning("+++++++++++++++++++++++++++++++++ getElement() ===> done " + message);
		return message;
	}
	
	public synchronized ActionToDoInAsyncLoop getElementWait() {
		if (this.data.isEmpty()) {
			try {
				wait();
			} catch (final InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
				return null;
			}
		}
		if (this.data.isEmpty()) {
			return null;
		}
		return getElement();
	}
	
	public synchronized int getSize() {
		return this.data.size();
	}
}