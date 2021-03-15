package org.atriasoft.gale.resource;

import java.nio.FloatBuffer;
import java.nio.IntBuffer;
import java.util.ArrayList;
import java.util.List;

import org.atriasoft.gale.backend3d.OpenGL;
import org.atriasoft.gale.backend3d.OpenGL.RenderMode;
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
	
	private int vaoID = -1;
	private boolean exist = false; //!< This data is availlable in the Graphic card
	private final List<Integer> vbo = new ArrayList<>();
	
	float[] positions = null;
	
	float[] colors = null;
	
	float[] textureCoordinates = null;
	
	float[] normals = null;
	
	int[] indices = null;
	
	int vertexCount = -1;
	
	/**
	 * @brief ructor of this VBO.
	 * @param accesMode Acces mode : ???
	 */
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
		if (this.exist == false) {
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
		final int vboId = OpenGL.glGenBuffers();
		this.vbo.add(vboId);
		GL15.glBindBuffer(GL15.GL_ELEMENT_ARRAY_BUFFER, vboId);
		final IntBuffer buffer = storeDataInIntBuffer(indices);
		GL15.glBufferData(GL15.GL_ELEMENT_ARRAY_BUFFER, buffer, GL15.GL_STATIC_DRAW);
	}
	
	/**
	 * @brief Destructor of this VBO.
	 */
	@Override
	public void cleanUp() {
		removeContext();
	}
	
	/**
	 * @brief clear buffers
	 */
	public void clear() {
		//Log.verbose(" Clear: [" + getId() + "] '" + getName() + "' (size=" + this.buffer.get(0).length + ")");
		// DO not clear the this.vbo indexed in the graphic cards ...
		
	}
	
	private void createVAO() {
		this.vaoID = GL30.glGenVertexArrays();
		GL30.glBindVertexArray(this.vaoID);
	}
	
	/**
	 * @brief Send the data to the graphic card.
	 */
	public void flush() {
		// request to the manager to be call at the next update ...
		getManager().update(this);
		Log.verbose("Request flush of VBO: [" + getId() + "] '" + getName() + "'");
	}
	
	/**
	 * @brief get the real openGL ID.
	 * @return the Ogl id reference of this VBO.
	 */
	public int getGLID() {
		return this.vaoID;
	};
	
	public void loadToVAO() {
		createVAO();
		if (this.indices != null) {
			Log.info("Set indices");
			bindIndicesBuffer(this.indices);
		}
		if (this.positions != null) {
			Log.info("Set positions");
			storeDataInAttributeList(0, 3, this.positions);
		}
		if (this.textureCoordinates != null) {
			Log.info("Set textureCoordinates");
			storeDataInAttributeList(1, 2, this.textureCoordinates);
		}
		if (this.normals != null) {
			Log.info("Set normals");
			storeDataInAttributeList(2, 3, this.normals);
		}
		if (this.colors != null) {
			Log.info("Set colors");
			storeDataInAttributeList(3, 4, this.colors);
		}
		unbindVAO();
	}
	
	/**
	 * @brief Relode the shader from the file. used when a request of resouces reload is done.
	 * @note this is really usefull when we tested the new themes or shader developpements.
	 */
	@Override
	public void reload() {
		removeContext();
		updateContext();
	}
	
	/**
	 * @brief remove the data from the opengl context.
	 */
	@Override
	public void removeContext() {
		
		if (this.exist == true) {
			// OpenGL.deleteBuffers(this.vbo);
			this.exist = false;
		}
	}
	
	/**
	 * @brief Special android spec! It inform us that all context is removed and after notify us...
	 */
	@Override
	public void removeContextToLate() {
		
		this.exist = false;
		//		for (int iii=0; iii<this.vbo.length; iii++) {
		//			this.vbo[iii] = 0;
		//		}
	}
	
	public void render(final RenderMode mode) {
		OpenGL.drawElements(mode, this.vertexCount);
	}
	
	private void storeDataInAttributeList(final int attributeNumber, final int coordinateSize, final float[] data) {
		final int vboID = GL15.glGenBuffers();
		this.vbo.add(vboID);
		GL15.glBindBuffer(GL15.GL_ARRAY_BUFFER, vboID);
		final FloatBuffer buffer = storeDataInFloatBuffer(data);
		GL15.glBufferData(GL15.GL_ARRAY_BUFFER, buffer, GL15.GL_STATIC_DRAW);
		GL20.glVertexAttribPointer(attributeNumber, coordinateSize, GL11.GL_FLOAT, false, 0, 0);
		GL15.glBindBuffer(GL15.GL_ARRAY_BUFFER, 0);
	}
	
	public void unBindForRendering() {
		if (this.exist == false) {
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
		GL30.glBindVertexArray(0);
	}
	
	/**
	 * @brief This load/reload the data in the opengl context, needed when removed previously.
	 */
	@Override
	public boolean updateContext() {
		//Log.verbose(" Start: [" + getId() + "] '" + getName() + "' (size=" + this.indices.length + ") ********************************");
		if (this.exist == false) {
			Log.debug("     ==> ALLOCATE new handle");
			// Allocate and assign a Vertex Array Object to our handle
			loadToVAO();
		}
		this.exist = true;
		Log.verbose(" Stop: [" + getId() + "] '" + getName() + "'");
		return true;
	}
	
}
