package org.atriasoft.gale.resource;

import java.nio.charset.StandardCharsets;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.HexFormat;
import java.util.Objects;
import java.util.function.Supplier;

import org.atriasoft.gale.backend3d.OpenGL.ShaderType;

/**
 * A shader given as text instead of a file: its source comes from a supplier, called once when the object is
 * made (the text first compiled) and again at each {@link ResourceShader#reload()}, so that a source assembled
 * from several files shows the edits of any of them. The resource is keyed by the name and the hash of the first
 * text: two shaders with the same name and the same text share one resource.
 */
public final class ShaderText {
	private final String name;
	private final ShaderType type;
	private final Supplier<String> supplier;
	private final String firstText;
	private final String key;

	private ShaderText(final String name, final ShaderType type, final Supplier<String> supplier,
			final String firstText) {
		this.name = name;
		this.type = type;
		this.supplier = supplier;
		this.firstText = firstText;
		this.key = name + "#" + hash(firstText);
	}

	/**
	 * @param name label of the shader (journal and resource name), for example its source Uri
	 * @param type vertex or fragment
	 * @param supplier gives the source text; it may throw to report a problem, the exception goes to the caller
	 *        at the first call
	 * @return the shader text, the supplier called once
	 */
	public static ShaderText of(final String name, final ShaderType type, final Supplier<String> supplier) {
		Objects.requireNonNull(name, "name");
		Objects.requireNonNull(type, "type");
		Objects.requireNonNull(supplier, "supplier");
		final String text = supplier.get();
		if (text == null) {
			throw new IllegalArgumentException("No source for the shader " + name);
		}
		return new ShaderText(name, type, supplier, text);
	}

	/** Shader of a fixed text. */
	public static ShaderText of(final String name, final ShaderType type, final String text) {
		Objects.requireNonNull(text, "text");
		return of(name, type, () -> text);
	}

	private static String hash(final String text) {
		try {
			final byte[] digest = MessageDigest.getInstance("SHA-1").digest(text.getBytes(StandardCharsets.UTF_8));
			return HexFormat.of().formatHex(digest, 0, 8);
		} catch (final NoSuchAlgorithmException e) {
			throw new IllegalStateException(e);
		}
	}

	public String name() {
		return this.name;
	}

	public ShaderType type() {
		return this.type;
	}

	/** Name and hash of the first text: the name of the resource. */
	public String key() {
		return this.key;
	}

	/** The text given when this object was made. */
	public String firstText() {
		return this.firstText;
	}

	/** The text now: the supplier called again. */
	String currentText() {
		return this.supplier.get();
	}

	@Override
	public String toString() {
		return this.key;
	}
}
