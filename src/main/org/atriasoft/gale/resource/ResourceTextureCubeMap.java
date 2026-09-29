package org.atriasoft.gale.resource;

import java.awt.image.BufferedImage;
import java.io.IOException;
import java.io.InputStream;

import javax.imageio.ImageIO;

import org.atriasoft.etk.Uri;
import org.atriasoft.gale.backend3d.OpenGL;
import org.lwjgl.opengl.GL11;
import org.lwjgl.opengl.GL13;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/**
 * OpenGL cubemap texture resource.
 * <p>
 * Loads 6 face images (right, left, top, bottom, front, back) and uploads
 * them as a single {@code GL_TEXTURE_CUBE_MAP}. Follows the same lifecycle
 * pattern as {@link ResourceTexture2}.
 */
public class ResourceTextureCubeMap extends Resource {
	private static final Logger LOGGER = LoggerFactory.getLogger(ResourceTextureCubeMap.class);

	/**
	 * The 6 cubemap face targets in OpenGL order:
	 * +X, -X, +Y, -Y, +Z, -Z
	 */
	private static final int[] CUBE_MAP_FACES = {
		GL13.GL_TEXTURE_CUBE_MAP_POSITIVE_X, // right
		GL13.GL_TEXTURE_CUBE_MAP_NEGATIVE_X, // left
		GL13.GL_TEXTURE_CUBE_MAP_POSITIVE_Y, // top
		GL13.GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, // bottom
		GL13.GL_TEXTURE_CUBE_MAP_POSITIVE_Z, // front
		GL13.GL_TEXTURE_CUBE_MAP_NEGATIVE_Z  // back
	};

	/**
	 * Create a cubemap texture from 6 face image URIs.
	 * @param right  +X face
	 * @param left   -X face
	 * @param top    +Y face
	 * @param bottom -Y face
	 * @param front  +Z face
	 * @param back   -Z face
	 * @return The cubemap resource
	 */
	public static ResourceTextureCubeMap create(
			final Uri right,
			final Uri left,
			final Uri top,
			final Uri bottom,
			final Uri front,
			final Uri back) {
		final String key = "cubemap[RGBA]:" + right + "," + left + "," + top + "," + bottom + "," + front + "," + back;
		final Resource existing = Resource.getManager().localKeep(key);
		if (existing != null) {
			if (existing instanceof final ResourceTextureCubeMap cubemap) {
				return cubemap;
			}
			LOGGER.error("Resource '{}' exists with wrong type", key);
			return null;
		}
		LOGGER.debug("CREATE: CubeMap: {}", key);
		return new ResourceTextureCubeMap(key, right, left, top, bottom, front, back);
	}

	private final Uri[] faceUris = new Uri[6];
	private final BufferedImage[] faceImages = new BufferedImage[6];
	private int texId = -1;
	private boolean loaded = false;

	private ResourceTextureCubeMap(
			final String name,
			final Uri right,
			final Uri left,
			final Uri top,
			final Uri bottom,
			final Uri front,
			final Uri back) {
		super(name);
		this.faceUris[0] = right;
		this.faceUris[1] = left;
		this.faceUris[2] = top;
		this.faceUris[3] = bottom;
		this.faceUris[4] = front;
		this.faceUris[5] = back;
		// Load images from disk immediately (they will be uploaded to GPU in updateContext)
		for (int i = 0; i < 6; i++) {
			try (final InputStream in = Uri.getStream(this.faceUris[i])) {
				this.faceImages[i] = ImageIO.read(in);
				if (this.faceImages[i] == null) {
					LOGGER.error("Failed to decode cubemap face image: {}", this.faceUris[i]);
				}
			} catch (final IOException ex) {
				LOGGER.error("Failed to load cubemap face: {}", this.faceUris[i], ex);
			}
		}
		// Upload to GPU: synchronous if GL context available, deferred otherwise
		if (OpenGL.hasContext()) {
			updateContext();
		} else {
			Resource.getManager().update(this);
		}
	}

	/**
	 * Bind this cubemap texture to the given texture unit for rendering.
	 * @param textureUnit Texture unit index (0, 1, 2, ...)
	 */
	public void bindForRendering(final int textureUnit) {
		if (!this.loaded) {
			LOGGER.warn("CubeMap: bindForRendering called but not loaded: [{}]", getId());
			return;
		}
		OpenGL.activeTexture(textureUnit);
		OpenGL.bindTextureCubeMap(this.texId);
	}

	/**
	 * Unbind the cubemap texture.
	 */
	public void unBindForRendering() {
		// Nothing specific needed
	}

	/**
	 * Get the OpenGL texture ID.
	 * @return OpenGL texture name
	 */
	public int getRendererId() {
		return this.texId;
	}

	@Override
	public synchronized boolean updateContext() {
		LOGGER.trace("CubeMap updateContext [START]");
		if (this.loaded) {
			OpenGL.glDeleteTextures(this.texId);
			this.loaded = false;
		}
		this.texId = OpenGL.glGenTextures();
		OpenGL.forceActiveTexture(0);
		OpenGL.bindTextureCubeMap(this.texId);
		for (int i = 0; i < 6; i++) {
			if (this.faceImages[i] == null) {
				LOGGER.error("CubeMap face {} is null, skipping", i);
				continue;
			}
			final BufferedImage img = this.faceImages[i];
			final byte[] raw = ResourceTexture2.extractRawBytesRGBA(img);
			OpenGL.glTexImage2DCubeMapFace(
					CUBE_MAP_FACES[i],
					0,
					OpenGL.GL_RGBA,
					img.getWidth(),
					img.getHeight(),
					0,
					OpenGL.GL_RGBA,
					GL11.GL_UNSIGNED_BYTE,
					raw);
		}
		OpenGL.setTextureCubeMapFilterLinear();
		OpenGL.setTextureCubeMapWrapClampToEdge();
		this.loaded = true;
		LOGGER.debug("CubeMap loaded: texId={}", this.texId);
		return true;
	}

	@Override
	public synchronized void removeContext() {
		if (this.loaded) {
			LOGGER.debug("CubeMap: Rm [{}] texId={}", getId(), this.texId);
			OpenGL.glDeleteTextures(this.texId);
			this.loaded = false;
		}
	}

	@Override
	public synchronized void removeContextToLate() {
		this.loaded = false;
		this.texId = -1;
	}

	@Override
	public void cleanUp() {
		removeContext();
	}
}
