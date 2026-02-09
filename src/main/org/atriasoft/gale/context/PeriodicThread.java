package org.atriasoft.gale.context;

import java.time.Clock;

import org.atriasoft.etk.ThreadAbstract;
import org.atriasoft.gale.GaleApplication;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class PeriodicThread extends ThreadAbstract {
	static final Logger LOGGER = LoggerFactory.getLogger(PeriodicThread.class);
	private final GaleContext context;
	
	public PeriodicThread(final GaleContext context) {
		super("GaleAsync");
		this.context = context;
	}
	
	@Override
	protected void birth() {
		// TODO Auto-generated method stub
	}
	
	@Override
	protected void death() {
		// TODO Auto-generated method stub
	}
	
	@Override
	protected void runPeriodic() {
		//LOGGER.trace("----------------------------- [START] -----------------------------------");
		try {
			Thread.sleep(100);
		} catch (final InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			return;
		}
		// Keep global clock to process events
		final Clock clock = Clock.systemUTC();
		final long time = System.nanoTime();
		
		///synchronized (this.context) {
		this.context.processEventsAsync(clock, time);
		// call all the application for periodic request (the application manage multiple instance )...
		final GaleApplication appl = this.context.getApplication();
		//LOGGER.trace("Call application : " + appl);
		if (appl != null) {
			appl.onPeriod(clock, time);
		}
		//LOGGER.trace("----------------------------- [ END ] -----------------------------------");
	}
}
