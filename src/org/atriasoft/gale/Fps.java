package org.atriasoft.gale;

public class Fps {
	private long startTime = 0;
	private long nbCallTime = 0;
	private long nbDisplayTime = 0;
	private float min = (float) 99999999999999.0;
	private float avg = 0;
	private float max = 0;
	private float minIdle = (float) 9999999999999.0;
	private float avgIdle = 0;
	private float maxIdle = 0;
	private long ticTime = 0;
	private boolean display = false;
	private boolean drawingDone = false;
	private String displayName = null;
	private final boolean displayFPS;
	public Fps(final String displayName, final boolean displayFPS) {
		this.displayName = displayName;
		this.displayFPS = displayFPS;
	}
	public void tic() {
		long currentTime = System.nanoTime();
		this.ticTime = currentTime;
		this.nbCallTime++;
		if (this.startTime == 0) {
			this.startTime = currentTime;
		}
		if ( (currentTime - this.startTime) > 10000000000L) {
			this.display = true;
		}
	}
	public void toc() {
		toc(false);
	}
	public void toc(final boolean displayTime) {
		long currentTime = System.nanoTime();
		long processTimeLocal = (currentTime - this.ticTime);
		if (displayTime) {
			System.out.println(this.displayName + ": processTime: " + processTimeLocal);
		}
		if (this.drawingDone) {
			this.min = Math.min(this.min, processTimeLocal);
			this.max = Math.max(this.max, processTimeLocal);
			this.avg += processTimeLocal;
			this.drawingDone = false;
		} else {
			this.minIdle = Math.min(this.minIdle, processTimeLocal);
			this.maxIdle = Math.max(this.maxIdle, processTimeLocal);
			this.avgIdle += processTimeLocal;
		}
	}

	public void incrementCounter() {
		this.nbDisplayTime++;
		this.drawingDone = true;
	}

	public void draw() {
		if (this.display) {
			if (this.nbDisplayTime > 0) {
				System.out.println(this.displayName + " : Active : "
				                         + this.min + " "
				                         + this.avg / this.nbDisplayTime + "ms "
				                         + this.max + " ");
			}
			if (this.nbCallTime-this.nbDisplayTime>0) {
				System.out.println(this.displayName + " : idle   : "
				                         + this.minIdle + " "
				                         + this.avgIdle / (this.nbCallTime-this.nbDisplayTime) + "ms "
				                         + this.maxIdle + " ");
			}
			if (this.displayFPS) {
				System.out.println("FPS : " + this.nbDisplayTime + "/" + this.nbCallTime + "fps");
			}
			this.max = 0;
			this.min = 99999999;
			this.avg = 0;
			this.maxIdle = 0;
			this.minIdle = 99999999;
			this.avgIdle = 0;
			this.nbCallTime = 0;
			this.nbDisplayTime = 0;
			this.startTime = 0;
			this.display = false;
		}
	}
}
