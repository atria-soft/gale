#!/usr/bin/python
import realog.debug as debug
import lutin.tools as tools
import realog.debug as debug
import lutin.image as image
import os
import lutin.multiprocess as lutinMultiprocess


def get_type():
	return "LIBRARY_DYNAMIC"

def get_desc():
	return "Ewol Tool Kit"

def get_licence():
	return "MPL-2"

def get_compagny_type():
	return "org"

def get_compagny_name():
	return "atria-soft"

#def get_maintainer():
#	return "authors.txt"

#def get_version():
#	return "version.txt"

def configure(target, my_module):

	my_module.add_src_file([
	    'src/module-info.java',
	    'src/org/atriasoft/gale/TextureFilter.java',
	    'src/org/atriasoft/gale/internal/LOGGER.java',
	    'src/org/atriasoft/gale/DisplayManagerDraw.java',
	    'src/org/atriasoft/gale/test/sample1/Sample1.java',
	    'src/org/atriasoft/gale/test/sample1/Sample1Application.java',
	    'src/org/atriasoft/gale/test/sample1/LOGGER.java',
	    'src/org/atriasoft/gale/test/sample2/Sample2Application.java',
	    'src/org/atriasoft/gale/test/sample2/Main.java',
	    'src/org/atriasoft/gale/test/sample2/LOGGER.java',
	    'src/org/atriasoft/gale/context/ClipboardList.java',
	    'src/org/atriasoft/gale/context/Cursor.java',
	    'src/org/atriasoft/gale/context/LWJG_AWT/ContextLWJGLAWT.java',
	    'src/org/atriasoft/gale/context/GaleContext.java',
	    'src/org/atriasoft/gale/context/ClipBoard.java',
	    'src/org/atriasoft/gale/context/CommandLine.java',
	    'src/org/atriasoft/gale/context/GaleContextTest.java',
	    'src/org/atriasoft/gale/backend3d/OpenGL.java',
	    'src/org/atriasoft/gale/Orientation.java',
	    'src/org/atriasoft/gale/resource/ResourceTexture2.java',
	    'src/org/atriasoft/gale/resource/ResourceShader.java',
	    'src/org/atriasoft/gale/resource/ResourceTexture.java',
	    'src/org/atriasoft/gale/resource/ResourceManager.java',
	    'src/org/atriasoft/gale/resource/ResourceVirtualArrayObject.java',
	    'src/org/atriasoft/gale/resource/Resource.java',
	    'src/org/atriasoft/gale/resource/ResourceVirtualBufferObject.java',
	    'src/org/atriasoft/gale/resource/ResourceColored3DObject.java',
	    'src/org/atriasoft/gale/resource/ResourceTextureFile.java',
	    'src/org/atriasoft/gale/resource/ResourceProgram.java',
	    'src/org/atriasoft/gale/key/KeySpecial.java',
	    'src/org/atriasoft/gale/key/KeyStatus.java',
	    'src/org/atriasoft/gale/key/KeyType.java',
	    'src/org/atriasoft/gale/key/KeyKeyboard.java',
	    'src/org/atriasoft/gale/GaleApplication.java',
	    'src/org/atriasoft/gale/Fps.java',
	    'src/org/atriasoft/gale/Gale.java',
	    'src/org/atriasoft/gale/tools/ImageRawData.java',
	    'src/org/atriasoft/gale/tools/ImageLoader.java',
	    ])
	my_module.add_path('src/', type='java')
	
	my_module.add_depend([
	    'org-atriasoft-iogami',
	    'org-atriasoft-pngdecoder',
	    ])
	
	my_module.add_path([
	    'lib/lwjgl/lwjgl.jar',
	    'lib/lwjgl3-awt-0.1.7.jar',
	    'lib/lwjgl/lwjgl-assimp.jar',
	    'lib/lwjgl/lwjgl-assimp-natives-linux.jar',
	    'lib/lwjgl/lwjgl-jawt.jar',
	    'lib/lwjgl/lwjgl-glfw-natives-linux.jar',
	    'lib/lwjgl/lwjgl-glfw.jar',
	    'lib/lwjgl/lwjgl-natives-linux.jar',
	    'lib/lwjgl/lwjgl-opengl.jar',
	    'lib/lwjgl/lwjgl-opengl-natives-linux.jar',
	    'lib/lwjgl/lwjgl-stb.jar',
	    'lib/lwjgl/lwjgl-stb-natives-linux.jar',
	    ],
	    type='java',
	    export=True
	);
	my_module.add_flag('java', "RELEASE_15_PREVIEW");
	
	return True

