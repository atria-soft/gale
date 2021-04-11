/** @file
 * @author Edouard DUPIN
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license MPL v2.0 (see license file)
 */
package org.atriasoft.gale.resource;

import java.util.Arrays;

import org.atriasoft.etk.Color;
import org.atriasoft.etk.math.Vector2f;
import org.atriasoft.etk.math.Vector3f;
import org.atriasoft.gale.backend3d.OpenGL;
import org.atriasoft.gale.backend3d.OpenGL.Usage;
import org.atriasoft.gale.internal.Log;

/**
 * ResourceVirtualBufferObject is a specific resources for opengl, this load the data directly in the graphic card ad keep these inside
 */
public class ResourceVirtualBufferObject extends Resource {
	
	public static ResourceVirtualBufferObject create(final int count) {
		return new ResourceVirtualBufferObject(count);
	}
	
	private boolean exist = false; //!< This data is availlable in the Graphic card
	private final int[] vbo; //!< openGl ID of this VBO
	private final Object[] buffer; //!< data that is availlable in the VBO system ...
	
	/**
	 * Constructor of this VBO.
	 * @param accesMode Acces mode : ???
	 */
	protected ResourceVirtualBufferObject(final int number) {
		super();
		this.vbo = new int[number]; // 0
		this.buffer = new Object[number];
		Log.debug("OGL : load VBO count=\"" + number + "\"");
		this.resourceLevel = 3;
	}
	
	public int bufferSize(final int vboidcoord) {
		if (this.buffer[vboidcoord] != null) {
			// select the buffer to set data inside it ...
			if (this.buffer[vboidcoord] instanceof float[]) {
				return ((float[]) (this.buffer[vboidcoord])).length;
			} else if (this.buffer[vboidcoord] instanceof int[]) {
				return ((int[]) (this.buffer[vboidcoord])).length;
			} else if (this.buffer[vboidcoord] instanceof Vector2f[]) {
				return ((Vector2f[]) (this.buffer[vboidcoord])).length;
			} else if (this.buffer[vboidcoord] instanceof Vector3f[]) {
				return ((Vector3f[]) (this.buffer[vboidcoord])).length;
			} else if (this.buffer[vboidcoord] instanceof Color[]) {
				return ((Color[]) (this.buffer[vboidcoord])).length;
			} else {
				Log.error("Not managed VBO model : " + this.buffer[vboidcoord].getClass().getCanonicalName());
			}
		}
		return 0;
	}
	
	@Override
	public void cleanUp() {
		// TODO Auto-generated method stub
	}
	
	/**
	 * clear buffers
	 */
	public void clear() {
		Log.verbose(" Clear: [" + getId() + "] '" + getName() + "' (size=" + this.buffer.length + ")");
		// DO not clear the this.vbo indexed in the graphic cards ...
		Arrays.fill(this.buffer, null);
	}
	
	/**
	 * Send the data to the graphic card.
	 */
	public synchronized void flush() {
		// request to the manager to be call at the next update ...
		getManager().update(this);
		Log.verbose("Request flush of VBO: [" + getId() + "] '" + getName() + "'");
	}
	
	public int getElementSize(final int index) {
		if (this.buffer[index] != null) {
			// select the buffer to set data inside it ...
			if (this.buffer[index] instanceof float[]) {
				return 1;
			} else if (this.buffer[index] instanceof int[]) {
				return 1;
			} else if (this.buffer[index] instanceof Vector2f[]) {
				return 2;
			} else if (this.buffer[index] instanceof Vector3f[]) {
				return 3;
			} else if (this.buffer[index] instanceof Color[]) {
				return 4;
			} else {
				Log.error("Not managed VBO model : " + this.buffer[index].getClass().getCanonicalName());
			}
		}
		return 1;
	}
	
	/**
	 * get the real openGL ID.
	 * @return the Ogl id reference of this VBO.
	 */
	public int getOpenGlId(final int id) {
		return this.vbo[id];
	}
	
	/**
	 * Relode the shader from the file. used when a request of resouces reload is done.
	 * @note this is really usefull when we tested the new themes or shader developpements.
	 */
	@Override
	public synchronized void reload() {
		removeContext();
		updateContext();
	}
	
	/**
	 * remove the data from the opengl context.
	 */
	@Override
	public synchronized void removeContext() {
		if (this.exist) {
			OpenGL.deleteBuffers(this.vbo);
			this.exist = false;
		}
	}
	
	/**
	 * Special android spec! It inform us that all context is removed and after notify us...
	 */
	@Override
	public synchronized void removeContextToLate() {
		this.exist = false;
		Arrays.fill(this.vbo, 0);
	}
	
	/**
	 * get the data from the graphic card.
	 */
	public void retreiveData() {
		Log.error("TODO ... ");
	}
	
	public void setVboData(final int vboId, final Color[] data) {
		this.buffer[vboId] = data;
	}
	
	public void setVboData(final int vboId, final float[] data) {
		this.buffer[vboId] = data;
	}
	
	public void setVboData(final int vboId, final int[] data) {
		this.buffer[vboId] = data;
	}
	
	public void setVboData(final int vboId, final Vector2f[] data) {
		this.buffer[vboId] = data;
	}
	
	public void setVboData(final int vboId, final Vector3f[] data) {
		this.buffer[vboId] = data;
	}
	
	/**
	 * This load/reload the data in the opengl context, needed when removed previously.
	 */
	@Override
	public synchronized boolean updateContext() {
		Log.warning("updateContext (VBO Start: [" + getId() + "] '" + getName() + "' (size=" + this.buffer.length + ")");
		/*
		if (lock.tryLock() == false) {
			//Lock error ==> try later ...
			Log.warning("     ==> Lock error on VBO");
			return false;
		}
		*/
		if (!this.exist) {
			Log.debug("     ==> ALLOCATE new handle");
			// Allocate and assign a Vertex Array Object to our handle
			OpenGL.genBuffers(this.vbo);
		}
		this.exist = true;
		for (int iii = 0; iii < this.vbo.length; iii++) {
			if (this.buffer[iii] != null) {
				Log.verbose("VBO    : add [" + getId() + "]=" + this.buffer[iii].getClass().getCanonicalName() + "*sizeof(float) OGl_Id=" + this.vbo[iii]);
				OpenGL.bindBuffer(this.vbo[iii]);
				// select the buffer to set data inside it ...
				if (this.buffer[iii] instanceof float[]) {
					OpenGL.bufferData((float[]) (this.buffer[iii]), Usage.streamDraw);
				} else if (this.buffer[iii] instanceof int[]) {
					OpenGL.bufferData((int[]) (this.buffer[iii]), Usage.streamDraw);
				} else if (this.buffer[iii] instanceof Vector2f[]) {
					OpenGL.bufferData((Vector2f[]) (this.buffer[iii]), Usage.streamDraw);
				} else if (this.buffer[iii] instanceof Vector3f[]) {
					OpenGL.bufferData((Vector3f[]) (this.buffer[iii]), Usage.streamDraw);
				} else if (this.buffer[iii] instanceof Color[]) {
					OpenGL.bufferData((Color[]) (this.buffer[iii]), Usage.streamDraw);
				} else {
					Log.error("Not managed VBO model : " + this.buffer[iii].getClass().getCanonicalName());
				}
			}
		}
		// un-bind it to permet to have no error in the next display ...
		OpenGL.unbindBuffer();
		Log.verbose(" Stop: [" + getId() + "] '" + getName() + "'");
		return true;
	}
}
