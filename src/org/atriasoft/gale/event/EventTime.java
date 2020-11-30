package org.atriasoft.gale.event;

public class EventTime {
	private long timeSystem; //!< Current system time (micro-second)
	private long timeUpAppl; //!< Current application wake up-time (micro-second)
	private long timeDelta; //!< Time from the last cycle call of the system (main appl tick) (micro-second)
	private long timeDeltaCall; //!< Time from the last call (when we can manage periodic call with specifying periode) (micro-second)
	public EventTime(long timeSystem, long timeUpAppl, long timeDelta, long timeDeltaCall) {
		super();
		this.timeSystem = timeSystem;
		this.timeUpAppl = timeUpAppl;
		this.timeDelta = timeDelta;
		this.timeDeltaCall = timeDeltaCall;
	}
	public long getTimeSystem() {
		return timeSystem;
	}
	public void setTimeSystem(long timeSystem) {
		this.timeSystem = timeSystem;
	}
	public long getTimeUpAppl() {
		return timeUpAppl;
	}
	public void setTimeUpAppl(long timeUpAppl) {
		this.timeUpAppl = timeUpAppl;
	}
	public long getTimeDelta() {
		return timeDelta;
	}
	public float getTimeDeltaSecond() {
		return (float)timeDelta*0.0000001f;
	}
	public void setTimeDelta(long timeDelta) {
		this.timeDelta = timeDelta;
	}
	public long getTimeDeltaCall() {
		return timeDeltaCall;
	}
	public float getTimeDeltaCallSecond() {
		return (float)timeDeltaCall*0.0000001f;
	}
	public void setTimeDeltaCall(long timeDeltaCall) {
		this.timeDeltaCall = timeDeltaCall;
	}
	@Override
	public String toString() {
		return "EventTime [timeSystem=" + timeSystem + "us, timeUpAppl=" + timeUpAppl + "us, timeDelta=" + timeDelta
				+ "us, timeDeltaCall=" + timeDeltaCall + "us]";
	}

}
