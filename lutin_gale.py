#!/usr/bin/python
import realog.debug as debug
import lutin.tools as tools
import realog.debug as debug
import lutin.image as image
import os
import lutin.multiprocess as lutinMultiprocess


def get_type():
	return "LIBRARY"

def get_desc():
	return "gale is a main library to use widget in the openGl environement and manage all the wraping os"

def get_licence():
	return "MPL-2"

def get_compagny_type():
	return "com"

def get_compagny_name():
	return "atria-soft"

def get_maintainer():
	return "authors.txt"

def get_version():
	return "version.txt"

def configure(target, my_module):
	my_module.add_extra_flags()
	# add the file to compile:
	my_module.add_src_file([
	    'gale/gale.cpp',
	    'gale/debug.cpp',
	    'gale/Dimension.cpp',
	    'gale/orientation.cpp',
	    'gale/Application.cpp',
	    'gale/Thread.cpp',
	    ])
	my_module.add_header_file([
	    'gale/debug.hpp',
	    'gale/gale.hpp',
	    'gale/Dimension.hpp',
	    'gale/orientation.hpp',
	    'gale/Application.hpp',
	    'gale/Thread.hpp',
	    ])
	# context:
	my_module.add_src_file([
	    'gale/context/clipBoard.cpp',
	    'gale/context/commandLine.cpp',
	    'gale/context/Context.cpp',
	    'gale/context/cursor.cpp',
	    ])
	my_module.add_header_file([
	    'gale/context/clipBoard.hpp',
	    'gale/context/commandLine.hpp',
	    'gale/context/Context.hpp',
	    'gale/context/cursor.hpp',
	    'gale/context/Fps.hpp'
	    ])
	if "Web" in target.get_type():
		my_module.add_src_file('gale/context/SDL/Context.cpp')
	elif "Linux" in target.get_type():
		
		my_module.add_optionnal_depend(
		    'X11',
		    ["c++", "-DGALE_BUILD_X11"],
		    src_file=[
		        'gale/context/X11/Context.cpp',
		        ]
		    )
		my_module.add_optionnal_depend(
		    'wayland',
		    ["c++", "-DGALE_BUILD_WAYLAND"],
		    src_file=[
		        'gale/context/wayland/Context.cpp',
		        ]
		    )
		# check if egami is present in the worktree: this is for the icon parsing ...
		my_module.add_optionnal_depend('egami', ["c++", "-DGALE_BUILD_EGAMI"])
	elif "Windows" in target.get_type():
		my_module.add_src_file('gale/context/Windows/Context.cpp')
		my_module.add_depend('start-mode-gui')
	elif "Android" in target.get_type():
		my_module.add_src_file('gale/context/Android/Context.cpp')
		my_module.add_src_file([
		    'android/src/org/gale/GaleCallback.java',
		    'android/src/org/gale/GaleConstants.java',
		    'android/src/org/gale/Gale.java',
		    'android/src/org/gale/GaleRendererGL.java',
		    'android/src/org/gale/GaleSurfaceViewGL.java',
		    'android/src/org/gale/GaleActivity.java',
		    'android/src/org/gale/GaleWallpaper.java',
		    'org.gale.GaleConstants.javah'
		    ])
		my_module.add_path('android/src/', type='java')
	elif "MacOs" in target.get_type():
		my_module.add_src_file([
		    'gale/context/MacOs/Context.mm',
		    'gale/context/MacOs/Interface.mm',
		    'gale/context/MacOs/Windows.mm',
		    'gale/context/MacOs/OpenglView.mm',
		    'gale/context/MacOs/AppDelegate.mm'
		    ])
	elif "IOs" in target.get_type():
		my_module.add_src_file([
		    'gale/context/IOs/Context.cpp',
		    'gale/context/IOs/Interface.m',
		    'gale/context/IOs/OpenglView.mm',
		    'gale/context/IOs/AppDelegate.mm'
		    ])
	else:
		debug.error("unknow mode...")
	
	if    "Linux" in target.get_type() \
	   or "Windows" in target.get_type() \
	   or "MacOs" in target.get_type():
		# only in debug we add simulation:
		if target.get_mode() == "debug":
			my_module.add_flag('c++', "-DGALE_BUILD_SIMULATION")
			my_module.add_src_file('gale/context/simulation/Context.cpp')
	
	# Key properties:
	my_module.add_src_file([
	    'gale/key/keyboard.cpp',
	    'gale/key/Special.cpp',
	    'gale/key/status.cpp',
	    'gale/key/type.cpp'
	    ])
	my_module.add_header_file([
	    'gale/key/keyboard.hpp',
	    'gale/key/Special.hpp',
	    'gale/key/status.hpp',
	    'gale/key/type.hpp',
	    'gale/key/key.hpp'
	    ])
	# OpenGL interface:
	my_module.add_src_file([
	    'gale/renderer/openGL/openGL.cpp'
	    ])
	my_module.add_header_file([
	    'gale/renderer/openGL/openGL.hpp',
	    'gale/renderer/openGL/openGL-include.hpp'
	    ])
	# resources:
	my_module.add_src_file([
	    'gale/resource/Manager.cpp',
	    'gale/resource/Program.cpp',
	    'gale/resource/Resource.cpp',
	    'gale/resource/Shader.cpp',
	    'gale/resource/Texture.cpp',
	    'gale/resource/VirtualBufferObject.cpp'
	    ])
	my_module.add_header_file([
	    'gale/resource/Manager.hpp',
	    'gale/resource/Program.hpp',
	    'gale/resource/Resource.hpp',
	    'gale/resource/Shader.hpp',
	    'gale/resource/Texture.hpp',
	    'gale/resource/VirtualBufferObject.hpp'
	    ])
	my_module.add_depend([
	    'etk',
	    'opengl',
	    'echrono',
	    ])
	my_module.add_optionnal_depend('esignal', ["c++", "-DGALE_BUILD_ESIGNAL"])
	my_module.add_optionnal_depend('eproperty', ["c++", "-DGALE_BUILD_EPROPERTY"])
	my_module.add_path(".")
	
	my_module.add_flag('c++', [
	    "-DGALE_VERSION=\"\\\"" + tools.version_to_string(my_module.get_pkg("VERSION")) + "\\\"\""
	    ])
	
	if "Web" in target.get_type():
		my_module.add_depend([
		    "SDL"
		    ])
	elif "Linux" in target.get_type():
		# TODO : This is specific at wayland ==> check How we can add it better
		my_module.add_depend([
		    'egl',
		    'gles2',
		    'xkbcommon'
		    ])
		my_module.add_action(tool_generate_wayland_protocol, data={})
	elif "Android" in target.get_type():
		my_module.add_depend(["SDK", "jvm-basics"])
		# add tre creator of the basic java class ...
		target.add_action("BINARY", 50, "gale-auto-wrapper", tool_generate_main_java_class)
		# TODO : Add the same for BINARY to create a console interface ?
	elif "Windows" in target.get_type():
		my_module.add_depend("glew")
	elif "MacOs" in target.get_type():
		my_module.add_depend([
		    "Cocoa",
		    "QuartzCore",
		    "AppKit",
		    ])
	elif "IOs" in target.get_type():
		my_module.add_depend([
		    "CoreGraphics",
		    "UIKit",
		    "GLKit",
		    "Foundation",
		    "QuartzCore",
		    ])
	
	return True



