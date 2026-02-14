package org.atriasoft.gale.tools;

import java.awt.image.BufferedImage;
import java.io.IOException;
import java.io.InputStream;
import java.nio.ByteBuffer;

import javax.imageio.ImageIO;

import org.atriasoft.etk.Uri;

public class ImageLoader {
	public static ImageRawData decodePngFile(final Uri filename) throws Exception {
		try (InputStream in = Uri.getStream(filename)) {
			if (in == null) {
				throw new Exception("Failed to get stream for: " + filename);
			}
			final BufferedImage image = ImageIO.read(in);
			if (image == null) {
				throw new Exception("Unsupported image format or corrupted data: " + filename);
			}
			final int width = image.getWidth();
			final int height = image.getHeight();
			final boolean hasAlpha = image.getColorModel().hasAlpha();
			if (hasAlpha) {
				final ByteBuffer buf = ByteBuffer.allocateDirect(4 * width * height);
				for (int yyy = 0; yyy < height; yyy++) {
					for (int xxx = 0; xxx < width; xxx++) {
						final int argb = image.getRGB(xxx, yyy);
						buf.put((byte) ((argb >> 16) & 0xFF));
						buf.put((byte) ((argb >> 8) & 0xFF));
						buf.put((byte) (argb & 0xFF));
						buf.put((byte) ((argb >> 24) & 0xFF));
					}
				}
				buf.flip();
				return new ImageRawData(buf, width, height, true);
			} else {
				final ByteBuffer buf = ByteBuffer.allocateDirect(3 * width * height);
				for (int yyy = 0; yyy < height; yyy++) {
					for (int xxx = 0; xxx < width; xxx++) {
						final int rgb = image.getRGB(xxx, yyy);
						buf.put((byte) ((rgb >> 16) & 0xFF));
						buf.put((byte) ((rgb >> 8) & 0xFF));
						buf.put((byte) (rgb & 0xFF));
					}
				}
				buf.flip();
				return new ImageRawData(buf, width, height, false);
			}
		} catch (final IOException e) {
			e.printStackTrace();
			System.err.println("try to load texture " + filename + ", didn't work");
			throw new Exception("Failed to load image: " + filename, e);
		}
	}

	private ImageLoader() {}
}
