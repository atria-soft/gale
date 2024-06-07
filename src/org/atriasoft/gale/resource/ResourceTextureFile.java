package org.atriasoft.gale.resource;

import org.atriasoft.egami.ImageByte;
import org.atriasoft.etk.Tools;
import org.atriasoft.etk.Uri;
import org.atriasoft.etk.math.Vector2i;
import org.atriasoft.iogami.IOgami;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

// TODO : Change tis file name ...

public class ResourceTextureFile extends ResourceTexture2 {
	static final Logger LOGGER = LoggerFactory.getLogger(ResourceTextureFile.class);
	public static Vector2i sizeAuto = new Vector2i(-1, -1);
	public static Vector2i sizeDefault = Vector2i.ZERO;
	
	public static ResourceTextureFile create(final Uri filename) {
		return ResourceTextureFile.create(filename, ResourceTextureFile.sizeAuto);
	}
	
	public static ResourceTextureFile create(final Uri filename, final Vector2i size) {
		return ResourceTextureFile.create(filename, size, ResourceTextureFile.sizeAuto);
	}
	
	/**
	 * keep the resource pointer.
	 * @note Never free this pointer by your own...
	 * @param uri Name of the image file.
	 * @param size size of the image (usefull when loading .svg to
	 *            automatic rescale)
	 * @param sizeRegister size register in named (When you preaload the images
	 *            the size write here will be )
	 * @return pointer on the resource or null if an error occured.
	 */
	public static ResourceTextureFile create(final Uri uri, final Vector2i inSize, final Vector2i sizeRegister) {
		LOGGER.trace("KEEP: TextureFile: '" + uri + "' size=" + inSize + " sizeRegister=" + sizeRegister);
		Vector2i size = inSize;
		if (uri == null) {
			return new ResourceTextureFile();
		}
		if (size.x() == 0) {
			size = size.withX(-1);
			// LOGGER.error("Error Request the image size.x() =0 ???");
		}
		if (size.y() == 0) {
			size = size.withY(-1);
			// LOGGER.error("Error Request the image size.y() =0 ???");
		}
		if (!uri.getExtention().toLowerCase().contentEquals("svg")) {
			size = ResourceTextureFile.sizeAuto;
		}
		if (size.x() > 0 && size.y() > 0) {
			LOGGER.trace("     == > specific size : " + size);
			size = new Vector2i(Tools.nextP2(size.x()), Tools.nextP2(size.y()));
			if (!sizeRegister.equals(ResourceTextureFile.sizeAuto)) {
				if (!sizeRegister.equals(ResourceTextureFile.sizeDefault)) {
					// tmpFilename.getQuery().set("x", "" + size.x));
					// tmpFilename.getQuery().set("y", "" + size.y));
				}
			}
		}
		
		LOGGER.trace("KEEP: TextureFile: '" + uri + "' new size=" + size);
		final Resource object2 = Resource.getManager().localKeep(uri.toString());
		if (object2 != null) {
			if (object2 instanceof final ResourceTextureFile out) {
				object2.keep();
				return out;
			}
			LOGGER.error("Request resource file : '" + uri + "' With the wrong type (dynamic cast error)");
			System.exit(-1);
			return null;
		}
		LOGGER.debug("CREATE: TextureFile: '" + uri + "' size=" + size);
		// need to crate a new one ...
		final ResourceTextureFile object = new ResourceTextureFile(uri.toString(), uri, size);
		Resource.getManager().localAdd(object);
		return object;
	}
	
	protected ResourceTextureFile() {}
	
	protected ResourceTextureFile(final String genName, final Uri uri, final Vector2i size) {
		super(genName);
		LOGGER.debug("create a new resource::Image : genName=" + genName + " uri=" + uri + " size=" + size);
		final ImageByte tmp = IOgami.load(uri, size);
		if (tmp == null) {
			LOGGER.error("Can not load the file : " + uri);
			return;
		}
		set(tmp);
	}
	
	public Vector2i getRealSize() {
		return this.realImageSize;
	}
	
}
