package org.atriasoft.gale.resource;

import java.nio.FloatBuffer;
import java.nio.IntBuffer;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import org.atriasoft.etk.Color;
import org.atriasoft.etk.math.Vector2f;
import org.atriasoft.etk.math.Vector3f;
import org.atriasoft.gale.backend3d.OpenGL;
import org.atriasoft.gale.backend3d.OpenGL.RenderMode;
import org.atriasoft.gale.backend3d.OpenGL.Usage;
import org.atriasoft.gale.internal.Log;
import org.lwjgl.BufferUtils;
import org.lwjgl.opengl.GL11;
import org.lwjgl.opengl.GL15;
import org.lwjgl.opengl.GL20;
import org.lwjgl.opengl.GL30;

//import models.RawModel;

public class ResourceVirtualArrayObject extends Resource {
	public static final int INDICE_VBO_POSITIONS = 0;
	public static final int INDICE_VBO_TEXTURE_COORDINATES = 1;
	public static final int INDICE_VBO_NORMALS = 2;
	public static final int INDICE_VBO_COLORS = 3;
	
	public static int[] convertIntegers(final List<Integer> integers) {
		int[] ret = new int[integers.size()];
		Iterator<Integer> iterator = integers.iterator();
		for (int i = 0; i < ret.length; i++) {
			ret[i] = iterator.next().intValue();
		}
		return ret;
	}
	
	public static ResourceVirtualArrayObject create(final float[] positions, final float[] colors, final float[] textureCoordinates, final float[] normals, final int[] indices) {
		final ResourceVirtualArrayObject resource = new ResourceVirtualArrayObject(positions, colors, textureCoordinates, normals, indices, indices.length);
		getManager().localAdd(resource);
		return resource;
	}
	
	public static ResourceVirtualArrayObject create(final float[] positions, final float[] textureCoordinates, final float[] normals, final int[] indices) {
		final ResourceVirtualArrayObject resource = new ResourceVirtualArrayObject(positions, null, textureCoordinates, normals, indices, indices.length);
		getManager().localAdd(resource);
		return resource;
	}
	
	public static ResourceVirtualArrayObject create(final float[] positions, final float[] colors, final int[] indices) {
		final ResourceVirtualArrayObject resource = new ResourceVirtualArrayObject(positions, colors, null, null, indices, indices.length);
		getManager().localAdd(resource);
		return resource;
	}
	
	public static ResourceVirtualArrayObject create(final float[] positions, final int dimentions) {
		final ResourceVirtualArrayObject resource = new ResourceVirtualArrayObject(positions, null, null, null, null, positions.length / dimentions);
		getManager().localAdd(resource);
		return resource;
	}
	
	public static ResourceVirtualArrayObject createDynamic() {
		final ResourceVirtualArrayObject resource = new ResourceVirtualArrayObject();
		getManager().localAdd(resource);
		return resource;
	}
	
	public static FloatBuffer storeDataInFloatBuffer(final float[] data) {
		final FloatBuffer buffer = BufferUtils.createFloatBuffer(data.length);
		buffer.put(data);
		buffer.flip();
		return buffer;
	}
	
	public static IntBuffer storeDataInIntBuffer(final int[] data) {
		final IntBuffer buffer = BufferUtils.createIntBuffer(data.length);
		buffer.put(data);
		buffer.flip();
		return buffer;
	}
	
	private final boolean dynamic = false;
	private int vaoID = -1;
	private boolean exist = false; //!< This data is availlable in the Graphic card
	
	private final List<Integer> vbo = new ArrayList<>();
	
	Object positions = null;
	
	Object colors = null;
	
	Object textureCoordinates = null;
	
	Object normals = null;
	
	int[] indices = null;
	
	int vertexCount = -1;
	
	protected ResourceVirtualArrayObject() {
		super();
		this.resourceLevel = 3;
		Log.debug("OGL: load VBO count (dynamic)");
	}
	
	protected ResourceVirtualArrayObject(final float[] positions, final float[] colors, final float[] textureCoordinates, final float[] normals, final int[] indices, final int vertexCount) {
		super();
		this.resourceLevel = 3;
		this.positions = positions;
		this.colors = colors;
		this.textureCoordinates = textureCoordinates;
		this.normals = normals;
		this.indices = indices;
		this.vertexCount = vertexCount;
		Log.debug("OGL: load VBO count");
	}
	
