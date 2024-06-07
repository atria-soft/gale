package org.atriasoft.gale;

import org.atriasoft.etk.Uri;
import org.atriasoft.gale.context.GaleContext;
//import org.atriasoft.gale.context.JOGL.ContextJOGL;
import org.atriasoft.gale.context.LWJG_AWT.ContextLWJGLAWT;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class Gale {
	static final Logger LOGGER = LoggerFactory.getLogger(Gale.class);
	
	public static GaleContext getContext() {
		// TODO Auto-generated method stub
		return GaleContext.getContext();
	}

	/**
	 * get GALE version
	 * @return The string that describe gale version
	 */
	public static String getVersion() {
		return "J-0.5";
	}

	public static void init() {
		Uri.addLibrary("gale", Gale.class, "/resources/gale/");
	}

	/**
	 * This is the only one things the User might done in his main();
	 * @note To answare you before you ask the question, this is really simple:
	 *       Due to the fect that the current system is multiple-platform, you "main"
	 *       Does not exist in the android platform, then gale call other start
	 *       and stop function, to permit to have only one code
	 * @note The main can not be in the gale, due to the fact thet is an librairy
	 * @param _application just created instance of the applicationo
	 * @param _argc Standard argc
	 * @param _argv Standard argv
	 * @return normal error int for the application error management
	 */
	public static int run(final GaleApplication application, final String[] arg) {
		init();
		//etk::init(_argc, _argv);
		GaleContext context = null;
		final String request = "";

		//context = ContextLWJGL.create(application, arg);
		//context = ContextJOGL.create(application, arg);
		context = ContextLWJGLAWT.create(application, arg);
		if (context == null) {
			LOGGER.error("Can not allocate the interface of the GUI ...");
			return -1;
		}
		return context.run();
	}

	private Gale() {}
}
