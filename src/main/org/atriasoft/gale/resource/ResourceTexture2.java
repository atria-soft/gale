/** @file
 * @author Edouard DUPIN
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license MPL v2.0 (see license file)
 */
package org.atriasoft.gale.resource;

import java.awt.image.BufferedImage;

import org.atriasoft.etk.Tools;
import org.atriasoft.etk.Uri;
import org.atriasoft.etk.math.Vector2i;
import org.atriasoft.gale.TextureFilter;
import org.atriasoft.gale.backend3d.OpenGL;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class ResourceTexture2 extends Resource {
	static final Logger LOGGER = LoggerFactory.getLogger(ResourceTexture2.class);

	public enum TextureColorMode {
		rgb, // !< red/green/blue data
		rgba // !< red/green/blue/alpha data
	}

	public static ResourceTexture2 create() {
		LOGGER.trace("KEEP: Resource Texture Dynamic: ");
		return new ResourceTexture2();
	}

	public static ResourceTexture2 create(final Uri uri) {
		LOGGER.trace("KEEP: Resource Texture: {}", uri);
		final Resource object2 = Resource.getManager().localKeep(uri);
		if (object2 != null) {
			if (object2 instanceof final ResourceTexture2 tmpp) {
				return tmpp;
			}
			LOGGER.error("Request resource file: '{}' With the wrong type (dynamic cast error)", uri);
			System.exit(-1);
			return null;
		}
		LOGGER.trace("CREATE: new Texture: {}", uri);
		return new ResourceTexture2(uri);
	}

	public static ResourceTexture2 createNamed(final String uri) {
		LOGGER.trace("KEEP: Resource Texture Named: {}", uri);
		final Resource object2 = Resource.getManager().localKeep(uri);
		if (object2 != null) {
			if (object2 instanceof final ResourceTexture2 tmpp) {
				return tmpp;
			}
			LOGGER.error("Request resource file: '{}' With the wrong type (dynamic cast error)", uri);
			System.exit(-1);
			return null;
		}
		LOGGER.debug("CREATE: new Texture Named: {}", uri);
		return new ResourceTexture2(uri);
	}

	/**
	 * Extract raw byte array (RGBA or RGB) from a BufferedImage for OpenGL upload.
	 */
	static byte[] extractRawBytes(final BufferedImage image) {
		final int width = image.getWidth();
		final int height = image.getHeight();
		final boolean hasAlpha = image.getColorModel().hasAlpha();
		final int channels = hasAlpha ? 4 : 3;
		final byte[] raw = new byte[width * height * channels];
		for (int yyy = 0; yyy < height; yyy++) {
			for (int xxx = 0; xxx < width; xxx++) {
				final int argb = image.getRGB(xxx, yyy);
				final int offset = (yyy * width + xxx) * channels;
				raw[offset] = (byte) ((argb >> 16) & 0xFF);
				raw[offset + 1] = (byte) ((argb >> 8) & 0xFF);
				raw[offset + 2] = (byte) (argb & 0xFF);
				if (hasAlpha) {
					raw[offset + 3] = (byte) ((argb >> 24) & 0xFF);
				}
			}
		}
		return raw;
	}

	/**
	 * Resize a BufferedImage, preserving existing pixel data.
	 * <p>
	 * WARNING: Do NOT replace with Graphics2D.drawImage() — it applies alpha
	 * compositing which destroys RGB channel data when Alpha is 0. The font
	 * atlas stores independent data per ARGB channel (one font mode per channel),
	 * so raw pixel copy is required to preserve all channels correctly.
	 */
	protected static BufferedImage resizeImage(final BufferedImage source, final int newWidth, final int newHeight) {
		final BufferedImage resized = new BufferedImage(newWidth, newHeight, source.getType());
		final int copyW = Math.min(source.getWidth(), newWidth);
		final int copyH = Math.min(source.getHeight(), newHeight);
		for (int y = 0; y < copyH; y++) {
			for (int x = 0; x < copyW; x++) {
				resized.setRGB(x, y, source.getRGB(x, y));
			}
		}
		return resized;
	}

	// openGl Context properties :
	protected BufferedImage data = new BufferedImage(32, 32, BufferedImage.TYPE_INT_ARGB);
	// !< Color space of the image.
	private final TextureColorMode dataColorSpace = TextureColorMode.rgba;
	// Filter apply at the image when rendering it
	protected TextureFilter filter = TextureFilter.LINEAR;
	// ! Last loaded size in the system openGL
	protected Vector2i lastSize = new Vector2i(1, 1);
	protected int lastSizeObject = 0;
	protected int lastTypeObject = 0;
	// internal state of the openGl system.
	protected boolean loaded = false;
	// ! some image are not square == > we need to sqared it to prevent some openGl
	// api error the the displayable size is not all the time 0.0 . 1.0
	protected Vector2i realImageSize = new Vector2i(1, 1);

	// repeat mode of the image (repeat the image if out of range [0..1])
	protected boolean repeat = false;

	protected int texId = -1; // !< openGl textureID.

	public ResourceTexture2() {}

	public ResourceTexture2(final String filename) {
		super(filename);
	}

	public ResourceTexture2(final Uri filename) {
		super(filename);
	}

	public void bindForRendering(final int idTexture) {
		if (!this.loaded) {
			LOGGER.warn("TEXTURE: bindForRendering called but texture not loaded: [{}]", getId());
			return;
		}
		OpenGL.activeTexture(idTexture);
		OpenGL.bindTexture2D(this.texId);
		if (this.dataColorSpace == TextureColorMode.rgb) {
			OpenGL.enable(OpenGL.Flag.flag_cullFace);
			OpenGL.enable(OpenGL.Flag.flag_back);
		}
	}

	@Override
	public void cleanUp() {
		removeContext();
	}

	// Flush the data to send it at the openGl system
	public synchronized void flush() {
		// request to the manager to be call at the next update ...
		LOGGER.trace("Request UPDATE of Element");
		Resource.getManager().update(this);
	}

	// Get the reference on this image to draw something on it ...
	public BufferedImage get() {
		return this.data;
	}

	public Vector2i getOpenGlSize() {
		return new Vector2i(this.data.getWidth(), this.data.getHeight());
	}

	public int getRendererId() {
		return this.texId;
	}

	public Vector2i getUsableSize() {
		return this.realImageSize;
	}

	@Override
	public synchronized void removeContext() {
		if (this.loaded) {
			// Request remove texture ...
			LOGGER.debug("TEXTURE: Rm [{}] texId={}", getId(), this.texId);
			// TODO Check if we are in the correct thread
			OpenGL.glDeleteTextures(this.texId);
			this.loaded = false;
		}
	}

	@Override
	public synchronized void removeContextToLate() {
		this.loaded = false;
		this.texId = -1;
	}

	/**
	 * Set the image in the texture system
	 * @param image Image to set.
	 */
	public synchronized void set(final BufferedImage image) {
		LOGGER.trace("Set a new image in a texture:    size={}x{}", image.getWidth(), image.getHeight());
		this.data = image;
		this.realImageSize = new Vector2i(this.data.getWidth(), this.data.getHeight());
		flush();
	}

	/**
	 * Set the Filter mode to apply at the image when display with a scale
	 *        (not 1:1 ratio)
	 * @param filter Value of the new filter mode
	 */
	public void setFilterMode(final TextureFilter filter) {
		this.filter = filter;
	}

	// You must set the size here, because it will be set in multiple of pow(2)
	public synchronized void setImageSize(final Vector2i requestedSize) {
		final Vector2i newSize = new Vector2i(Tools.nextP2(requestedSize.x()), Tools.nextP2(requestedSize.y()));
		if (this.data.getWidth() != newSize.x() || this.data.getHeight() != newSize.y()) {
			this.data = resizeImage(this.data, newSize.x(), newSize.y());
		}
	}

	/**
	 * Set the repeat mode of the images if UV range is out of [0..1]
	 * @param value Value of the new repeat mode
	 */
	public void setRepeat(final boolean value) {
		this.repeat = value;
	}

	public void unBindForRendering() {
		if (!this.loaded) {
			return;
		}
		if (this.dataColorSpace == TextureColorMode.rgb) {
			OpenGL.disable(OpenGL.Flag.flag_cullFace);
			OpenGL.disable(OpenGL.Flag.flag_back);
		}
	}

	@Override
	public synchronized boolean updateContext() {
		LOGGER.trace("updateContext [START]");
		final boolean hasAlpha = this.data.getColorModel().hasAlpha();
		final int typeObject = hasAlpha ? OpenGL.GL_RGBA : OpenGL.GL_RGB;
		final int sizeObject = OpenGL.GL_UNSIGNED_BYTE;
		final int width = this.data.getWidth();
		final int height = this.data.getHeight();
		final Vector2i currentSize = new Vector2i(width, height);
		if (this.loaded) {
			if (this.lastTypeObject != typeObject || this.lastSizeObject != sizeObject
					|| !this.lastSize.equals(currentSize)) {
				LOGGER.trace("TEXTURE: Rm [{}] texId={}", getId(), this.texId);
				OpenGL.glDeleteTextures(this.texId);
				this.loaded = false;
			}
		}
		if (!this.loaded) {
			// Request a new texture at openGl :
			this.texId = OpenGL.glGenTextures();
			this.lastSize = currentSize;
			this.lastTypeObject = typeObject;
			this.lastSizeObject = sizeObject;
			LOGGER.trace("TEXTURE: add [{}]={}x{} OGlId={}", getId(), width, height, this.texId);
		} else {
			LOGGER.trace("TEXTURE: update [{}]={}x{} OGlId={}", getId(), width, height, this.texId);
		}
		// in all case we set the texture properties :
		// Force texture unit 0 to avoid binding on wrong unit
		OpenGL.forceActiveTexture(0);
		OpenGL.bindTexture2D(this.texId);

		if (!this.loaded) {
			if (!this.repeat) {
				OpenGL.setTexture2DWrapClampToEdge();
			} else {
				OpenGL.setTexture2DWrapRepeat();
			}
			if (this.filter == TextureFilter.LINEAR) {
				OpenGL.setTexture2DFilterLinear();
			} else {
				OpenGL.setTexture2DFilterNearest();
			}
		}
		final byte[] raw = extractRawBytes(this.data);
		if (!this.loaded) {
			OpenGL.glTexImage2D(0, // Level
					typeObject, // Format internal
					width, height, 0, // Border
					typeObject, // format
					sizeObject, // type
					raw);

		} else {
			OpenGL.glTexSubImage2D(0, // Level
					0, // x offset
					0, // y offset
					width, height, typeObject, // format
					sizeObject, // type
					raw);
		}
		// now the data is loaded
		this.loaded = true;
		return true;
	}

}
