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
import org.lwjgl.BufferUtils;
import org.lwjgl.opengl.GL11;
import org.lwjgl.opengl.GL15;
import org.lwjgl.opengl.GL20;
import org.lwjgl.opengl.GL30;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class ResourceVirtualArrayObject extends Resource {
	static final Logger LOGGER = LoggerFactory.getLogger(ResourceVirtualArrayObject.class);
	public static final int INDICE_VBO_POSITIONS = 0;
	public static final int INDICE_VBO_TEXTURE_COORDINATES = 1;
	public static final int INDICE_VBO_NORMALS = 2;
	public static final int INDICE_VBO_COLORS = 3;

	public static int[] convertIntegers(final List<Integer> integers) {
		final int[] ret = new int[integers.size()];
		final Iterator<Integer> iterator = integers.iterator();
		for (int i = 0; i < ret.length; i++) {
			ret[i] = iterator.next().intValue();
		}
		return ret;
	}

	public static ResourceVirtualArrayObject create(
			final float[] positions,
			final float[] colors,
			final float[] textureCoordinates,
			final float[] normals,
			final int[] indices) {
		final ResourceVirtualArrayObject resource = new ResourceVirtualArrayObject(positions, colors,
				textureCoordinates, normals, indices, indices.length);
		Resource.getManager().localAdd(resource);
		return resource;
	}

	public static ResourceVirtualArrayObject create(
			final float[] positions,
			final float[] textureCoordinates,
			final float[] normals,
			final int[] indices) {
		final ResourceVirtualArrayObject resource = new ResourceVirtualArrayObject(positions, null, textureCoordinates,
				normals, indices, indices.length);
		Resource.getManager().localAdd(resource);
		return resource;
	}

	public static ResourceVirtualArrayObject create(
			final float[] positions,
			final float[] colors,
			final int[] indices) {
		final ResourceVirtualArrayObject resource = new ResourceVirtualArrayObject(positions, colors, null, null,
				indices, indices.length);
		Resource.getManager().localAdd(resource);
		return resource;
	}

	public static ResourceVirtualArrayObject create(final float[] positions, final int dimentions) {
		final ResourceVirtualArrayObject resource = new ResourceVirtualArrayObject(positions, null, null, null, null,
				positions.length / dimentions);
		Resource.getManager().localAdd(resource);
		return resource;
	}

	public static ResourceVirtualArrayObject createDynamic() {
		final ResourceVirtualArrayObject resource = new ResourceVirtualArrayObject();
		Resource.getManager().localAdd(resource);
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

	private boolean dynamic = false;
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
		this.resourceLevel = 3;
		this.dynamic = true;
		LOGGER.trace("OGL: load VBO count (dynamic)");
	}

	protected ResourceVirtualArrayObject(final float[] positions, final float[] colors,
			final float[] textureCoordinates, final float[] normals, final int[] indices, final int vertexCount) {
		this.resourceLevel = 3;
		this.positions = positions;
		this.colors = colors;
		this.textureCoordinates = textureCoordinates;
		this.normals = normals;
		this.indices = indices;
		this.vertexCount = vertexCount;
		LOGGER.debug("OGL: load VBO count");
	}

	public void bindForRendering() {
		if (!this.exist) {
			return;
		}
		GL30.glBindVertexArray(this.vaoID);
		if (this.positions != null) {
			GL20.glEnableVertexAttribArray(ResourceVirtualArrayObject.INDICE_VBO_POSITIONS);
			//LOGGER.info("unbind POSITION");
		}
		if (this.textureCoordinates != null) {
			GL20.glEnableVertexAttribArray(ResourceVirtualArrayObject.INDICE_VBO_TEXTURE_COORDINATES);
		}
		if (this.normals != null) {
			GL20.glEnableVertexAttribArray(ResourceVirtualArrayObject.INDICE_VBO_NORMALS);
		}
		if (this.colors != null) {
			GL20.glEnableVertexAttribArray(ResourceVirtualArrayObject.INDICE_VBO_COLORS);
		}
	}

	private void bindIndicesBuffer(final int[] indices) {
		final int vboId = OpenGL.genBuffers();
		this.vbo.add(vboId);
		GL15.glBindBuffer(GL15.GL_ELEMENT_ARRAY_BUFFER, vboId);
		final IntBuffer buffer = ResourceVirtualArrayObject.storeDataInIntBuffer(indices);
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
		//LOGGER.trace(" Clear: [" + getId() + "] '" + getName() + "' (size=" + buffer.get(0).length + ")");
		this.positions = null;
		this.colors = null;
		this.textureCoordinates = null;
		this.normals = null;
		this.indices = null;
		this.vertexCount = -1;
	}

	private void createVAO() {
		LOGGER.trace("create VAO...");
		this.vaoID = GL30.glGenVertexArrays();
		GL30.glBindVertexArray(this.vaoID);
	}

	/**
	 * Send the data to the graphic card.
	 */
	public void flush() {
		// request to the manager to be call at the next update ...
		Resource.getManager().update(this);
		LOGGER.trace("Request flush of VAO: [{}] '{}'", getId(), getName());
	}

	/**
	 * get the real openGL ID.
	 * @return the Ogl id reference of this VBO.
	 */
	public int getGLID() {
		return this.vaoID;
	}

	public int getVertexCount() {
		return this.vertexCount;
	}

	public void loadAgainToVAO() {
		GL30.glBindVertexArray(this.vaoID);
		LOGGER.trace("push VAO: [{}] '{}'", getId(), getName());
		// Delete old VBOs before creating new ones
		for (final Integer vboId : this.vbo) {
			GL15.glDeleteBuffers(vboId);
		}
		this.vbo.clear();
		if (this.indices != null) {
			LOGGER.trace("Set indices");
			bindIndicesBuffer(this.indices);
		}
		if (this.positions != null) {
			LOGGER.trace("Set positions");
			storeDataInAttributeList(ResourceVirtualArrayObject.INDICE_VBO_POSITIONS, 3, this.positions);
		}
		if (this.textureCoordinates != null) {
			LOGGER.trace("Set textureCoordinates");
			storeDataInAttributeList(ResourceVirtualArrayObject.INDICE_VBO_TEXTURE_COORDINATES, 2,
					this.textureCoordinates);
		}
		if (this.normals != null) {
			LOGGER.trace("Set normals");
			storeDataInAttributeList(ResourceVirtualArrayObject.INDICE_VBO_NORMALS, 3, this.normals);
		}
		if (this.colors != null) {
			LOGGER.trace("Set colors");
			storeDataInAttributeList(ResourceVirtualArrayObject.INDICE_VBO_COLORS, 4, this.colors);
		}
		unbindVAO();
	}

	public void loadToVAO() {
		createVAO();
		LOGGER.trace("push VAO: [{}] '{}'", getId(), getName());
		if (this.indices != null) {
			LOGGER.trace("Set indices");
			bindIndicesBuffer(this.indices);
		}
		if (this.positions != null) {
			LOGGER.trace("Set positions");
			storeDataInAttributeList(ResourceVirtualArrayObject.INDICE_VBO_POSITIONS, 3, this.positions);
		}
		if (this.textureCoordinates != null) {
			LOGGER.trace("Set textureCoordinates");
			storeDataInAttributeList(ResourceVirtualArrayObject.INDICE_VBO_TEXTURE_COORDINATES, 2,
					this.textureCoordinates);
		}
		if (this.normals != null) {
			LOGGER.trace("Set normals");
			storeDataInAttributeList(ResourceVirtualArrayObject.INDICE_VBO_NORMALS, 3, this.normals);
		}
		if (this.colors != null) {
			LOGGER.trace("Set colors");
			storeDataInAttributeList(ResourceVirtualArrayObject.INDICE_VBO_COLORS, 4, this.colors);
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
		LOGGER.trace("request rendering indices: {}", this.vertexCount);
		OpenGL.drawElements(mode, this.vertexCount);
	}

	public void render(final RenderMode mode, final int start, final int stop) {
		OpenGL.drawArrays(mode, start, stop);
	}

	public void renderArrays(final RenderMode mode) {
		LOGGER.trace("request rendering direct: {}", this.vertexCount);
		OpenGL.drawArrays(mode, 0, this.vertexCount);
	}

	public void setColors(final Color[] colors) {
		this.colors = colors;
	}

	public void setColors(final float[] colors) {
		this.colors = colors;
	}

	public void setColors(final List<Color> colors) {
		setColors(colors.toArray(Color[]::new));

	}

	public void setIndices(final int[] indices) {
		this.indices = indices;
	}

	public void setIndices(final List<Integer> indices) {
		this.indices = ResourceVirtualArrayObject.convertIntegers(indices);
		this.vertexCount = this.indices.length;
	}

	public void setNormals(final float[] normals) {
		this.normals = normals;
	}

	public void setNormals(final List<Vector3f> normals) {
		setNormals(normals.toArray(Vector3f[]::new));
	}

	public void setNormals(final Vector3f[] normals) {
		this.normals = normals;
	}

	public void setPosition(final float[] positions) {
		this.positions = positions;
	}

	public void setPosition(final List<Vector3f> outPosition) {
		setPosition(outPosition.toArray(Vector3f[]::new));

	}

	public void setPosition(final Vector3f[] positions) {
		this.positions = positions;
	}

	public void setTextureCoordinate(final float[] textureCoordinates) {
		this.textureCoordinates = textureCoordinates;
	}

	public void setTextureCoordinate(final List<Vector2f> outTexturePosition) {
		setTextureCoordinate(outTexturePosition.toArray(Vector2f[]::new));
	}

	public void setTextureCoordinate(final Vector2f[] textureCoordinates) {
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
		if (data instanceof final float[] buffer) {
			OpenGL.bufferData(buffer, usage);
		} else if (data instanceof final int[] buffer) {
			OpenGL.bufferData(buffer, usage);
		} else if (data instanceof final Vector2f[] buffer) {
			OpenGL.bufferData(buffer, usage);
		} else if (data instanceof final Vector3f[] buffer) {
			OpenGL.bufferData(buffer, usage);
		} else if (data instanceof final Color[] buffer) {
			OpenGL.bufferData(buffer, usage);
		} else {
			LOGGER.error("Not managed VBO model: {}", data.getClass().getCanonicalName());
		}
		GL20.glVertexAttribPointer(attributeNumber, coordinateSize, GL11.GL_FLOAT, false, 0, 0);
		GL15.glBindBuffer(GL15.GL_ARRAY_BUFFER, 0);
	}

	public void unBindForRendering() {
		if (!this.exist) {
			return;
		}
		if (this.positions != null) {
			GL20.glDisableVertexAttribArray(ResourceVirtualArrayObject.INDICE_VBO_POSITIONS);
		}
		if (this.textureCoordinates != null) {
			GL20.glDisableVertexAttribArray(ResourceVirtualArrayObject.INDICE_VBO_TEXTURE_COORDINATES);
		}
		if (this.normals != null) {
			GL20.glDisableVertexAttribArray(ResourceVirtualArrayObject.INDICE_VBO_NORMALS);
		}
		if (this.colors != null) {
			GL20.glDisableVertexAttribArray(ResourceVirtualArrayObject.INDICE_VBO_COLORS);
		}
		GL30.glBindVertexArray(0);
	}

	private void unbindVAO() {
		LOGGER.trace("Unbind VAO ...");
		GL30.glBindVertexArray(0);
	}

	/**
	 * This load/reload the data in the opengl context, needed when removed previously.
	 */
	@Override
	public boolean updateContext() {
		LOGGER.trace(" Start: [{}] '{}' (size={}) ********************************", getId(), getName(), this.vertexCount);
		if (!this.exist) {
			LOGGER.trace("     ==> ALLOCATE new handle");
			// Allocate and assign a Vertex Array Object to our handle
			loadToVAO();
		} else {
			// Update VAO (only for dynamic:
			if (!this.dynamic) {
				LOGGER.error(" Request update a VAO with a static buffer !!! {}", this.name);
			}
			loadAgainToVAO();

		}
		this.exist = true;
		LOGGER.trace(" Stop: [{}] '{}'", getId(), getName());
		return true;
	}

}
