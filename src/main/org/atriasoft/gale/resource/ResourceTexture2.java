/** @file
 * @author Edouard DUPIN
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license MPL v2.0 (see license file)
 */
package org.atriasoft.gale.resource;

import org.atriasoft.egami.ImageByte;
import org.atriasoft.egami.ImageByteRGBA;
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
		LOGGER.trace("KEEP: Resource Texture: " + uri);
		final Resource object2 = Resource.getManager().localKeep(uri);
		if (object2 != null) {
			if (object2 instanceof final ResourceTexture2 tmpp) {
				return tmpp;
			}
			LOGGER.error("Request resource file : '" + uri + "' With the wrong type (dynamic cast error)");
			System.exit(-1);
			return null;
		}
		LOGGER.trace("CREATE: new Texture: " + uri);
		return new ResourceTexture2(uri);
	}
	
	public static ResourceTexture2 createNamed(final String uri) {
		LOGGER.trace("KEEP: Resource Texture Named: " + uri);
		final Resource object2 = Resource.getManager().localKeep(uri);
		if (object2 != null) {
			if (object2 instanceof final ResourceTexture2 tmpp) {
				return tmpp;
			}
			LOGGER.error("Request resource file : '" + uri + "' With the wrong type (dynamic cast error)");
			System.exit(-1);
			return null;
		}
		LOGGER.debug("CREATE: new Texture Named: " + uri);
		return new ResourceTexture2(uri);
	}
	
	/*
	 * public static ResourceTexture2 createFromPng(final Uri uriTexture) { return
	 * createFromPng(uriTexture, 1); }
	 *
	 * public static ResourceTexture2 createFromPng(final Uri uriTexture, final int
	 * textureUnit) { ResourceTexture2 resource; Resource resource2; final String
	 * name = uriTexture.getValue(); if (name.isEmpty() == false && name != "---") {
	 * resource2 = getManager().localKeep(name); } else {
	 * LOGGER.error("Can not create a shader without a filaname"); return null; } if
	 * (resource2 != null) { if (resource2 instanceof ResourceTexture2) {
	 * resource2.keep(); return (ResourceTexture2) resource2; }
	 * LOGGER.critical("Request resource file : '" + name +
	 * "' With the wrong type (dynamic cast error)");
	 * System.exit(-1);
	 * return null; } resource = new
	 * ResourceTexture2(uriTexture, textureUnit); final ImageRawData decodedData =
	 * ImageLoader.decodePngFile(uriTexture);
	 * resource.setTexture(decodedData.getBuffer(), new
	 * Vector2i(decodedData.getWidth(), decodedData.getHeight()),
	 * (decodedData.isHasAlpha() == true ? TextureColorMode.rgba :
	 * TextureColorMode.rgb), textureUnit); resource.flush(); return resource; }
	 */
	
	// openGl Context properties :
	protected ImageByte data = new ImageByteRGBA(32, 32);
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
	
	/*
	 * public void bindForRendering(final int idTexture) { if (this.loaded == false)
	 * { return; } GL13.glActiveTexture(textureIdBinding[idTexture]);
	 * GL11.glBindTexture(GL11.GLTEXTURE2D, this.texId); if (this.dataColorSpace
	 * == TextureColorMode.rgb) { OpenGL.enable(OpenGL.Flag.flagcullFace);
	 * OpenGL.enable(OpenGL.Flag.flagback); } }
	 */
	
	public ResourceTexture2(final Uri filename) {
		super(filename);
	}
	
	public void bindForRendering(final int idTexture) {
		if (!this.loaded) {
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
	public ImageByte get() {
		return this.data;
	}
	
	public Vector2i getOpenGlSize() {
		return this.data.getSize();
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
			LOGGER.info("TEXTURE: Rm [" + getId() + "] texId=" + this.texId);
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
	 * @note It will resize in square2 if needed by the system.
	 * @param image Image to set.
	 */
	public synchronized void set(final ImageByte image) {
		System.out.println("Set a new image in a texture:");
		System.out.println("    size=" + image.getSize());
		this.data = image;
		this.realImageSize = this.data.getSize();
		// Disable compatibility size for embended ...
		//		final Vector2i compatibilityHWSize = new Vector2i(Tools.nextP2(this.realImageSize.x()), Tools.nextP2(this.realImageSize.y()));
		//		if (!this.realImageSize.equals(compatibilityHWSize)) {
		//			LOGGER.warn("RESIZE Image for HArwareCompatibility:" + this.realImageSize + " => " + compatibilityHWSize);
		//			this.data.resize(compatibilityHWSize.x(), compatibilityHWSize.y());
		//		}
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
	public synchronized void setImageSize(Vector2i newSize) {
		newSize = new Vector2i(Tools.nextP2(newSize.x()), Tools.nextP2(newSize.y()));
		this.data.resize(newSize.x(), newSize.y());
	}
	
	/**
	 * Set the repeate mode of the images if UV range is out of [0..1]
	 * @param value Value of the new repeate mode
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
		//final Steady tic = Steady.now();
		/*
		 * TODO : use unlockable synchronized ... if (lock.tryLock() == false) { //Lock
		 * error ==> try later ... return false; }
		 */
		final int typeObject = this.data.hasAlpha() ? OpenGL.GL_RGBA : OpenGL.GL_RGB;
		final int sizeObject = OpenGL.GL_UNSIGNED_BYTE;
		if (this.loaded) {
			if (this.lastTypeObject != typeObject || this.lastSizeObject != sizeObject
					|| !this.lastSize.equals(this.data.getSize())) {
				LOGGER.trace("TEXTURE: Rm [" + getId() + "] texId=" + this.texId);
				OpenGL.glDeleteTextures(this.texId);
				this.loaded = false;
			}
		}
		if (!this.loaded) {
			// Request a new texture at openGl :
			this.texId = OpenGL.glGenTextures();
			this.lastSize = this.data.getSize();
			this.lastTypeObject = typeObject;
			this.lastSizeObject = sizeObject;
			LOGGER.debug("TEXTURE: add [" + getId() + "]=" + this.data.getSize() + "=>" + this.data.getGPUSize()
					+ " OGlId=" + this.texId + " type=" + this.data.getClass().getCanonicalName());
		} else {
			LOGGER.debug("TEXTURE: update [" + getId() + "]=" + this.data.getSize() + "=>" + this.data.getGPUSize()
					+ " OGlId=" + this.texId + " type=" + this.data.getClass().getCanonicalName());
		}
		// in all case we set the texture properties :
		// TODO check error ???
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
		// glPixelStorei(GLUNPACKALIGNMENT,1);
		//final Steady toc1 = Steady.now();
		//LOGGER.trace("    BIND                 ==> " + toc1.less(tic));
		// egami::store(this.data, String("~/texture") + etk::toString(getId()) + ".bmp");
		if (!this.loaded) {
			OpenGL.glTexImage2D(0, // Level
					typeObject, // Format internal
					this.data.getWidth(), this.data.getHeight(), 0, // Border
					typeObject, // format
					sizeObject, // type
					this.data.getRaw());
			
		} else {
			OpenGL.glTexSubImage2D(0, // Level
					0, // x offset
					0, // y offset
					this.data.getWidth(), this.data.getHeight(), typeObject, // format
					sizeObject, // type
					this.data.getRaw());
		}
		// now the data is loaded
		this.loaded = true;
		// final Steady toc = Steady.now();
		// LOGGER.error(" updateContext [STOP] ==> " + (toc - toc1));
		return true;
	}
	
}
