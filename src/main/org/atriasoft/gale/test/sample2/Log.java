package org.atriasoft.gale.test.sample2;

public class Log {
	private Log() {}
	private static final String LIBNAME = "Sample2";
	public static void print(String data) {
		System.out.println(data);
	}
	/**
	 * Journal a fatal error and throw it: the caller (or the context, for an error
	 * in the GUI loop) ends the application, never this sample through System.exit.
	 * @param data Message of the error.
	 * @throws IllegalStateException always.
	 */
	public static void critical(final String data) {
		System.out.println("[C] " + LIBNAME + " | " + data);
		throw new IllegalStateException(data);
	}
	public static void error(String data) {
		System.out.println("[E] " + LIBNAME + " | " + data);
	}
	public static void warning(String data) {
		System.out.println("[W] " + LIBNAME + " | " + data);
	}
	public static void info(String data) {
		System.out.println("[I] " + LIBNAME + " | " + data);
	}
	public static void debug(String data) {
		System.out.println("[D] " + LIBNAME + " | " + data);
	}
	public static void verbose(String data) {
		System.out.println("[V] " + LIBNAME + " | " + data);
	}
	public static void todo(String data) {
		System.out.println("[TODO] " + LIBNAME + " | " + data);
	}
}
