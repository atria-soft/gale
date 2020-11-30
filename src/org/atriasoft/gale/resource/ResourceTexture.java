package org.atriasoft.gale.resource;

import java.nio.ByteBuffer;

import org.atriasoft.etk.Uri;
import org.atriasoft.etk.math.Vector2f;
import org.atriasoft.etk.math.Vector2i;
import org.atriasoft.gale.internal.Log;
import org.atriasoft.gale.backend3d.OpenGL;
import org.atriasoft.gale.tools.ImageLoader;
import org.atriasoft.gale.tools.ImageRawData;
import org.lwjgl.opengl.GL11;
import org.lwjgl.opengl.GL13;
import org.lwjgl.opengl.GL30;

public class ResourceTexture extends Resource {
	private static int[] textureIdBinding = {
			GL13.GL_TEXTURE0,
			GL13.GL_TEXTURE1,
			GL13.GL_TEXTURE2,
			GL13.GL_TEXTURE3,
			GL13.GL_TEXTURE4,
			GL13.GL_TEXTURE5,
			GL13.GL_TEXTURE6,
			GL13.GL_TEXTURE7,
			GL13.GL_TEXTURE8,
			GL13.GL_TEXTURE9,
			GL13.GL_TEXTURE10,
			GL13.GL_TEXTURE11,
			GL13.GL_TEXTURE12,
			GL13.GL_TEXTURE13,
			GL13.GL_TEXTURE14,
			GL13.GL_TEXTURE15,
			GL13.GL_TEXTURE16,
			GL13.GL_TEXTURE17,
			GL13.GL_TEXTURE18,
			GL13.GL_TEXTURE19,
			GL13.GL_TEXTURE20,
			GL13.GL_TEXTURE21,
			GL13.GL_TEXTURE22,
			GL13.GL_TEXTURE23,
			GL13.GL_TEXTURE24,
			GL13.GL_TEXTURE25,
			GL13.GL_TEXTURE26,
			GL13.GL_TEXTURE27,
			GL13.GL_TEXTURE28,
			GL13.GL_TEXTURE29,
			GL13.GL_TEXTURE30,
			GL13.GL_TEXTURE31
	};
	public enum TextureColorMode {
		rgb, //!< red/green/blue data
		rgba //!< red/green/blue/alpha data
	};
	protected int texId = -1; //!< openGl textureID.
	protected Vector2f endPointSize = new Vector2f(-1, -1); //!< some image are not square  == > we need to sqared it to prevent some openGl api error the the displayable size is not all the time 0.0 . 1.0.
	protected boolean loaded = false; //!< internal state of the openGl system.
	// Image properties:
	private ByteBuffer data = null; //!< pointer on the image data.
	private Vector2i size = new Vector2i(-1,-1); //!< size of the image data.
	private TextureColorMode dataColorSpace = TextureColorMode.rgb; //!< Color space of the image.
	private int textureUnit = 0; // number of lines and colomns in the texture (multiple texturing in a single texture)
	private String filename = "";
	/**
	 * @brief get the next power 2 if the input
	 * @param value Value that we want the next power of 2
	 * @return result value
	 */
	private static int nextP2(int value) {
		int val=1;
		for (int iii=1; iii<31; iii++) {
			if (value <= val) {
				return val;
			}
			val *=2;
		}
		Log.critical("impossible CASE....");
		return val;
	}
	// Public API:
	protected ResourceTexture(String filename, int textureUnit)  {
		super(filename + "__" + textureUnit);
		this.filename = filename;
		this.textureUnit = textureUnit;
		addResourceType("gale::resource::Texture");
	}
	protected ResourceTexture(Uri filename, int textureUnit)  {
		super(filename + "__" + textureUnit);
		this.filename = filename.get();
		this.textureUnit = textureUnit;
		addResourceType("gale::resource::Texture");
	}
	protected ResourceTexture()  {
		super();
		addResourceType("gale::resource::Texture");
	}
	public void cleanUp() {
		removeContext();
	}
	// Gale internal API:
	@Override
	public boolean updateContext() {
		if (this.loaded == true) {
			return true;
		}
		// Request a new texture at openGl :
		texId = GL11.glGenTextures();
        GL13.glActiveTexture(textureUnit);
        GL11.glBindTexture(GL11.GL_TEXTURE_2D, texId);
         
        // All RGB bytes are aligned to each other and each component is 1 byte
        GL11.glPixelStorei(GL11.GL_UNPACK_ALIGNMENT, 1);
		Log.info("TEXTURE: add [" + getId() + "]=" + this.size + " OGlId=" + this.texId);
		if (dataColorSpace == TextureColorMode.rgb) {
			GL11.glTexImage2D(GL11.GL_TEXTURE_2D, 0, GL11.GL_RGBA, this.size.x, this.size.y, 0, GL11.GL_RGB, GL11.GL_UNSIGNED_BYTE, data);
		} else {
			GL11.glTexImage2D(GL11.GL_TEXTURE_2D, 0, GL11.GL_RGBA, this.size.x, this.size.y, 0, GL11.GL_RGBA, GL11.GL_UNSIGNED_BYTE, data);
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
	public void removeContext() {
		if (true == this.loaded) {
			// Request remove texture ...
			Log.info("TEXTURE: Rm [" + getId() + "] texId=" + this.texId);
			// TODO Check if we are in the correct thread
			GL11.glDeleteTextures(this.texId);
			this.loaded = false;
		}
	}
	public void removeContextToLate() {
		this.loaded = false;
		this.texId = -1;
	}
	public int getRendererId()  {
		return this.texId;
	};
	public Vector2f getUsableSize()  {
		return this.endPointSize;
	};
	public Vector2i getOpenGlSize()  {
		return this.size;
	};
	// flush the data to send it at the openGl system
	public void flush() {
		// request to the manager to be call at the next update ...
		getManager().update(this);
	}
	public void setTexture(ByteBuffer data,
				Vector2i size,
				TextureColorMode dataColorSpace,
                int textureUnit) {
		this.data = data;
		this.size = size;
		this.textureUnit = textureUnit;
		this.endPointSize.x = (float)size.x;
		this.endPointSize.y = (float)size.y;
		this.dataColorSpace = dataColorSpace;
		flush();
	}
	public void bindForRendering(int idTexture) {
		if (this.loaded == false) {
			return;
		}
		GL13.glActiveTexture(textureIdBinding[idTexture]);
		GL11.glBindTexture(GL11.GL_TEXTURE_2D, this.texId);
		if (dataColorSpace == TextureColorMode.rgb) {
			OpenGL.enable(OpenGL.Flag.flag_cullFace);
			OpenGL.enable(OpenGL.Flag.flag_back);
		}
	}
	public void unBindForRendering() {
		if (this.loaded == false) {
			return;
		}
		if (dataColorSpace == TextureColorMode.rgb) {
			OpenGL.disable(OpenGL.Flag.flag_cullFace);
			OpenGL.disable(OpenGL.Flag.flag_back);
		}
	}

	public static ResourceTexture createFromPng(Uri uriTexture) {
		return createFromPng(uriTexture, 1);
	}
	public static ResourceTexture createFromPng(Uri uriTexture, int textureUnit) {
		ResourceTexture resource;
		Resource resource2;
		String name = uriTexture.getValue();
		if (name.isEmpty() == false && name != "---") {
			resource2 = getManager().localKeep(name);
		} else {
			Log.error("Can not create a shader without a filaname");
			return null;
		}
		if (resource2 != null) {
			if (resource2 instanceof ResourceTexture) {
				resource2.keep();
				return (ResourceTexture)resource2;
			}
			Log.critical("Request resource file : '" + name + "' With the wrong type (dynamic cast error)");
			return null;
		}
		resource = new ResourceTexture(uriTexture, textureUnit);
		ImageRawData decodedData = ImageLoader.decodePngFile(uriTexture);
		resource.setTexture(decodedData.getBuffer(),
				new Vector2i(decodedData.getWidth(), decodedData.getHeight()),
				(decodedData.isHasAlpha() == true?TextureColorMode.rgba:TextureColorMode.rgb),
                textureUnit);
		if (resource.resourceHasBeenCorectlyInit() == false) {
			Log.critical("resource Is not correctly init : ResourceProgram" );
			return null;
		}
		resource.flush();
		return resource;
	}
	
}
