/**
 * Basic module interface.
 *
 * @author Edouard DUPIN
 */

open module org.atriasoft.gale {
	exports org.atriasoft.gale;
	exports org.atriasoft.gale.backend3d;
	exports org.atriasoft.gale.context;
	// exports org.atriasoft.gale.context.JOGL;
	exports org.atriasoft.gale.context.LWJG_AWT;
	exports org.atriasoft.gale.key;
	exports org.atriasoft.gale.resource;
	
	requires transitive org.atriasoft.etk;
	requires transitive org.atriasoft.egami;
	
	requires transitive org.lwjgl;
	requires transitive org.lwjgl.natives;
	requires transitive org.lwjgl.glfw;
	requires transitive org.lwjgl.glfw.natives;
	requires transitive org.lwjgl.assimp;
	requires transitive org.lwjgl.assimp.natives;
	requires transitive org.lwjgl.stb;
	requires transitive org.lwjgl.stb.natives;
	requires transitive org.lwjgl.jawt;
	requires transitive org.lwjgl.opengl;
	requires transitive org.lwjgl.opengl.natives;
	
	requires transitive java.desktop;
	requires transitive org.atriasoft.pngdecoder;
	requires transitive lwjgl3.awt;
	requires org.atriasoft.reggol;
	requires org.atriasoft.iogami;
}
