package org.atriasoft.gale.resource;

import java.nio.ByteBuffer;

import org.atriasoft.egami.ImageByteRGBA;
import org.atriasoft.etk.Uri;
import org.atriasoft.etk.math.Vector2i;
import org.atriasoft.gale.backend3d.OpenGL;
import org.atriasoft.gale.tools.ImageLoader;
import org.atriasoft.gale.tools.ImageRawData;
import org.lwjgl.opengl.GL11;
import org.lwjgl.opengl.GL13;
import org.lwjgl.opengl.GL30;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

@Deprecated
public class ResourceTexture extends Resource {
	static final Logger LOGGER = LoggerFactory.getLogger(ResourceTexture.class);
	
	public enum TextureColorMode {
		rgb, //!< red/green/blue data
		rgba //!< red/green/blue/alpha data
	}

	private static int[] textureIdBinding = { GL13.GL_TEXTURE0, GL13.GL_TEXTURE1, GL13.GL_TEXTURE2, GL13.GL_TEXTURE3,
			GL13.GL_TEXTURE4, GL13.GL_TEXTURE5, GL13.GL_TEXTURE6, GL13.GL_TEXTURE7, GL13.GL_TEXTURE8, GL13.GL_TEXTURE9,
			GL13.GL_TEXTURE10, GL13.GL_TEXTURE11, GL13.GL_TEXTURE12, GL13.GL_TEXTURE13, GL13.GL_TEXTURE14,
			GL13.GL_TEXTURE15, GL13.GL_TEXTURE16, GL13.GL_TEXTURE17, GL13.GL_TEXTURE18, GL13.GL_TEXTURE19,
			GL13.GL_TEXTURE20, GL13.GL_TEXTURE21, GL13.GL_TEXTURE22, GL13.GL_TEXTURE23, GL13.GL_TEXTURE24,
			GL13.GL_TEXTURE25, GL13.GL_TEXTURE26, GL13.GL_TEXTURE27, GL13.GL_TEXTURE28, GL13.GL_TEXTURE29,
			GL13.GL_TEXTURE30, GL13.GL_TEXTURE31 };;

	public static ResourceTexture createFromPng(final Uri uriTexture) {
		return createFromPng(uriTexture, 1);
	}

	public static ResourceTexture createFromPng(final Uri uriTexture, final int textureUnit) {
		ResourceTexture resource;
		Resource resource2;
		final String name = uriTexture.getValue();
		if (!name.isEmpty() && !name.equals("---")) {
			resource2 = getManager().localKeep(name);
		} else {
			LOGGER.error("Can not create a shader without a filaname");
			return null;
		}
		if (resource2 != null) {
			if (resource2 instanceof ResourceTexture) {
				resource2.keep();
				return (ResourceTexture) resource2;
			}
			LOGGER.error("Request resource file: '{}' With the wrong type (dynamic cast error)", name);
			System.exit(-1);
			return null;
		}
		resource = new ResourceTexture(uriTexture, textureUnit);
		ImageRawData decodedData;
		try {
			decodedData = ImageLoader.decodePngFile(uriTexture);
		} catch (final Exception e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			return null;
		}
		final ImageByteRGBA img = new ImageByteRGBA(decodedData.getWidth(), decodedData.getHeight());
		final ByteBuffer mlklmklm = decodedData.getBuffer();
		final byte[] elemData = new byte[mlklmklm.remaining()];
		mlklmklm.get(elemData);
		if (decodedData.isHasAlpha()) {
			for (int yyy = 0; yyy < decodedData.getHeight(); yyy++) {
				for (int xxx = 0; xxx < decodedData.getWidth(); xxx++) {
					img.setRByte(xxx, yyy, elemData[(yyy * decodedData.getWidth() + xxx) * 4 + 0]);
					img.setGByte(xxx, yyy, elemData[(yyy * decodedData.getWidth() + xxx) * 4 + 1]);
					img.setBByte(xxx, yyy, elemData[(yyy * decodedData.getWidth() + xxx) * 4 + 2]);
					img.setAByte(xxx, yyy, elemData[(yyy * decodedData.getWidth() + xxx) * 4 + 3]);
				}
			}
		} else {
			for (int yyy = 0; yyy < decodedData.getHeight(); yyy++) {
				for (int xxx = 0; xxx < decodedData.getWidth(); xxx++) {
					img.setAByte(xxx, yyy, (byte) 0xFF);
					img.setRByte(xxx, yyy, elemData[(yyy * decodedData.getWidth() + xxx) * 3 + 0]);
					img.setGByte(xxx, yyy, elemData[(yyy * decodedData.getWidth() + xxx) * 3 + 1]);
					img.setBByte(xxx, yyy, elemData[(yyy * decodedData.getWidth() + xxx) * 3 + 2]);
				}
			}
		}
		resource.setTexture(img, new Vector2i(decodedData.getWidth(), decodedData.getHeight()),
				(decodedData.isHasAlpha() ? TextureColorMode.rgba : TextureColorMode.rgb), textureUnit);
		resource.flush();
		return resource;
	}

