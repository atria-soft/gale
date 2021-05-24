/** @file
 * @author Edouard DUPIN
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license MPL v2.0 (see license file)
 */
package org.atriasoft.gale.resource;

import org.atriasoft.egami.ImageByte;
import org.atriasoft.etk.Tools;
import org.atriasoft.etk.Uri;
import org.atriasoft.etk.math.Vector2i;
import org.atriasoft.gale.internal.Log;
import org.atriasoft.iogami.IOgami;

// TODO : Change tis file name ...

public class ResourceTextureFile extends ResourceTexture2 {
	public static Vector2i sizeAuto = new Vector2i(-1, -1);
	public static Vector2i sizeDefault = new Vector2i(0, 0);
	
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
		Log.verbose("KEEP: TextureFile: '" + uri + "' size=" + inSize + " sizeRegister=" + sizeRegister);
		Vector2i size = inSize;
		if (uri == null) {
			return new ResourceTextureFile();
		}
		if (size.x() == 0) {
			size = size.withX(-1);
			// Log.error("Error Request the image size.x() =0 ???");
		}
		if (size.y() == 0) {
			size = size.withY(-1);
			// Log.error("Error Request the image size.y() =0 ???");
		}
		if (!uri.getExtention().toLowerCase().contentEquals("svg")) {
			size = ResourceTextureFile.sizeAuto;
		}
		if (size.x() > 0 && size.y() > 0) {
			Log.verbose("     == > specific size : " + size);
			size = new Vector2i(Tools.nextP2(size.x()), Tools.nextP2(size.y()));
			if (!sizeRegister.equals(ResourceTextureFile.sizeAuto)) {
				if (!sizeRegister.equals(ResourceTextureFile.sizeDefault)) {
					// tmpFilename.getQuery().set("x", "" + size.x));
					// tmpFilename.getQuery().set("y", "" + size.y));
				}
			}
		}
		
		Log.verbose("KEEP: TextureFile: '" + uri + "' new size=" + size);
		final Resource object2 = Resource.getManager().localKeep(uri.toString());
		if (object2 != null) {
			if (object2 instanceof ResourceTextureFile out) {
				object2.keep();
				return out;
			}
			Log.critical("Request resource file : '" + uri + "' With the wrong type (dynamic cast error)");
			return null;
		}
		Log.debug("CREATE: TextureFile: '" + uri + "' size=" + size);
		// need to crate a new one ...
		ResourceTextureFile object = new ResourceTextureFile(uri.toString(), uri, size);
		Resource.getManager().localAdd(object);
		return object;
	}
	
	protected ResourceTextureFile() {}
	
	protected ResourceTextureFile(final String genName, final Uri uri, final Vector2i size) {
		super(genName);
		Log.debug("create a new resource::Image : genName=" + genName + " uri=" + uri + " size=" + size);
		final ImageByte tmp = IOgami.load(uri, size);
		if (tmp == null) {
			Log.error("Can not load the file : " + uri);
			return;
		}
		set(tmp);
	}
	
	public Vector2i getRealSize() {
		return this.realImageSize;
	}
	
}
