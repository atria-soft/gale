package test.atriasoft.gale;

import org.atriasoft.reggol.LogLevel;
import org.atriasoft.reggol.Logger;

public class Log2 {
	private static final String LIB_NAME = "gale-test-2";
	private static final String LIB_NAME_DRAW = Logger.getDrawableName(LIB_NAME);
	private static final boolean PRINT_DEBUG = Logger.getNeedPrint(LIB_NAME, LogLevel.DEBUG);

	private Log2() {}

	public static void debug(String data) {
		if (PRINT_DEBUG)
			Logger.debug(LIB_NAME_DRAW, data);
	}

}