	public void bindForRendering() {
		if (!this.exist) {
			return;
		}
		GL30.glBindVertexArray(this.vaoID);
		if (this.positions != null) {
			GL20.glEnableVertexAttribArray(INDICE_VBO_POSITIONS);
			//Log.info("unbind POSITION");
		}
		if (this.textureCoordinates != null) {
			GL20.glEnableVertexAttribArray(INDICE_VBO_TEXTURE_COORDINATES);
		}
		if (this.normals != null) {
			GL20.glEnableVertexAttribArray(INDICE_VBO_NORMALS);
		}
		if (this.colors != null) {
			GL20.glEnableVertexAttribArray(INDICE_VBO_COLORS);
		}
	}
	
	private void bindIndicesBuffer(final int[] indices) {
		final int vboId = OpenGL.genBuffers();
		this.vbo.add(vboId);
		GL15.glBindBuffer(GL15.GL_ELEMENT_ARRAY_BUFFER, vboId);
		final IntBuffer buffer = storeDataInIntBuffer(indices);
		if (this.dynamic) {
			GL15.glBufferData(GL15.GL_ELEMENT_ARRAY_BUFFER, buffer, GL15.GL_DYNAMIC_DRAW);
		} else {
			GL15.glBufferData(GL15.GL_ELEMENT_ARRAY_BUFFER, buffer, GL15.GL_STATIC_DRAW);
		}
	}
	
	/**
	 * Destructor of this VBO.
	 */
	@Override
	public void cleanUp() {
		removeContext();
	}
	
	/**
	 * clear buffers
	 */
	public void clear() {
		//Log.verbose(" Clear: [" + getId() + "] '" + getName() + "' (size=" + buffer.get(0).length + ")");
		this.positions = null;
		this.colors = null;
		this.textureCoordinates = null;
		this.normals = null;
		this.indices = null;
		this.vertexCount = -1;
	}
	
	private void createVAO() {
		Log.error("create VAO...");
		this.vaoID = GL30.glGenVertexArrays();
		GL30.glBindVertexArray(this.vaoID);
	}
	
	/**
	 * Send the data to the graphic card.
	 */
	public void flush() {
		// request to the manager to be call at the next update ...
		getManager().update(this);
		Log.verbose("Request flush of VBO: [" + getId() + "] '" + getName() + "'");
	}
	
	/**
	 * get the real openGL ID.
	 * @return the Ogl id reference of this VBO.
	 */
	public int getGLID() {
		return this.vaoID;
	}
	
	public void loadAgainToVAO() {
		createVAO();
		if (this.indices != null) {
			Log.error("Set indices");
			bindIndicesBuffer(this.indices);
		}
		if (this.positions != null) {
			Log.error("Set positions");
			storeDataInAttributeList(0, 3, this.positions);
		}
		if (this.textureCoordinates != null) {
			Log.error("Set textureCoordinates");
			storeDataInAttributeList(1, 2, this.textureCoordinates);
		}
		if (this.normals != null) {
			Log.error("Set normals");
			storeDataInAttributeList(2, 3, this.normals);
		}
		if (this.colors != null) {
			Log.error("Set colors");
			storeDataInAttributeList(3, 4, this.colors);
		}
		unbindVAO();
	}
	
	public void loadToVAO() {
		GL30.glBindVertexArray(this.vaoID);
		if (this.indices != null) {
			Log.error("Set indices");
			bindIndicesBuffer(this.indices);
		}
		if (this.positions != null) {
			Log.error("Set positions");
			storeDataInAttributeList(0, 3, this.positions);
		}
		if (this.textureCoordinates != null) {
			Log.error("Set textureCoordinates");
			storeDataInAttributeList(1, 2, this.textureCoordinates);
		}
		if (this.normals != null) {
			Log.error("Set normals");
			storeDataInAttributeList(2, 3, this.normals);
		}
		if (this.colors != null) {
			Log.error("Set colors");
			storeDataInAttributeList(3, 4, this.colors);
		}
		unbindVAO();
	}
	
	/**
	 * Relode the shader from the file. used when a request of resouces reload is done.
	 * @note this is really usefull when we tested the new themes or shader developpements.
	 */
	@Override
	public void reload() {
		removeContext();
		updateContext();
	}
	