	/**
	 * get the next power 2 if the input
	 * @param value Value that we want the next power of 2
	 * @return result value
	 */
	private static int nextP2(final int value) {
		int val = 1;
		for (int iii = 1; iii < 31; iii++) {
			if (value <= val) {
				return val;
			}
			val *= 2;
		}
		LOGGER.error("impossible CASE....");
		System.exit(-1);
		return val;
	}

	protected int texId = -1; //!< openGl textureID.
	// some image are not square  == > we need to sqared it to prevent some openGl api error the the displayable size is not all the time 0.0 . 1.0.
	protected Vector2i endPointSize = new Vector2i(-1, -1);
	// internal state of the openGl system.
	protected boolean loaded = false;
	// Image properties:
	// pointer on the image data.
	//private ByteBuffer data = null;
	protected ImageByteRGBA data = new ImageByteRGBA(32, 32);
	// size of the image data.
	private Vector2i size = new Vector2i(-1, -1);
	//!< Color space of the image.
	private TextureColorMode dataColorSpace = TextureColorMode.rgb;
	// number of lines and colomns in the texture (multiple texturing in a single texture)
	private int textureUnit = 0;

	protected ResourceTexture() {}

	protected ResourceTexture(final Uri filename, final int textureUnit) {
		super(filename.toString() + "__" + textureUnit);
		this.textureUnit = textureUnit;
	}

	public void bindForRendering(final int idTexture) {
		if (!this.loaded) {
			return;
		}
		GL13.glActiveTexture(textureIdBinding[idTexture]);
		GL11.glBindTexture(GL11.GL_TEXTURE_2D, this.texId);
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
		getManager().update(this);
	}

	public Vector2i getOpenGlSize() {
		return this.size;
	};

	public int getRendererId() {
		return this.texId;
	}

	public Vector2i getUsableSize() {
		return this.endPointSize;
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

	public void setTexture(
			final ImageByteRGBA data,
			final Vector2i size,
			final TextureColorMode dataColorSpace,
			final int textureUnit) {
		this.data = data;
		this.size = size;
		this.textureUnit = textureUnit;
		this.endPointSize = new Vector2i(size.x(), size.y());
		this.dataColorSpace = dataColorSpace;
		flush();
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

	// Gale internal API:
	@Override
	public boolean updateContext() {
		if (this.loaded) {
			return true;
		}
		// Request a new texture at openGl :
		this.texId = GL11.glGenTextures();
		GL13.glActiveTexture(this.textureUnit);
		GL11.glBindTexture(GL11.GL_TEXTURE_2D, this.texId);

		// All RGB bytes are aligned to each other and each component is 1 byte
		GL11.glPixelStorei(GL11.GL_UNPACK_ALIGNMENT, 1);
		LOGGER.debug("TEXTURE: add [{}]={} OGlId={}", getId(), this.size, this.texId);
		if (this.dataColorSpace == TextureColorMode.rgb) {
			OpenGL.glTexImage2D(0, GL11.GL_RGBA, this.size.x(), this.size.y(), 0, GL11.GL_RGBA, GL11.GL_UNSIGNED_BYTE,
					this.data.getRaw());
			//The local image has not RGB but only RGBA data ...
			//OpenGL.glTexImage2D(0, GL11.GL_RGBA, this.size.x(), this.size.y(), 0, GL11.GL_RGB, GL11.GL_UNSIGNED_BYTE, this.data.getRaw());
		} else {
			OpenGL.glTexImage2D(0, GL11.GL_RGBA, this.size.x(), this.size.y(), 0, GL11.GL_RGBA, GL11.GL_UNSIGNED_BYTE,
					this.data.getRaw());
		}
		// generate multi-texture mapping
		GL30.glGenerateMipmap(GL11.GL_TEXTURE_2D);

		// Setup the ST coordinate system
		GL11.glTexParameteri(GL11.GL_TEXTURE_2D, GL11.GL_TEXTURE_WRAP_S, GL11.GL_REPEAT);
		GL11.glTexParameteri(GL11.GL_TEXTURE_2D, GL11.GL_TEXTURE_WRAP_T, GL11.GL_REPEAT);

		// Setup what to do when the texture has to be scaled
		GL11.glTexParameteri(GL11.GL_TEXTURE_2D, GL11.GL_TEXTURE_MAG_FILTER, GL11.GL_NEAREST);
		GL11.glTexParameteri(GL11.GL_TEXTURE_2D, GL11.GL_TEXTURE_MIN_FILTER, GL11.GL_LINEAR_MIPMAP_LINEAR);
		// now the data is loaded
		this.loaded = true;
		return true;
	}
}
