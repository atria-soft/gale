package org.atriasoft.gale.tools;

import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteBuffer;

import org.atriasoft.etk.Uri;

import org.atriasoft.pngdecoder.PNGDecoder;
import org.atriasoft.pngdecoder.PNGDecoder.Format;

public class ImageLoader {
	public static ImageRawData decodePngFile(final Uri filename) {
		ByteBuffer buf = null;
		int tWidth = 0;
		int tHeight = 0;
		boolean hasAlpha = false;
		try {
			// Open the PNG file as an InputStream
			final InputStream in = Uri.getStream(filename);
			// Link the PNG decoder to this stream
			final PNGDecoder decoder = new PNGDecoder(in);
			// Get the width and height of the texture
			tWidth = decoder.getWidth();
			tHeight = decoder.getHeight();
			hasAlpha = decoder.hasAlpha();
			// Decode the PNG file in a ByteBuffer
			if (hasAlpha) {
				buf = ByteBuffer.allocateDirect(4 * decoder.getWidth() * decoder.getHeight());
				//decoder.decodeFlipped(buf, decoder.getWidth() * 4, Format.RGBA);
				decoder.decode(buf, decoder.getWidth() * 4, Format.RGBA);
			} else {
				buf = ByteBuffer.allocateDirect(3 * decoder.getWidth() * decoder.getHeight());
				//decoder.decodeFlipped(buf, decoder.getWidth() * 4, Format.RGBA);
				decoder.decode(buf, decoder.getWidth() * 3, Format.RGB);
			}
			buf.flip();
			in.close();
		} catch (final IOException e) {
			e.printStackTrace();
			System.err.println("try to load texture " + filename + ", didn't work");
			System.exit(-1);
		}
		return new ImageRawData(buf, tWidth, tHeight, hasAlpha);
	}
	
	private ImageLoader() {}
}
