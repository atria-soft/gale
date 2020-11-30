package org.atriasoft.gale.resource;

import java.nio.FloatBuffer;
import java.nio.IntBuffer;
import java.util.ArrayList;
import java.util.List;
import org.atriasoft.gale.internal.Log;
import org.atriasoft.gale.backend3d.OpenGL;
import org.atriasoft.gale.backend3d.OpenGL.RenderMode;
import org.lwjgl.BufferUtils;
import org.lwjgl.opengl.GL11;
import org.lwjgl.opengl.GL15;
import org.lwjgl.opengl.GL20;
import org.lwjgl.opengl.GL30;

//import models.RawModel;

public class ResourceVirtualArrayObject extends Resource {
	private int vaoID = -1;
	private boolean exist = false;  //!< This data is availlable in the Graphic card
	private List<Integer> vbo = new ArrayList<Integer>();
	float[] positions = null;
	float[] colors = null;
	float[] textureCoordinates = null;
	float[] normals = null;
	int[] indices = null;
	int vertexCount = -1;
	public static final int INDICE_VBO_POSITIONS = 0;
	public static final int INDICE_VBO_TEXTURE_COORDINATES = 1;
	public static final int INDICE_VBO_NORMALS = 2;
	public static final int INDICE_VBO_COLORS = 3;

	public void bindForRendering() {
		if (exist == false) {
			return;
		}
		GL30.glBindVertexArray(vaoID);
		if (positions != null) {
			GL20.glEnableVertexAttribArray(INDICE_VBO_POSITIONS);
			//Log.info("unbind POSITION");
		}
		if (textureCoordinates != null) {
			GL20.glEnableVertexAttribArray(INDICE_VBO_TEXTURE_COORDINATES);
		}
		if (normals != null) {
			GL20.glEnableVertexAttribArray(INDICE_VBO_NORMALS);
		}
		if (colors != null) {
			GL20.glEnableVertexAttribArray(INDICE_VBO_COLORS);
		}
	}

	public void unBindForRendering() {
		if (exist == false) {
			return;
		}
		if (positions != null) {
			GL20.glDisableVertexAttribArray(INDICE_VBO_POSITIONS);
		}
		if (textureCoordinates != null) {
			GL20.glDisableVertexAttribArray(INDICE_VBO_TEXTURE_COORDINATES);
		}
		if (normals != null) {
			GL20.glDisableVertexAttribArray(INDICE_VBO_NORMALS);
		}
		if (colors != null) {
			GL20.glDisableVertexAttribArray(INDICE_VBO_COLORS);
		}
		GL30.glBindVertexArray(0);
	}

	public void render(RenderMode mode) {
		OpenGL.drawElements(mode, vertexCount);
	}
	
	
	private FloatBuffer storeDataInFloatBuffer(float[] data) {
		FloatBuffer buffer = BufferUtils.createFloatBuffer(data.length);
		buffer.put(data);
		buffer.flip();
		return buffer;
	}
	
	private IntBuffer storeDataInIntBuffer(int[] data) {
		IntBuffer buffer = BufferUtils.createIntBuffer(data.length);
		buffer.put(data);
		buffer.flip();
		return buffer;
	}
	
	private void storeDataInAttributeList(int attributeNumber, int coordinateSize, float[] data) {
		int vboID = GL15.glGenBuffers();
		vbo.add(vboID);
		GL15.glBindBuffer(GL15.GL_ARRAY_BUFFER, vboID);
		FloatBuffer buffer = storeDataInFloatBuffer(data);
		GL15.glBufferData(GL15.GL_ARRAY_BUFFER, buffer, GL15.GL_STATIC_DRAW);
		GL20.glVertexAttribPointer(attributeNumber, coordinateSize, GL11.GL_FLOAT, false, 0, 0);
		GL15.glBindBuffer(GL15.GL_ARRAY_BUFFER, 0);
	}
	
	private void bindIndicesBuffer(int[] indices) {
		int vboId = GL15.glGenBuffers();
		vbo.add(vboId);
		GL15.glBindBuffer(GL15.GL_ELEMENT_ARRAY_BUFFER, vboId);
		IntBuffer buffer = storeDataInIntBuffer(indices);
		GL15.glBufferData(GL15.GL_ELEMENT_ARRAY_BUFFER, buffer, GL15.GL_STATIC_DRAW);
	}

	private void createVAO() {
		vaoID = GL30.glGenVertexArrays();
		GL30.glBindVertexArray(vaoID);
	}
	private void unbindVAO() {
		GL30.glBindVertexArray(0);
	}