##################################################################
##
## Wayland specific section
##
##################################################################
def tool_generate_wayland_protocol(target, module, package_name):
	file_list = []
	debug.warning("------------------------------------------------------------------------")
	debug.warning("Generate wayland back elements... '" + str(module) + "'" )
	debug.warning("------------------------------------------------------------------------")
	cmd = ["pkg-config", "wayland-protocols", "--variable=pkgdatadir"]
	ret = lutinMultiprocess.run_command_direct(tools.list_to_str(cmd))
	if ret == False:
		debug.error("Can not execute protocol extraction...")
	WAYLAND_PROTOCOLS_DIR = ret
	debug.warning("WAYLAND_PROTOCOLS_DIR = " + str(WAYLAND_PROTOCOLS_DIR))
	cmd = ["pkg-config", "--variable=wayland_scanner", "wayland-scanner"]
	ret = lutinMultiprocess.run_command_direct(tools.list_to_str(cmd))
	if ret == False:
		debug.error("Can not execute protocol extraction...")
	WAYLAND_SCANNER = ret
	debug.warning("WAYLAND_SCANNER = " + str(WAYLAND_SCANNER))
	XDG_SHELL_PROTOCOL = os.path.join(WAYLAND_PROTOCOLS_DIR, "stable", "xdg-shell", "xdg-shell.xml")
	debug.warning("XDG_SHELL_PROTOCOL = " + str(XDG_SHELL_PROTOCOL))
	client_protocol_header = "xdg-shell-client-protocol.h"
	client_protocol = "xdg-shell-protocol.c"
	# create files
	
	debug.warning("Generate file = " + client_protocol_header)
	tmp_file = "/tmp/gale_wayland.tmp"
	cmd = [WAYLAND_SCANNER, "client-header", XDG_SHELL_PROTOCOL, tmp_file]
	ret = lutinMultiprocess.run_command_direct(tools.list_to_str(cmd))
	if ret == False:
		debug.error("error in generate wayland header code")
	tmp_file_data = tools.file_read_data(tmp_file)
	module.add_generated_header_file(tmp_file_data, client_protocol_header)
	
	
	debug.warning("Generate file = " + client_protocol)
	cmd = [WAYLAND_SCANNER, "private-code", XDG_SHELL_PROTOCOL, tmp_file]
	ret = lutinMultiprocess.run_command_direct(tools.list_to_str(cmd))
	if ret == False:
		debug.error("Error in wayland generation code of private header protocole")
	
	tmp_file_data = tools.file_read_data(tmp_file)
	module.add_generated_src_file(tmp_file_data, client_protocol)


