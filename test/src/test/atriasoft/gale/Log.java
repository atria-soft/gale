package test.atriasoft.gale;

import org.atriasoft.reggol.LogLevel;
import org.atriasoft.reggol.Logger;

public class Log {
	private static final String LIB_NAME = "gale-test";
	private static final String LIB_NAME_DRAW = Logger.getDrawableName(LIB_NAME);
	private static final boolean PRINT_CRITICAL = Logger.getNeedPrint(LIB_NAME, LogLevel.CRITICAL);
	private static final boolean PRINT_ERROR = Logger.getNeedPrint(LIB_NAME, LogLevel.ERROR);
	private static final boolean PRINT_WARNING = Logger.getNeedPrint(LIB_NAME, LogLevel.WARNING);
	private static final boolean PRINT_INFO = Logger.getNeedPrint(LIB_NAME, LogLevel.INFO);
	private static final boolean PRINT_DEBUG = Logger.getNeedPrint(LIB_NAME, LogLevel.DEBUG);
	private static final boolean PRINT_VERBOSE = Logger.getNeedPrint(LIB_NAME, LogLevel.VERBOSE);
	private static final boolean PRINT_TODO = Logger.getNeedPrint(LIB_NAME, LogLevel.TODO);
	private static final boolean PRINT_PRINT = Logger.getNeedPrint(LIB_NAME, LogLevel.PRINT);

	private Log() {}

	public static void print(String data) {
		if (PRINT_PRINT)
			Logger.print(LIB_NAME_DRAW, data);
	}

	public static void critical(String data) {
		if (PRINT_CRITICAL)
			Logger.critical(LIB_NAME_DRAW, data);
	}

	public static void error(String data) {
		if (PRINT_ERROR)
			Logger.error(LIB_NAME_DRAW, data);
	}

	public static void warning(String data) {
		if (PRINT_WARNING)
			Logger.warning(LIB_NAME_DRAW, data);
	}

	public static void info(String data) {
		if (PRINT_INFO)
			Logger.info(LIB_NAME_DRAW, data);
	}

	public static void debug(String data) {
		if (PRINT_DEBUG)
			Logger.debug(LIB_NAME_DRAW, data);
	}

	public static void verbose(String data) {
		if (PRINT_VERBOSE)
			Logger.verbose(LIB_NAME_DRAW, data);
	}

	public static void todo(String data) {
		if (PRINT_TODO)
			Logger.todo(LIB_NAME_DRAW, data);
	}
}
