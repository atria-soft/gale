package org.atriasoft.gale;

public class Fps {
	private long startTime = 0;
	private long nbCallTime = 0;
	private long nbDisplayTime = 0;
	private float min = 99999999;
	private float avg = 0;
	private float max = 0;
	private float minIdle = 99999999;
	private float avgIdle = 0;
	private float maxIdle = 0;
	private long ticTime = 0;
	private boolean display = false;
	private boolean drawingDone = false;
	private String displayName = null;
	private boolean displayFPS;
	public Fps(String displayName, boolean displayFPS) {
		this.displayName = displayName;
		this.displayFPS = displayFPS;
	}
	public void tic() {
		long currentTime = System.currentTimeMillis();
		ticTime = currentTime;
		nbCallTime++;
		if (startTime == 0) {
			startTime = currentTime;
		}
		if ( (currentTime - startTime) > 10000) {
			display = true;
		}
	}
	public void toc() {
		toc(false);
	}
	public void toc(boolean displayTime) {
		long currentTime = System.currentTimeMillis();
		long processTimeLocal = (currentTime - ticTime);
		if (displayTime) {
			System.out.println(displayName + ": processTime: " + processTimeLocal);
		}
		if (drawingDone) {
			min = Math.min(min, processTimeLocal);
			max = Math.max(max, processTimeLocal);
			avg += processTimeLocal;
			drawingDone = false;
		} else {
			minIdle = Math.min(minIdle, processTimeLocal);
			maxIdle = Math.max(maxIdle, processTimeLocal);
			avgIdle += processTimeLocal;
		}
	}

	public void incrementCounter() {
		nbDisplayTime++;
		drawingDone = true;
	}

	public void draw() {
		if (display) {
			if (nbDisplayTime > 0) {
				System.out.println(displayName + " : Active : "
				                         + min + " "
				                         + avg / nbDisplayTime + "ms "
				                         + max + " ");
			}
			if (nbCallTime-nbDisplayTime>0) {
				System.out.println(displayName + " : idle   : "
				                         + minIdle + " "
				                         + avgIdle / (nbCallTime-nbDisplayTime) + "ms "
				                         + maxIdle + " ");
			}
			if (displayFPS) {
				System.out.println("FPS : " + nbDisplayTime + "/" + nbCallTime + "fps");
			}
			max = 0;
			min = 99999999;
			avg = 0;
			maxIdle = 0;
			minIdle = 99999999;
			avgIdle = 0;
			nbCallTime = 0;
			nbDisplayTime = 0;
			startTime = 0;
			display = false;
		}
	}
}