##################################################################
##
## Android specific section
##
##################################################################
def tool_generate_main_java_class(target, module, package_name):
	file_list = []
	
	debug.debug("------------------------------------------------------------------------")
	debug.debug("Generate android wrapping for '" + package_name + "' ==> '" + target.convert_name_application(package_name) + "'" )
	debug.debug("------------------------------------------------------------------------")
	application_name = target.convert_name_application(package_name)
	if target.get_mode() == "debug":
		application_name += "debug"
	target.path_java_project=   target.get_build_path(package_name) \
	                            + "/src/" \
	                            + module.get_pkg("COMPAGNY_TYPE") \
	                            + "/" + module.get_pkg("COMPAGNY_NAME2") \
	                            + "/" + application_name + "/"
	
	java_file_wrapper = target.path_java_project + "/" + application_name + ".java"
	
	android_package_name = module.get_pkg("COMPAGNY_TYPE")+"."+module.get_pkg("COMPAGNY_NAME2")+"." + application_name
	
	debug.print_element("pkg", "absractionFile", "<==", "dynamic file")
	# Create path :
	tools.create_directory_of_file(java_file_wrapper)
	debug.debug("create file : '" + java_file_wrapper + "'")
	# Create file :
	tmpFile = open(java_file_wrapper + "_tmp", 'w')
	
	tmpFile.write( "/**\n")
	tmpFile.write( " * @author Edouard DUPIN, Kevin BILLONNEAU\n")
	tmpFile.write( " * @copyright 2011, Edouard DUPIN, all right reserved\n")
	tmpFile.write( " * @license MPL-2 (see license file)\n")
	tmpFile.write( " * @note This file is autogenerate ==> see documantation to generate your own\n")
	tmpFile.write( " */\n")
	tmpFile.write( "package "+ android_package_name + ";\n")
	tmpFile.write( "import android.util.Log;\n")
	if module.get_pkg("ANDROID_APPL_TYPE")=="APPL":
		tmpFile.write( "import org.gale.GaleActivity;\n")
	else:
		tmpFile.write( "import org.gale.GaleWallpaper;\n")
	tmpFile.write( "\n")
	
	if module.get_pkg("GENERATE_SECTION__IMPORT") != None:
		for elem in module.get_pkg("GENERATE_SECTION__IMPORT"):
			for line in elem:
				tmpFile.write( line + "\n")
	if module.get_pkg("ANDROID_APPL_TYPE")=="APPL":
		tmpFile.write( "public class " + application_name + " extends GaleActivity {\n")
	else:
		tmpFile.write( "public class " + application_name + " extends GaleWallpaper {\n")
		tmpFile.write( "	public static final String SHARED_PREFS_NAME = \"" + application_name + "settings\";\n")
	
	if module.get_pkg("GENERATE_SECTION__DECLARE") != None:
		for elem in module.get_pkg("GENERATE_SECTION__DECLARE"):
			for line in elem:
				tmpFile.write( "	" + line + "\n")
	
	tmpFile.write( "	\n")
	tmpFile.write( "	static {\n")
	tmpFile.write( "		try {\n")
	tmpFile.write( "			System.loadLibrary(\"" + package_name + "\");\n")
	tmpFile.write( "		} catch (UnsatisfiedLinkError e) {\n")
	tmpFile.write( "			Log.e(\"" + application_name + "\", \"error getting lib(): \" + e);\n")
	tmpFile.write( "		}\n")
	tmpFile.write( "	}\n")
	tmpFile.write( "	\n")
	if module.get_pkg("ANDROID_APPL_TYPE")!="APPL":
		tmpFile.write( "	public Engine onCreateEngine() {\n")
		tmpFile.write( "		initApkPath(\"" + module.get_pkg("COMPAGNY_TYPE")+"\", \""+module.get_pkg("COMPAGNY_NAME2")+"\", \"" + application_name + "\", \"" + package_name + "\");\n")
		tmpFile.write( "		Engine tmpEngine = super.onCreateEngine();\n")
		tmpFile.write( "		return tmpEngine;\n")
		tmpFile.write( "	}\n")
	
	if module.get_pkg("GENERATE_SECTION__CONSTRUCTOR") != None:
		tmpFile.write( "	public " + application_name + "() {\n")
		for elem in module.get_pkg("GENERATE_SECTION__CONSTRUCTOR"):
			for line in elem:
				tmpFile.write( "		" + line + "\n")
		tmpFile.write( "	}\n")
	
	if module.get_pkg("ANDROID_APPL_TYPE")!="APPL":
		tmpFile.write( "	public void onCreate() {\n")
		tmpFile.write( "		initApkPath(\"" + module.get_pkg("COMPAGNY_TYPE")+"\", \""+module.get_pkg("COMPAGNY_NAME2")+"\", \"" + application_name + "\", \"" + package_name + "\");\n")
		tmpFile.write( "		super.onCreate();\n")
	else:
		tmpFile.write( "	public void onCreate(android.os.Bundle savedInstanceState) {\n")
		tmpFile.write( "		initApkPath(\"" + module.get_pkg("COMPAGNY_TYPE")+"\", \""+module.get_pkg("COMPAGNY_NAME2")+"\", \"" + application_name + "\", \"" + package_name + "\");\n")
		tmpFile.write( "		super.onCreate(savedInstanceState);\n")
	
	if module.get_pkg("GENERATE_SECTION__ON_CREATE") != None:
		for elem in module.get_pkg("GENERATE_SECTION__ON_CREATE"):
			for line in elem:
				tmpFile.write( "		" + line + "\n")
	tmpFile.write( "	}\n")
	
	
	if module.get_pkg("GENERATE_SECTION__ON_START") != None:
		tmpFile.write( "	@Override protected void onStart() {\n")
		for elem in module.get_pkg("GENERATE_SECTION__ON_START"):
			for line in elem:
				tmpFile.write( "		" + line + "\n")
		tmpFile.write( "		super.onStart();\n")
		tmpFile.write( "	}\n")
	
	if module.get_pkg("GENERATE_SECTION__ON_RESTART") != None:
		tmpFile.write( "	@Override protected void onRestart() {\n")
		for elem in module.get_pkg("GENERATE_SECTION__ON_RESTART"):
			for line in elem:
				tmpFile.write( "		" + line + "\n")
		tmpFile.write( "		super.onRestart();\n")
		tmpFile.write( "	}\n")
	
	if module.get_pkg("GENERATE_SECTION__ON_RESUME") != None:
		tmpFile.write( "	@Override protected void onResume() {\n")
		tmpFile.write( "		super.onResume();\n")
		for elem in module.get_pkg("GENERATE_SECTION__ON_RESUME"):
			for line in elem:
				tmpFile.write( "		" + line + "\n")
		tmpFile.write( "	}\n")
	
	if module.get_pkg("GENERATE_SECTION__ON_PAUSE") != None:
		tmpFile.write( "	@Override protected void onPause() {\n")
		for elem in module.get_pkg("GENERATE_SECTION__ON_PAUSE"):
			for line in elem:
				tmpFile.write( "		" + line + "\n")
		tmpFile.write( "		super.onPause();\n")
		tmpFile.write( "	}\n")
	
	if module.get_pkg("GENERATE_SECTION__ON_STOP") != None:
		tmpFile.write( "	@Override protected void onStop() {\n")
		for elem in module.get_pkg("GENERATE_SECTION__ON_STOP"):
			for line in elem:
				tmpFile.write( "		" + line + "\n")
		tmpFile.write( "		super.onStop();\n")
		tmpFile.write( "	}\n")
	
	if module.get_pkg("GENERATE_SECTION__ON_DESTROY") != None:
		tmpFile.write( "	@Override protected void onDestroy() {\n")
		for elem in module.get_pkg("GENERATE_SECTION__ON_DESTROY"):
			for line in elem:
				tmpFile.write( "		" + line + "\n")
		tmpFile.write( "		super.onDestroy();\n")
		tmpFile.write( "	}\n")
	tmpFile.write( "}\n")
	tmpFile.flush()
	tmpFile.close()
	
	tools.move_if_needed(java_file_wrapper + "_tmp", java_file_wrapper);
	# add java file to build:
	module.add_src_file([java_file_wrapper])
	
	
	## todo:
	"""
	debug.warning("icon : " + module.get_pkg("ICON"))
	tools.create_directory_of_file(target.get_staging_path(package_name) + "/res/drawable/icon.png");
	if     module.get_pkg("ICON") != None \
	   and module.get_pkg("ICON") != "":
		image.resize(module.get_pkg("ICON"), target.get_staging_path(package_name) + "/res/drawable/icon.png", 256, 256)
		# if must be copied befor as user data ... todo : check
		pass
	else:
		debug.warning("copy a generic Atria-soft icon ...")
		# to be sure that we have all time a resource ...
		image.resize(os.path.join(tools.get_current_path(__file__),"atria-soft.png"),
		             os.path.join(target.get_staging_path(package_name), "res", "drawable", "icon.png"),
		             256, 256)
	"""
	
	if module.get_pkg("ANDROID_MANIFEST") == "":
		# force manifest file:
		module.set_pkg("ANDROID_MANIFEST", os.path.join(target.get_build_path(package_name), "AndroidManifest.xml"));
		debug.debug(" create file: '" + module.get_pkg("ANDROID_MANIFEST") + "'")
		if module.get_pkg("VERSION_CODE") == None:
			module.set_pkg("VERSION_CODE", "1")
		debug.print_element("pkg", "AndroidManifest.xml", "<==", "package configurations")
		tools.create_directory_of_file(module.get_pkg("ANDROID_MANIFEST"))
		tmpFile = open(module.get_pkg("ANDROID_MANIFEST"), 'w')
		tmpFile.write( '<?xml version="1.0" encoding="utf-8"?>\n')
		tmpFile.write( '<!-- Manifest is autoGenerated with Gale ... do not patch it-->\n')
		tmpFile.write( '<manifest xmlns:android="http://schemas.android.com/apk/res/android" \n')
		tmpFile.write( '          package="' + android_package_name + '" \n')
		if module.get_pkg("VERSION_CODE") == '':
			debug.warning("Missing application 'VERSION_CODE' ==> set it at '0' (this can creata an NON update on android play store)")
			module.set_pkg("VERSION_CODE", "0")
		tmpFile.write( '          android:versionCode="' + str(module.get_pkg("VERSION_CODE")) + '" \n')
		tmpFile.write( '          android:versionName="'+tools.version_to_string(module.get_pkg("VERSION"))+'"> \n')
		tmpFile.write( '	<uses-feature android:glEsVersion="0x00030000" android:required="true" />\n')
		tmpFile.write( '	<uses-sdk android:minSdkVersion="' + str(target.board_id) + '" \n')
		tmpFile.write( '	          android:targetSdkVersion="' + str(target.board_id) + '" /> \n')
		if module.get_pkg("ANDROID_APPL_TYPE")=="APPL":
			tmpFile.write( '	<application android:label="' + application_name + '" \n')
			if module.get_pkg("ICON") != None:
				tmpFile.write( '	             android:icon="@drawable/icon" \n')
			if target.get_mode() == "debug":
				tmpFile.write( '	             android:debuggable="true" \n')
			tmpFile.write( '	             >\n')
			if module.get_pkg("ADMOD_ID") != None:
				tmpFile.write( '		<meta-data android:name="com.google.android.gms.version" \n')
				tmpFile.write( '		           android:value="@integer/google_play_services_version"/>\n')
			
			tmpFile.write( '		<activity android:name=".' + application_name + '" \n')
			tmpFile.write( '		          android:label="' + module.get_pkg('NAME'))
			if target.get_mode() == "debug":
				tmpFile.write("-debug")
			tmpFile.write( '"\n')
			if module.get_pkg("ICON") != None:
				tmpFile.write( '		          android:icon="@drawable/icon" \n')
			tmpFile.write( '		          android:hardwareAccelerated="true" \n')
			tmpFile.write( '		          android:configChanges="keyboard|keyboardHidden|orientation|screenSize"> \n')
			tmpFile.write( '			<intent-filter> \n')
			tmpFile.write( '				<action android:name="android.intent.action.MAIN" /> \n')
			tmpFile.write( '				<category android:name="android.intent.category.LAUNCHER" /> \n')
			tmpFile.write( '			</intent-filter> \n')
			tmpFile.write( '		</activity> \n')
			if module.get_pkg("ADMOD_ID") != None:
				tmpFile.write( '		<activity android:name="com.google.android.gms.ads.AdActivity"\n')
				tmpFile.write( '		          android:configChanges="keyboard|keyboardHidden|orientation|screenLayout|uiMode|screenSize|smallestScreenSize"/>\n')
			
			tmpFile.write( '	</application>\n')
		else:
			tmpFile.write( '	<application android:label="' + application_name + '" \n')
			tmpFile.write( '	             android:permission="android.permission.BIND_WALLPAPER" \n')
			if module.get_pkg("ICON") != None:
				tmpFile.write( '	             android:icon="@drawable/icon"\n')
			tmpFile.write( '	             >\n')
			tmpFile.write( '		<service android:name=".' + application_name + '" \n')
			tmpFile.write( '		         android:label="' + module.get_pkg('NAME'))
			if target.get_mode() == "debug":
				tmpFile.write("-debug")
			tmpFile.write( '"\n')
			if module.get_pkg("ICON") != None:
				tmpFile.write( '		         android:icon="@drawable/icon"\n')
			tmpFile.write( '		         >\n')
			tmpFile.write( '			<intent-filter>\n')
			tmpFile.write( '				<action android:name="android.service.wallpaper.WallpaperService" />\n')
			tmpFile.write( '			</intent-filter>\n')
			tmpFile.write( '			<meta-data android:name="android.service.wallpaper"\n')
			tmpFile.write( '			           android:resource="@xml/' + application_name + '_resource" />\n')
			tmpFile.write( '		</service>\n')
			if len(module.get_pkg("ANDROID_WALLPAPER_PROPERTIES"))!=0:
				tmpFile.write( '		<activity android:label="Setting"\n')
				tmpFile.write( '		          android:name=".' + application_name + 'Settings"\n')
				tmpFile.write( '		          android:theme="@android:style/Theme.Light.WallpaperSettings"\n')
				tmpFile.write( '		          android:exported="true"\n')
				if module.get_pkg("ICON") != None:
					tmpFile.write( '		          android:icon="@drawable/icon"\n')
				tmpFile.write( '		          >\n')
				tmpFile.write( '		</activity>\n')
			tmpFile.write( '	</application>\n')
		# write package autorisations :
		if "WRITE_EXTERNAL_STORAGE" in module.get_pkg("RIGHT"):
			tmpFile.write( '	<permission android:name="android.permission.WRITE_EXTERNAL_STORAGE" /> \n')
			tmpFile.write( '	<uses-permission android:name="android.permission.WRITE_EXTERNAL_STORAGE" /> \n')
		if "CAMERA" in module.get_pkg("RIGHT"):
			tmpFile.write( '	<permission android:name="android.permission.CAMERA" /> \n')
			tmpFile.write( '	<uses-permission android:name="android.permission.CAMERA" /> \n')
		if "INTERNET" in module.get_pkg("RIGHT"):
			tmpFile.write( '	<permission android:name="android.permission.INTERNET" /> \n')
			tmpFile.write( '	<uses-permission android:name="android.permission.INTERNET" /> \n')
		if "ACCESS_NETWORK_STATE" in module.get_pkg("RIGHT"):
			tmpFile.write( '	<permission android:name="android.permission.ACCESS_NETWORK_STATE" /> \n')
			tmpFile.write( '	<uses-permission android:name="android.permission.ACCESS_NETWORK_STATE" /> \n')
		if "MODIFY_AUDIO_SETTINGS" in module.get_pkg("RIGHT"):
			tmpFile.write( '	<permission android:name="android.permission.MODIFY_AUDIO_SETTINGS" /> \n')
			tmpFile.write( '	<uses-permission android:name="android.permission.MODIFY_AUDIO_SETTINGS" /> \n')
		if "READ_CALENDAR" in module.get_pkg("RIGHT"):
			tmpFile.write( '	<permission android:name="android.permission.READ_CALENDAR" /> \n')
			tmpFile.write( '	<uses-permission android:name="android.permission.READ_CALENDAR" /> \n')
		if "READ_CONTACTS" in module.get_pkg("RIGHT"):
			tmpFile.write( '	<permission android:name="android.permission.READ_CONTACTS" /> \n')
			tmpFile.write( '	<uses-permission android:name="android.permission.READ_CONTACTS" /> \n')
		if "READ_FRAME_BUFFER" in module.get_pkg("RIGHT"):
			tmpFile.write( '	<permission android:name="android.permission.READ_FRAME_BUFFER" /> \n')
			tmpFile.write( '	<uses-permission android:name="android.permission.READ_FRAME_BUFFER" /> \n')
		if "READ_PROFILE" in module.get_pkg("RIGHT"):
			tmpFile.write( '	<permission android:name="android.permission.READ_PROFILE" /> \n')
			tmpFile.write( '	<uses-permission android:name="android.permission.READ_PROFILE" /> \n')
		if "RECORD_AUDIO" in module.get_pkg("RIGHT"):
			tmpFile.write( '	<permission android:name="android.permission.RECORD_AUDIO" /> \n')
			tmpFile.write( '	<uses-permission android:name="android.permission.RECORD_AUDIO" /> \n')
		if "SET_ORIENTATION" in module.get_pkg("RIGHT"):
			tmpFile.write( '	<permission android:name="android.permission.SET_ORIENTATION" /> \n')
			tmpFile.write( '	<uses-permission android:name="android.permission.SET_ORIENTATION" /> \n')
		if "VIBRATE" in module.get_pkg("RIGHT"):
			tmpFile.write( '	<permission android:name="android.permission.VIBRATE" /> \n')
			tmpFile.write( '	<uses-permission android:name="android.permission.VIBRATE" /> \n')
		if "ACCESS_COARSE_LOCATION" in module.get_pkg("RIGHT"):
			tmpFile.write( '	<permission android:name="android.permission.ACCESS_COARSE_LOCATION" /> \n')
			tmpFile.write( '	<uses-permission android:name="android.permission.ACCESS_COARSE_LOCATION" /> \n')
		if "ACCESS_FINE_LOCATION" in module.get_pkg("RIGHT"):
			tmpFile.write( '	<permission android:name="android.permission.ACCESS_FINE_LOCATION" /> \n')
			tmpFile.write( '	<uses-permission android:name="android.permission.ACCESS_FINE_LOCATION" /> \n')
		tmpFile.write( '</manifest>\n\n')
		tmpFile.flush()
		tmpFile.close()
		# end generating android manifest
		
		if module.get_pkg("ANDROID_APPL_TYPE") != "APPL":
			#create the Wallpaper sub files : (main element for the application
			debug.print_element("pkg", application_name + "_resource.xml", "<==", "package configurations")
			resource_file_name = os.path.join(target.get_staging_path(package_name), "res", "xml", application_name + "_resource.xml")
			tools.create_directory_of_file(resource_file_name)
			tmpFile = open(resource_file_name, 'w')
			tmpFile.write( "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n")
			tmpFile.write( "<wallpaper xmlns:android=\"http://schemas.android.com/apk/res/android\"\n")
			if len(module.get_pkg("ANDROID_WALLPAPER_PROPERTIES"))!=0:
				tmpFile.write( "           android:settingsActivity=\"" + android_package_name + "." + application_name + "Settings\"\n")
			if module.get_pkg("ICON") != None:
				tmpFile.write( "           android:thumbnail=\"@drawable/icon\"\n")
			tmpFile.write( "           />\n")
			tmpFile.flush()
			tmpFile.close()
			# create wallpaper setting if needed (class and config file)
			if len(module.get_pkg("ANDROID_WALLPAPER_PROPERTIES"))!=0:
				tools.create_directory_of_file(target.path_java_project + application_name + "Settings.java")
				debug.print_element("pkg", target.path_java_project + application_name + "Settings.java", "<==", "package configurations")
				tmpFile = open(target.path_java_project + application_name + "Settings.java", 'w');
				tmpFile.write( "package " + android_package_name + ";\n")
				tmpFile.write( "\n")
				tmpFile.write( "import " + android_package_name + ".R;\n")
				tmpFile.write( "\n")
				tmpFile.write( "import android.content.SharedPreferences;\n")
				tmpFile.write( "import android.os.Bundle;\n")
				tmpFile.write( "import android.preference.PreferenceActivity;\n")
				tmpFile.write( "\n")
				tmpFile.write( "public class " + application_name + "Settings extends PreferenceActivity implements SharedPreferences.OnSharedPreferenceChangeListener\n")
				tmpFile.write( "{\n")
				tmpFile.write( "	@Override protected void onCreate(Bundle icicle) {\n")
				tmpFile.write( "		super.onCreate(icicle);\n")
				tmpFile.write( "		getPreferenceManager().setSharedPreferencesName("+ application_name + ".SHARED_PREFS_NAME);\n")
				tmpFile.write( "		addPreferencesFromResource(R.xml."+ application_name  + "_settings);\n")
				tmpFile.write( "		getPreferenceManager().getSharedPreferences().registerOnSharedPreferenceChangeListener(this);\n")
				tmpFile.write( "	}\n")
				tmpFile.write( "	@Override protected void onResume() {\n")
				tmpFile.write( "		super.onResume();\n")
				tmpFile.write( "	}\n")
				tmpFile.write( "	@Override protected void onDestroy() {\n")
				tmpFile.write( "		getPreferenceManager().getSharedPreferences().unregisterOnSharedPreferenceChangeListener(this);\n")
				tmpFile.write( "		super.onDestroy();\n")
				tmpFile.write( "	}\n")
				tmpFile.write( "	public void onSharedPreferenceChanged(SharedPreferences sharedPreferences,String key) { }\n")
				tmpFile.write( "}\n")
				tmpFile.flush()
				tmpFile.close()
				
				debug.print_element("pkg", target.get_build_path(package_name) + "/res/xml/" + application_name + "_settings.xml", "<==", "package configurations")
				tools.create_directory_of_file(target.get_build_path(package_name) + "/res/xml/" + application_name + "_settings.xml")
				tmpFile = open(target.get_build_path(package_name) + "/res/xml/" + application_name + "_settings.xml", 'w');
				tmpFile.write( "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n")
				tmpFile.write( "<PreferenceScreen xmlns:android=\"http://schemas.android.com/apk/res/android\"\n")
				tmpFile.write( "                  android:title=\"Settings\"\n")
				tmpFile.write( "                  android:key=\"" + application_name  + "_settings\">\n")
				WALL_haveArray = False
				for WALL_type, WALL_key, WALL_title, WALL_summary, WALL_other in module.get_pkg("ANDROID_WALLPAPER_PROPERTIES"):
					debug.info("find : '" + WALL_type + "'");
					if WALL_type == "list":
						debug.info("    create : LIST");
						tmpFile.write( "	<ListPreference android:key=\"" + application_name + "_" + WALL_key + "\"\n")
						tmpFile.write( "	                android:title=\"" + WALL_title + "\"\n")
						tmpFile.write( "	                android:summary=\"" + WALL_summary + "\"\n")
						tmpFile.write( "	                android:entries=\"@array/" + application_name + "_" + WALL_key + "_names\"\n")
						tmpFile.write( "	                android:entryValues=\"@array/" + application_name + "_" + WALL_key + "_prefix\"/>\n")
						WALL_haveArray=True
					elif WALL_type == "bool":
						debug.info("    create : CHECKBOX");
						tmpFile.write( "	<CheckBoxPreference android:key=\"" + application_name + "_" + WALL_key + "\"\n")
						tmpFile.write( "	                    android:title=\"" + WALL_title + "\"\n")
						tmpFile.write( "	                    android:summary=\"" + WALL_summary + "\"\n")
						tmpFile.write( "	                    android:summaryOn=\"" + WALL_other[0] + "\"\n")
						tmpFile.write( "	                    android:summaryOff=\"" + WALL_other[1] + "\"/>\n")
				tmpFile.write( "</PreferenceScreen>\n")
				tmpFile.flush()
				tmpFile.close()
				if WALL_haveArray==True:
					for WALL_type, WALL_key, WALL_title, WALL_summary, WALL_other in module.get_pkg("ANDROID_WALLPAPER_PROPERTIES"):
						if WALL_type == "list":
							debug.print_element("pkg", target.get_build_path(package_name) + "/res/values/" + WALL_key + ".xml", "<==", "package configurations")
							tools.create_directory_of_file(target.get_build_path(package_name) + "/res/values/" + WALL_key + ".xml")
							tmpFile = open(target.get_build_path(package_name) + "/res/values/" + WALL_key + ".xml", 'w');
							tmpFile.write( "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n")
							tmpFile.write( "<resources xmlns:xliff=\"urn:oasis:names:tc:xliff:document:1.2\">\n")
							tmpFile.write( "	<string-array name=\"" + application_name + "_" + WALL_key + "_names\">\n")
							for WALL_subKey, WALL_display in WALL_other:
								tmpFile.write( "		<item>" + WALL_display + "</item>\n")
							tmpFile.write( "	</string-array>\n")
							tmpFile.write( "	<string-array name=\"" + application_name + "_" + WALL_key + "_prefix\">\n")
							for WALL_subKey, WALL_display in WALL_other:
								tmpFile.write( "		<item>" + WALL_subKey + "</item>\n")
							tmpFile.write( "	</string-array>\n")
							tmpFile.write( "</resources>\n")
							tmpFile.flush()
							tmpFile.close()
		
	
	"""
	#add properties on wallpaper : 
	# my_module.add_pkg("ANDROID_WALLPAPER_PROPERTIES", ["list", key, title, summary, [["key","value display"],["key2","value display 2"]])
	# my_module.add_pkg("ANDROID_WALLPAPER_PROPERTIES", ["list", "testpattern", "Select test pattern", "Choose which test pattern to display", [["key","value display"],["key2","value display 2"]]])
	# my_module.add_pkg("ANDROID_WALLPAPER_PROPERTIES", ["bool", key, title, summary, ["enable string", "disable String"])
	# my_module.add_pkg("ANDROID_WALLPAPER_PROPERTIES", ["bool", "movement", "Motion", "Apply movement to test pattern", ["Moving test pattern", "Still test pattern"]
	#copy needed resources :
	for res_source, res_dest in module.get_pkg("ANDROID_RESOURCES"):
		if res_source == "":
			continue
		tools.copy_file(res_source , target.get_staging_path(package_name) + "/res/" + res_dest + "/" + os.path.basename(res_source), force=True)
	"""
	"""
	# Doc :
	# http://asantoso.wordpress.com/2009/09/15/how-to-build-android-application-package-apk-from-the-command-line-using-the-sdk-tools-continuously-integrated-using-cruisecontrol/
	debug.print_element("pkg", "R.java", "<==", "Resources files")
	tools.create_directory_of_file(target.get_staging_path(package_name) + "/src/noFile")
	androidToolPath = target.path_sdk + "/build-tools/"
	# find android tool version
	dirnames = tools.get_list_sub_path(androidToolPath)
	if len(dirnames) != 1:
		debug.error("an error occured when getting the tools for android")
	androidToolPath += dirnames[0] + "/"
	
	adModResoucepath = ""
	if module.get_pkg("ADMOD_ID") != None:
		adModResoucepath = " -S " + target.path_sdk + "/extras/google/google_play_services/libproject/google-play-services_lib/res/ "
	cmdLine = androidToolPath + "aapt p -f " \
	          + "-M " + target.get_staging_path(package_name) + "/AndroidManifest.xml " \
	          + "-F " + target.get_staging_path(package_name) + "/resources.res " \
	          + "-I " + target.path_sdk + "/platforms/android-" + str(target.board_id) + "/android.jar "\
	          + "-S " + target.get_staging_path(package_name) + "/res/ " \
	          + adModResoucepath \
	          + "-J " + target.get_staging_path(package_name) + "/src/ "
	multiprocess.run_command(cmdLine)
	#aapt  package -f -M ${manifest.file} -F ${packaged.resource.file} -I ${path.to.android-jar.library} 
	#      -S ${android-resource-directory} [-m -J ${path.to.output.the.R.java}]
	
	tools.create_directory_of_file(target.get_staging_path(package_name) + "/build/classes/noFile")
	debug.print_element("pkg", "*.class", "<==", "*.java")
	# more information with : -Xlint
	#          + java_file_wrapper + " "\ # this generate ex: out/Android/debug/staging/tethys/src/com/edouarddupin/tethys/edn.java
	
	#generate android java files:
	filesString=""
	for element in module.get_pkg("ANDROID_JAVA_FILES"):
		if element=="DEFAULT":
			filesString += target.path_gale + "/android/src/org/gale/GaleAudioTask.java "
			filesString += target.path_gale + "/android/src/org/gale/GaleCallback.java "
			filesString += target.path_gale + "/android/src/org/gale/GaleConstants.java "
			filesString += target.path_gale + "/android/src/org/gale/Gale.java "
			filesString += target.path_gale + "/android/src/org/gale/GaleRendererGL.java "
			filesString += target.path_gale + "/android/src/org/gale/GaleSurfaceViewGL.java "
			filesString += target.path_gale + "/android/src/org/gale/GaleActivity.java "
			filesString += target.path_gale + "/android/src/org/gale/GaleWallpaper.java "
		else:
			filesString += element + " "
	
	if in module.get_pkg("ADMOD_ID") != None:
		filesString += target.path_sdk + "/extras/google/google_play_services/libproject/google-play-services_lib/src/android/UnusedStub.java "
		
	if len(module.get_pkg("ANDROID_WALLPAPER_PROPERTIES"))!=0:
		filesString += target.path_java_project + application_name + "Settings.java "
	
	adModJarFile = ""
	if module.get_pkg("ADMOD_ID") != None:
		adModJarFile = ":" + target.path_sdk + "/extras/google/google_play_services/libproject/google-play-services_lib/libs/google-play-services.jar"
	
	cmdLine = "javac " \
	          + "-d " + target.get_staging_path(package_name) + "/build/classes " \
	          + "-classpath " + target.path_sdk + "/platforms/android-" + str(target.board_id) + "/android.jar" \
	          + adModJarFile + " " \
	          + filesString \
	          + java_file_wrapper + " "  \
	          + target.get_staging_path(package_name) + "/src/R.java "
	multiprocess.run_command(cmdLine)
	
	debug.print_element("pkg", ".dex", "<==", "*.class")
	cmdLine = androidToolPath + "dx " \
	          + "--dex --no-strict " \
	          + "--output=" + target.get_staging_path(package_name) + "/build/" + application_name + ".dex " \
	          + target.get_staging_path(package_name) + "/build/classes/ "
	
	if module.get_pkg("ADMOD_ID") != None:
		cmdLine += target.path_sdk + "/extras/google/google_play_services/libproject/google-play-services_lib/libs/google-play-services.jar "
	multiprocess.run_command(cmdLine)
	"""
	return {"files":file_list}



