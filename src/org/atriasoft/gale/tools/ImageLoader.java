package org.atriasoft.gale.tools;

import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteBuffer;

import org.atriasoft.etk.Uri;

import de.matthiasmann.twl.utils.PNGDecoder;
import de.matthiasmann.twl.utils.PNGDecoder.Format;

public class ImageLoader {
	private ImageLoader() {}

    public static ImageRawData decodePngFile(Uri filename) {
    	ByteBuffer buf = null;
        int tWidth = 0;
        int tHeight = 0;
        boolean hasAlpha = false;
        try {
            // Open the PNG file as an InputStream
            InputStream in = new FileInputStream(filename.get());
            // Link the PNG decoder to this stream
            PNGDecoder decoder = new PNGDecoder(in);
            // Get the width and height of the texture
            tWidth = decoder.getWidth();
            tHeight = decoder.getHeight();
            hasAlpha = decoder.hasAlpha();
            // Decode the PNG file in a ByteBuffer
            if (hasAlpha == true) {
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
        } catch (IOException e) {
            e.printStackTrace();
            System.err.println("try to load texture " + filename + ", didn't work");
            System.exit(-1);
        }
        return new ImageRawData(buf, tWidth, tHeight, hasAlpha);
    }
}
