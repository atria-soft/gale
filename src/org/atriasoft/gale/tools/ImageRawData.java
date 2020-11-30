package org.atriasoft.gale.tools;

import java.nio.ByteBuffer;

public class ImageRawData {

	private int width;
	private int height;
	private ByteBuffer buffer;
	private boolean hasAlpha;
	
	public ImageRawData(ByteBuffer buffer, int width, int height, boolean hasAlpha){
		this.buffer = buffer;
		this.width = width;
		this.height = height;
		this.hasAlpha = hasAlpha;
	}
	
	public int getWidth(){
		return width;
	}
	
	public int getHeight(){
		return height;
	}
	
	public ByteBuffer getBuffer(){
		return buffer;
	}

	public boolean isHasAlpha() {
		return hasAlpha;
	}
}
