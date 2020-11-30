package org.atriasoft.gale.test.sample1;

public class Log {
	private Log() {}
	private static final String LIBNAME = "Sample1";
	public static void print(String data) {
		System.out.println(data);
	}
	public static void critical(String data) {
		System.out.println("[C] " + LIBNAME + " | " + data);
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