	/**
	 * remove the data from the opengl context.
	 */
	@Override
	public void removeContext() {
		if (this.exist) {
			// OpenGL.deleteBuffers(this.vbo);
			this.exist = false;
		}
	}
	
	/**
	 * Special android spec! It inform us that all context is removed and after notify us...
	 */
	@Override
	public void removeContextToLate() {
		this.exist = false;
		//		for (int iii=0; iii<this.vbo.length; iii++) {
		//			this.vbo[iii] = 0;
		//		}
	}
	
	public void render(final RenderMode mode) {
		Log.warning("request rendering indices : " + this.vertexCount);
		OpenGL.drawElements(mode, this.vertexCount);
	}
	
	public void renderArrays(final RenderMode mode) {
		Log.warning("request rendering direct : " + this.vertexCount);
		OpenGL.drawArrays(mode, 0, this.vertexCount);
	}
	
	public void setColors(final Object colors) {
		this.colors = colors;
	}
	
	public void setIndices(final int[] indices) {
		this.indices = indices;
	}
	
	public void setIndices(final List<Integer> indices) {
		this.indices = convertIntegers(indices);
		this.vertexCount = this.indices.length;
	}
	
	public void setNormals(final Object normals) {
		this.normals = normals;
	}
	
	public void setPosition(final Object positions) {
		this.positions = positions;
	}
	
	public void setTextureCoordinate(final float[] textureCoordinates) {
		this.textureCoordinates = textureCoordinates;
	}
	
	public void setVertexCount(final int vertexCount) {
		this.vertexCount = vertexCount;
	}
	
	private void storeDataInAttributeList(final int attributeNumber, final int coordinateSize, final Object data) {
		final int vboID = GL15.glGenBuffers();
		this.vbo.add(vboID);
		GL15.glBindBuffer(GL15.GL_ARRAY_BUFFER, vboID);
		Usage usage = Usage.staticDraw;
		if (this.dynamic) {
			usage = Usage.streamDraw;
		}
		// select the buffer to set data inside it ...
		if (data instanceof float[]buffer) {
			OpenGL.bufferData(buffer, usage);
		} else if (data instanceof int[]buffer) {
			OpenGL.bufferData(buffer, usage);
		} else if (data instanceof Vector2f[]buffer) {
			OpenGL.bufferData(buffer, usage);
		} else if (data instanceof Vector3f[]buffer) {
			OpenGL.bufferData(buffer, usage);
		} else if (data instanceof Color[]buffer) {
			OpenGL.bufferData(buffer, usage);
		} else {
			Log.error("Not managed VBO model : " + data.getClass().getCanonicalName());
		}
		GL20.glVertexAttribPointer(attributeNumber, coordinateSize, GL11.GL_FLOAT, false, 0, 0);
		GL15.glBindBuffer(GL15.GL_ARRAY_BUFFER, 0);
	}
	
	public void unBindForRendering() {
		if (!this.exist) {
			return;
		}
		if (this.positions != null) {
			GL20.glDisableVertexAttribArray(INDICE_VBO_POSITIONS);
		}
		if (this.textureCoordinates != null) {
			GL20.glDisableVertexAttribArray(INDICE_VBO_TEXTURE_COORDINATES);
		}
		if (this.normals != null) {
			GL20.glDisableVertexAttribArray(INDICE_VBO_NORMALS);
		}
		if (this.colors != null) {
			GL20.glDisableVertexAttribArray(INDICE_VBO_COLORS);
		}
		GL30.glBindVertexArray(0);
	}
	
	private void unbindVAO() {
		Log.error("Unbind VAO ...");
		GL30.glBindVertexArray(0);
	}
	
	/**
	 * This load/reload the data in the opengl context, needed when removed previously.
	 */
	@Override
	public boolean updateContext() {
		Log.error(" Start: [" + getId() + "] '" + getName() + "' (size=" + this.vertexCount + ") ********************************");
		if (!this.exist) {
			Log.error("     ==> ALLOCATE new handle");
			// Allocate and assign a Vertex Array Object to our handle
			loadToVAO();
		} else {
			// Update VAO (only for dynamic:
			if (!this.dynamic) {
				Log.error(" Request update a VAO with a static buffer !!!");
			}
			loadAgainToVAO();
			
		}
		this.exist = true;
		Log.error(" Stop: [" + getId() + "] '" + getName() + "'");
		return true;
	}
	
}