	public void loadToVAO() {
		createVAO();
		if (indices != null) {
			Log.info("Set indices");
			bindIndicesBuffer(indices);
		}
		if (positions != null) {
			Log.info("Set positions");
			storeDataInAttributeList(0, 3, positions);
		}
		if (textureCoordinates != null) {
			Log.info("Set textureCoordinates");
			storeDataInAttributeList(1, 2, textureCoordinates);
		}
		if (normals != null) {
			Log.info("Set normals");
			storeDataInAttributeList(2, 3, normals);
		}
		if (colors != null) {
			Log.info("Set colors");
			storeDataInAttributeList(3, 4, colors);
		}
		unbindVAO();
	}
	/**
	 * @brief ructor of this VBO.
	 * @param accesMode Acces mode : ???
	 */
	protected ResourceVirtualArrayObject(float[] positions, float[] colors, float[] textureCoordinates, float[] normals, int[] indices, int vertexCount) {
		super();
		addResourceType("gale::VirtualBufferObject");
		this.resourceLevel = 3;
		this.positions = positions;
		this.colors = colors;
		this.textureCoordinates = textureCoordinates;
		this.normals = normals;
		this.indices = indices;
		this.vertexCount = vertexCount;
		Log.debug("OGL: load VBO count");
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
	/**
	 * @brief get the real openGL ID.
	 * @return the Ogl id reference of this VBO.
	 */
	public int getGLID()  {
		return this.vaoID;
	};

	/**
	 * @brief Send the data to the graphic card.
	 */
	public void flush() {
		// request to the manager to be call at the next update ...
		getManager().update(this);
		Log.verbose("Request flush of VBO: [" + getId() + "] '" + getName() + "'");
	}
	/**
	 * @brief This load/reload the data in the opengl context, needed when removed previously.
	 */
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
	/**
	 * @brief remove the data from the opengl context.
	 */
	public void removeContext() {
		
		if (this.exist == true) {
			// OpenGL.deleteBuffers(this.vbo);
			this.exist = false;
		}
	}
	/**
	 * @brief Special android spec! It inform us that all context is removed and after notify us...
	 */
	public void removeContextToLate() {
		
		this.exist = false;
//		for (int iii=0; iii<this.vbo.length; iii++) {
//			this.vbo[iii] = 0;
//		}
	}
	/**
	 * @brief Relode the shader from the file. used when a request of resouces reload is done.
	 * @note this is really usefull when we tested the new themes or shader developpements.
	 */
	public void reload() {
		removeContext();
		updateContext();
	}

	public static ResourceVirtualArrayObject create(float[] positions, float[] colors, float[] textureCoordinates, float[] normals, int[] indices) {
		ResourceVirtualArrayObject resource = new ResourceVirtualArrayObject(positions, colors, textureCoordinates, normals, indices, indices.length);
		if (resource.resourceHasBeenCorectlyInit() == false) {
			Log.critical("resource Is not correctly init: ResourceVirtualBufferObject");
		}
		getManager().localAdd(resource);
		return resource;
	}
	public static ResourceVirtualArrayObject create(float[] positions, float[] colors, int[] indices) {
		ResourceVirtualArrayObject resource = new ResourceVirtualArrayObject(positions, colors, null, null, indices, indices.length);
		if (resource.resourceHasBeenCorectlyInit() == false) {
			Log.critical("resource Is not correctly init: ResourceVirtualBufferObject");
		}
		getManager().localAdd(resource);
		return resource;
	}
	public static ResourceVirtualArrayObject create(float[] positions, int dimentions) {
		ResourceVirtualArrayObject resource = new ResourceVirtualArrayObject(positions, null, null, null, null, positions.length/dimentions);
		if (resource.resourceHasBeenCorectlyInit() == false) {
			Log.critical("resource Is not correctly init: ResourceVirtualBufferObject");
		}
		getManager().localAdd(resource);
		return resource;
	}
	public static ResourceVirtualArrayObject create(float[] positions, float[] textureCoordinates, float[] normals, int[] indices) {
		ResourceVirtualArrayObject resource = new ResourceVirtualArrayObject(positions, null, textureCoordinates, normals, indices, indices.length);
		if (resource.resourceHasBeenCorectlyInit() == false) {
			Log.critical("resource Is not correctly init: ResourceVirtualBufferObject");
		}
		getManager().localAdd(resource);
		return resource;
	}

}
