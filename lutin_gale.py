#!/usr/bin/python
import lutin.module as module
import lutin.tools as tools
import lutin.debug as debug
import os
import lutin.multiprocess as lutinMultiprocess

def get_desc():
	return "gale is a main library to use widget in the openGl environement and manage all the wraping os"

def get_license():
	return "APACHE v2.0"

def create(target):
	# module name is 'edn' and type binary.
	myModule = module.Module(__file__, 'gale', 'LIBRARY')
	
	# add extra compilation flags :
	myModule.add_extra_compile_flags()
	# add the file to compile:
	myModule.add_src_file([
		'gale/gale.cpp',
		'gale/debug.cpp',
		'gale/Dimension.cpp',
		'gale/orientation.cpp',
		'gale/Application.cpp',
		])
	
	# context :
	myModule.add_src_file([
		'gale/context/clipBoard.cpp',
		'gale/context/commandLine.cpp',
		'gale/context/Context.cpp',
		'gale/context/cursor.cpp',
		#'gale/context/InputManager.cpp'
		])
	if target.name=="Linux":
		myModule.add_src_file('gale/context/X11/Context.cpp')
		# check if egami is present in the worktree: this is for the icon parsing ...
		myModule.add_optionnal_module_depend('egami', ["c++", "-DGALE_BUILD_EGAMI"])
	elif target.name=="Windows":
		myModule.add_src_file('gale/context/Windows/Context.cpp')
	elif target.name=="Android":
		myModule.add_src_file('gale/context/Android/Context.cpp')
		myModule.add_src_file([
			'android/src/org/gale/GaleCallback.java',
			'android/src/org/gale/GaleConstants.java',
			'android/src/org/gale/Gale.java',
			'android/src/org/gale/GaleRendererGL.java',
			'android/src/org/gale/GaleSurfaceViewGL.java',
			'android/src/org/gale/GaleActivity.java',
			'android/src/org/gale/GaleWallpaper.java',
			'org.gale.GaleConstants.javah'
			])
		myModule.add_path(tools.get_current_path(__file__) + '/android/src/', type='java')
	elif target.name=="MacOs":
		myModule.add_src_file([
			'gale/context/MacOs/Context.mm',
			'gale/context/MacOs/Interface.mm',
			'gale/context/MacOs/Windows.mm',
			'gale/context/MacOs/OpenglView.mm',
			'gale/context/MacOs/AppDelegate.mm'])
	elif target.name=="IOs":
		myModule.add_src_file([
			'gale/context/IOs/Context.cpp',
			'gale/context/IOs/Interface.m',
			'gale/context/IOs/OpenglView.mm',
			'gale/context/IOs/AppDelegate.mm'])
	else:
		debug.error("unknow mode...")
	
	# Key properties :
	myModule.add_src_file([
		'gale/key/keyboard.cpp',
		'gale/key/Special.cpp',
		'gale/key/status.cpp',
		'gale/key/type.cpp'
		])
	
	# OpenGL interface :
	myModule.add_src_file([
		'gale/renderer/openGL/openGL.cpp'
		])
	
	# resources :
	myModule.add_src_file([
		'gale/resource/Manager.cpp',
		'gale/resource/Program.cpp',
		'gale/resource/Resource.cpp',
		'gale/resource/Shader.cpp',
		'gale/resource/Texture.cpp',
		'gale/resource/VirtualBufferObject.cpp'
		])
	
	# name of the dependency
	myModule.add_module_depend(['etk', 'date'])
	
	myModule.add_export_path(tools.get_current_path(__file__))

	tagFile = tools.get_current_path(__file__) + "/tag"
	galeVersionID = tools.file_read_data(tagFile)
	myModule.compile_flags('c++', [
		"-DGALE_VERSION=\"\\\""+galeVersionID+"\\\"\""
		])
	
	if target.name=="Linux":
		# todo : myModule.add_module_depend(['egami'])
		myModule.add_export_flag('link', '-lGL')
		#`pkg-config --cflags directfb` `pkg-config --libs directfb`
		#ifeq ("$(CONFIG___GALE_LINUX_GUI_MODE_X11__)","y")
		myModule.add_export_flag('link', '-lX11')
		#endif
		#ifeq ("$(CONFIG___GALE_LINUX_GUI_MODE_DIRECT_FB__)","y")
		#myModule.add_export_flag('link', ['-L/usr/local/lib', '-ldirectfb', '-lfusion', '-ldirect'])
		#endif
	elif target.name=="Android":
		myModule.add_module_depend(["SDK", "jvm-basics"])
		myModule.add_export_flag('link', "-lGLESv2")
		# add tre creator of the basic java class ...
		target.add_action("PACKAGE", 50, "gale-auto-wrapper", tool_generate_main_java_class)
		# TODO : Add the same for BINARY to create a console interface ?
	elif target.name=="Windows":
		myModule.add_module_depend("glew")
	elif target.name=="MacOs":
		myModule.add_export_flag('link', [
			"-framework Cocoa",
			"-framework OpenGL",
			"-framework QuartzCore",
			"-framework AppKit"])
	elif target.name=="IOs":
		myModule.add_export_flag('link', [
			"-framework OpenGLES",
			"-framework CoreGraphics",
			"-framework UIKit",
			"-framework GLKit",
			"-framework Foundation",
			"-framework QuartzCore"])
	
	return myModule



##################################################################
##
## Android specific section
##
##################################################################
def tool_generate_main_java_class(target, module, package_name):
	file_list = []
	
	debug.debug("------------------------------------------------------------------------")
	debug.debug("Generate android wrapping for '" + package_name + "'")
	debug.debug("------------------------------------------------------------------------")
	application_name = package_name
	if target.config["mode"] == "debug":
		application_name += "debug"
	target.folder_java_project=   target.get_build_folder(package_name) \
	                            + "/src/" \
	                            + module.package_prop["COMPAGNY_TYPE"] \
	                            + "/" + module.package_prop["COMPAGNY_NAME2"] \
	                            + "/" + application_name + "/"
	
	java_file_wrapper = target.folder_java_project + "/" + application_name + ".java"
	
	android_package_name = module.package_prop["COMPAGNY_TYPE"]+"."+module.package_prop["COMPAGNY_NAME2"]+"." + application_name
	
	debug.print_element("pkg", "absractionFile", "<==", "dynamic file")
	# Create folder :
	tools.create_directory_of_file(java_file_wrapper)
	debug.debug("create file : '" + java_file_wrapper + "'")
	# Create file :
	tmpFile = open(java_file_wrapper + "_tmp", 'w')
	
	tmpFile.write( "/**\n")
	tmpFile.write( " * @author Edouard DUPIN, Kevin BILLONNEAU\n")
	tmpFile.write( " * @copyright 2011, Edouard DUPIN, all right reserved\n")
	tmpFile.write( " * @license APACHE v2.0 (see license file)\n")
	tmpFile.write( " * @note This file is autogenerate ==> see documantation to generate your own\n")
	tmpFile.write( " */\n")
	tmpFile.write( "package "+ android_package_name + ";\n")
	tmpFile.write( "import android.util.Log;\n")
	if module.package_prop["ANDROID_APPL_TYPE"]=="APPL":
		tmpFile.write( "import org.gale.GaleActivity;\n")
	else:
		tmpFile.write( "import org.gale.GaleWallpaper;\n")
	tmpFile.write( "\n")
	
	if "GENERATE_SECTION__IMPORT" in module.package_prop:
		for elem in module.package_prop["GENERATE_SECTION__IMPORT"]:
			for line in elem:
				tmpFile.write( line + "\n")
	if module.package_prop["ANDROID_APPL_TYPE"]=="APPL":
		tmpFile.write( "public class " + application_name + " extends GaleActivity {\n")
	else:
		tmpFile.write( "public class " + application_name + " extends GaleWallpaper {\n")
		tmpFile.write( "	public static final String SHARED_PREFS_NAME = \"" + application_name + "settings\";\n")
	
	if "GENERATE_SECTION__DECLARE" in module.package_prop:
		for elem in module.package_prop["GENERATE_SECTION__DECLARE"]:
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
	if module.package_prop["ANDROID_APPL_TYPE"]!="APPL":
		tmpFile.write( "	public Engine onCreateEngine() {\n")
		tmpFile.write( "		Engine tmpEngine = super.onCreateEngine();\n")
		tmpFile.write( "		initApkPath(\"" + module.package_prop["COMPAGNY_TYPE"]+"\", \""+module.package_prop["COMPAGNY_NAME2"]+"\", \"" + application_name + "\");\n")
		tmpFile.write( "		return tmpEngine;\n")
		tmpFile.write( "	}\n")
	
	if "GENERATE_SECTION__CONSTRUCTOR" in module.package_prop:
		tmpFile.write( "	public " + application_name + "() {\n")
		for elem in module.package_prop["GENERATE_SECTION__CONSTRUCTOR"]:
			for line in elem:
				tmpFile.write( "		" + line + "\n")
		tmpFile.write( "	}\n")
	
	tmpFile.write( "	public void onCreate(android.os.Bundle savedInstanceState) {\n")
	tmpFile.write( "		super.onCreate(savedInstanceState);\n")
	tmpFile.write( "		initApkPath(\"" + module.package_prop["COMPAGNY_TYPE"]+"\", \""+module.package_prop["COMPAGNY_NAME2"]+"\", \"" + application_name + "\");\n")
	
	if "GENERATE_SECTION__ON_CREATE" in module.package_prop:
		for elem in module.package_prop["GENERATE_SECTION__ON_CREATE"]:
			for line in elem:
				tmpFile.write( "		" + line + "\n")
	tmpFile.write( "	}\n")
	
	
	if "GENERATE_SECTION__ON_START" in module.package_prop:
		tmpFile.write( "	@Override protected void onStart() {\n")
		for elem in module.package_prop["GENERATE_SECTION__ON_START"]:
			for line in elem:
				tmpFile.write( "		" + line + "\n")
		tmpFile.write( "		super.onStart();\n")
		tmpFile.write( "	}\n")
	
	if "GENERATE_SECTION__ON_RESTART" in module.package_prop:
		tmpFile.write( "	@Override protected void onRestart() {\n")
		for elem in module.package_prop["GENERATE_SECTION__ON_RESTART"]:
			for line in elem:
				tmpFile.write( "		" + line + "\n")
		tmpFile.write( "		super.onRestart();\n")
		tmpFile.write( "	}\n")
	
	if "GENERATE_SECTION__ON_RESUME" in module.package_prop:
		tmpFile.write( "	@Override protected void onResume() {\n")
		tmpFile.write( "		super.onResume();\n")
		for elem in module.package_prop["GENERATE_SECTION__ON_RESUME"]:
			for line in elem:
				tmpFile.write( "		" + line + "\n")
		tmpFile.write( "	}\n")
	
	if "GENERATE_SECTION__ON_PAUSE" in module.package_prop:
		tmpFile.write( "	@Override protected void onPause() {\n")
		for elem in module.package_prop["GENERATE_SECTION__ON_PAUSE"]:
			for line in elem:
				tmpFile.write( "		" + line + "\n")
		tmpFile.write( "		super.onPause();\n")
		tmpFile.write( "	}\n")
	
	if "GENERATE_SECTION__ON_STOP" in module.package_prop:
		tmpFile.write( "	@Override protected void onStop() {\n")
		for elem in module.package_prop["GENERATE_SECTION__ON_STOP"]:
			for line in elem:
				tmpFile.write( "		" + line + "\n")
		tmpFile.write( "		super.onStop();\n")
		tmpFile.write( "	}\n")
	
	if "GENERATE_SECTION__ON_DESTROY" in module.package_prop:
		tmpFile.write( "	@Override protected void onDestroy() {\n")
		for elem in module.package_prop["GENERATE_SECTION__ON_DESTROY"]:
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
	
	"""
	    ## todo:
	tools.create_directory_of_file(target.get_staging_folder(package_name) + "/res/drawable/icon.png");
	if     "ICON" in module.package_prop.keys() \
	   and module.package_prop["ICON"] != "":
		image.resize(module.package_prop["ICON"], target.get_staging_folder(package_name) + "/res/drawable/icon.png", 256, 256)
	else:
		# to be sure that we have all time a resource ...
		tmpFile = open(target.get_staging_folder(package_name) + "/res/drawable/plop.txt", 'w')
		tmpFile.write('plop\n')
		tmpFile.flush()
		tmpFile.close()
	
	"""
	if module.package_prop["ANDROID_MANIFEST"] == "":
		# force manifest file:
		module.package_prop["ANDROID_MANIFEST"] = target.get_build_folder(package_name) + "/AndroidManifest.xml";
		debug.debug(" create file: '" + module.package_prop["ANDROID_MANIFEST"] + "'")
		if "VERSION_CODE" not in module.package_prop:
			module.package_prop["VERSION_CODE"] = "1"
		debug.print_element("pkg", "AndroidManifest.xml", "<==", "package configurations")
		tools.create_directory_of_file(module.package_prop["ANDROID_MANIFEST"])
		tmpFile = open(module.package_prop["ANDROID_MANIFEST"], 'w')
		tmpFile.write( '<?xml version="1.0" encoding="utf-8"?>\n')
		tmpFile.write( '<!-- Manifest is autoGenerated with Gale ... do not patch it-->\n')
		tmpFile.write( '<manifest xmlns:android="http://schemas.android.com/apk/res/android" \n')
		tmpFile.write( '          package="' + android_package_name + '" \n')
		tmpFile.write( '          android:versionCode="'+module.package_prop["VERSION_CODE"]+'" \n')
		tmpFile.write( '          android:versionName="'+module.package_prop["VERSION"]+'"> \n')
		tmpFile.write( '	<uses-feature android:glEsVersion="0x00020000" android:required="true" />\n')
		tmpFile.write( '	<uses-sdk android:minSdkVersion="' + str(target.boardId) + '" \n')
		tmpFile.write( '	          android:targetSdkVersion="' + str(target.boardId) + '" /> \n')
		if module.package_prop["ANDROID_APPL_TYPE"]=="APPL":
			tmpFile.write( '	<application android:label="' + application_name + '" \n')
			if "ICON" in module.package_prop.keys():
				tmpFile.write( '	             android:icon="@drawable/icon" \n')
			if target.config["mode"] == "debug":
				tmpFile.write( '	             android:debuggable="true" \n')
			tmpFile.write( '	             >\n')
			if "ADMOD_ID" in module.package_prop:
				tmpFile.write( '		<meta-data android:name="com.google.android.gms.version" \n')
				tmpFile.write( '		           android:value="@integer/google_play_services_version"/>\n')
			
			tmpFile.write( '		<activity android:name=".' + application_name + '" \n')
			tmpFile.write( '		          android:label="' + module.package_prop['NAME'])
			if target.config["mode"] == "debug":
				tmpFile.write("-debug")
			tmpFile.write( '"\n')
			if "ICON" in module.package_prop.keys():
				tmpFile.write( '		          android:icon="@drawable/icon" \n')
			tmpFile.write( '		          android:hardwareAccelerated="true" \n')
			tmpFile.write( '		          android:configChanges="keyboard|keyboardHidden|orientation|screenSize"> \n')
			tmpFile.write( '			<intent-filter> \n')
			tmpFile.write( '				<action android:name="android.intent.action.MAIN" /> \n')
			tmpFile.write( '				<category android:name="android.intent.category.LAUNCHER" /> \n')
			tmpFile.write( '			</intent-filter> \n')
			tmpFile.write( '		</activity> \n')
			if "ADMOD_ID" in module.package_prop:
				tmpFile.write( '		<activity android:name="com.google.android.gms.ads.AdActivity"\n')
				tmpFile.write( '		          android:configChanges="keyboard|keyboardHidden|orientation|screenLayout|uiMode|screenSize|smallestScreenSize"/>\n')
			
			tmpFile.write( '	</application>\n')
		else:
			tmpFile.write( '	<application android:label="' + application_name + '" \n')
			tmpFile.write( '	             android:permission="android.permission.BIND_WALLPAPER" \n')
			if "ICON" in module.package_prop.keys():
				tmpFile.write( '	             android:icon="@drawable/icon"\n')
			tmpFile.write( '	             >\n')
			tmpFile.write( '		<service android:name=".' + application_name + '" \n')
			tmpFile.write( '		         android:label="' + module.package_prop['NAME'])
			if target.config["mode"] == "debug":
				tmpFile.write("-debug")
			tmpFile.write( '"\n')
			if "ICON" in module.package_prop.keys():
				tmpFile.write( '		         android:icon="@drawable/icon"\n')
			tmpFile.write( '		         >\n')
			tmpFile.write( '			<intent-filter>\n')
			tmpFile.write( '				<action android:name="android.service.wallpaper.WallpaperService" />\n')
			tmpFile.write( '			</intent-filter>\n')
			tmpFile.write( '			<meta-data android:name="android.service.wallpaper"\n')
			tmpFile.write( '			           android:resource="@xml/' + application_name + '_resource" />\n')
			tmpFile.write( '		</service>\n')
			if len(module.package_prop["ANDROID_WALLPAPER_PROPERTIES"])!=0:
				tmpFile.write( '		<activity android:label="Setting"\n')
				tmpFile.write( '		          android:name=".' + application_name + 'Settings"\n')
				tmpFile.write( '		          android:theme="@android:style/Theme.Light.WallpaperSettings"\n')
				tmpFile.write( '		          android:exported="true"\n')
				if "ICON" in module.package_prop.keys():
					tmpFile.write( '		          android:icon="@drawable/icon"\n')
				tmpFile.write( '		          >\n')
				tmpFile.write( '		</activity>\n')
			tmpFile.write( '	</application>\n')
		# write package autorisations :
		if True==target.check_right_package(module.package_prop, "WRITE_EXTERNAL_STORAGE"):
			tmpFile.write( '	<permission android:name="android.permission.WRITE_EXTERNAL_STORAGE" /> \n')
			tmpFile.write( '	<uses-permission android:name="android.permission.WRITE_EXTERNAL_STORAGE" /> \n')
		if True==target.check_right_package(module.package_prop, "CAMERA"):
			tmpFile.write( '	<permission android:name="android.permission.CAMERA" /> \n')
			tmpFile.write( '	<uses-permission android:name="android.permission.CAMERA" /> \n')
		if True==target.check_right_package(module.package_prop, "INTERNET"):
			tmpFile.write( '	<permission android:name="android.permission.INTERNET" /> \n')
			tmpFile.write( '	<uses-permission android:name="android.permission.INTERNET" /> \n')
		if True==target.check_right_package(module.package_prop, "ACCESS_NETWORK_STATE"):
			tmpFile.write( '	<permission android:name="android.permission.ACCESS_NETWORK_STATE" /> \n')
			tmpFile.write( '	<uses-permission android:name="android.permission.ACCESS_NETWORK_STATE" /> \n')
		if True==target.check_right_package(module.package_prop, "MODIFY_AUDIO_SETTINGS"):
			tmpFile.write( '	<permission android:name="android.permission.MODIFY_AUDIO_SETTINGS" /> \n')
			tmpFile.write( '	<uses-permission android:name="android.permission.MODIFY_AUDIO_SETTINGS" /> \n')
		if True==target.check_right_package(module.package_prop, "READ_CALENDAR"):
			tmpFile.write( '	<permission android:name="android.permission.READ_CALENDAR" /> \n')
			tmpFile.write( '	<uses-permission android:name="android.permission.READ_CALENDAR" /> \n')
		if True==target.check_right_package(module.package_prop, "READ_CONTACTS"):
			tmpFile.write( '	<permission android:name="android.permission.READ_CONTACTS" /> \n')
			tmpFile.write( '	<uses-permission android:name="android.permission.READ_CONTACTS" /> \n')
		if True==target.check_right_package(module.package_prop, "READ_FRAME_BUFFER"):
			tmpFile.write( '	<permission android:name="android.permission.READ_FRAME_BUFFER" /> \n')
			tmpFile.write( '	<uses-permission android:name="android.permission.READ_FRAME_BUFFER" /> \n')
		if True==target.check_right_package(module.package_prop, "READ_PROFILE"):
			tmpFile.write( '	<permission android:name="android.permission.READ_PROFILE" /> \n')
			tmpFile.write( '	<uses-permission android:name="android.permission.READ_PROFILE" /> \n')
		if True==target.check_right_package(module.package_prop, "RECORD_AUDIO"):
			tmpFile.write( '	<permission android:name="android.permission.RECORD_AUDIO" /> \n')
			tmpFile.write( '	<uses-permission android:name="android.permission.RECORD_AUDIO" /> \n')
		if True==target.check_right_package(module.package_prop, "SET_ORIENTATION"):
			tmpFile.write( '	<permission android:name="android.permission.SET_ORIENTATION" /> \n')
			tmpFile.write( '	<uses-permission android:name="android.permission.SET_ORIENTATION" /> \n')
		if True==target.check_right_package(module.package_prop, "VIBRATE"):
			tmpFile.write( '	<permission android:name="android.permission.VIBRATE" /> \n')
			tmpFile.write( '	<uses-permission android:name="android.permission.VIBRATE" /> \n')
		if True==target.check_right_package(module.package_prop, "ACCESS_COARSE_LOCATION"):
			tmpFile.write( '	<permission android:name="android.permission.ACCESS_COARSE_LOCATION" /> \n')
			tmpFile.write( '	<uses-permission android:name="android.permission.ACCESS_COARSE_LOCATION" /> \n')
		if True==target.check_right_package(module.package_prop, "ACCESS_FINE_LOCATION"):
			tmpFile.write( '	<permission android:name="android.permission.ACCESS_FINE_LOCATION" /> \n')
			tmpFile.write( '	<uses-permission android:name="android.permission.ACCESS_FINE_LOCATION" /> \n')
		tmpFile.write( '</manifest>\n\n')
		tmpFile.flush()
		tmpFile.close()
		# end generating android manifest
		
		if module.package_prop["ANDROID_APPL_TYPE"]!="APPL":
			#create the Wallpaper sub files : (main element for the application
			debug.print_element("pkg", application_name + "_resource.xml", "<==", "package configurations")
			tools.create_directory_of_file(target.get_build_folder(package_name) + "/res/xml/" + application_name + "_resource.xml")
			tmpFile = open(target.get_build_folder(package_name) + "/res/xml/" + application_name + "_resource.xml", 'w')
			tmpFile.write( "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n")
			tmpFile.write( "<wallpaper xmlns:android=\"http://schemas.android.com/apk/res/android\"\n")
			if len(module.package_prop["ANDROID_WALLPAPER_PROPERTIES"])!=0:
				tmpFile.write( "           android:settingsActivity=\""+android_package_name + "."+ application_name + "Settings\"\n")
			if "ICON" in module.package_prop.keys():
				tmpFile.write( "           android:thumbnail=\"@drawable/icon\"\n")
			tmpFile.write( "           />\n")
			tmpFile.flush()
			tmpFile.close()
			# create wallpaper setting if needed (class and config file)
			if len(module.package_prop["ANDROID_WALLPAPER_PROPERTIES"])!=0:
				tools.create_directory_of_file(target.folder_java_project + application_name + "Settings.java")
				debug.print_element("pkg", target.folder_java_project + application_name + "Settings.java", "<==", "package configurations")
				tmpFile = open(target.folder_java_project + application_name + "Settings.java", 'w');
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
				
				debug.print_element("pkg", target.get_build_folder(package_name) + "/res/xml/" + application_name + "_settings.xml", "<==", "package configurations")
				tools.create_directory_of_file(target.get_build_folder(package_name) + "/res/xml/" + application_name + "_settings.xml")
				tmpFile = open(target.get_build_folder(package_name) + "/res/xml/" + application_name + "_settings.xml", 'w');
				tmpFile.write( "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n")
				tmpFile.write( "<PreferenceScreen xmlns:android=\"http://schemas.android.com/apk/res/android\"\n")
				tmpFile.write( "                  android:title=\"Settings\"\n")
				tmpFile.write( "                  android:key=\"" + application_name  + "_settings\">\n")
				WALL_haveArray = False
				for WALL_type, WALL_key, WALL_title, WALL_summary, WALL_other in module.package_prop["ANDROID_WALLPAPER_PROPERTIES"]:
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
					for WALL_type, WALL_key, WALL_title, WALL_summary, WALL_other in module.package_prop["ANDROID_WALLPAPER_PROPERTIES"]:
						if WALL_type == "list":
							debug.print_element("pkg", target.get_build_folder(package_name) + "/res/values/" + WALL_key + ".xml", "<==", "package configurations")
							tools.create_directory_of_file(target.get_build_folder(package_name) + "/res/values/" + WALL_key + ".xml")
							tmpFile = open(target.get_build_folder(package_name) + "/res/values/" + WALL_key + ".xml", 'w');
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
	# myModule.pkg_add("ANDROID_WALLPAPER_PROPERTIES", ["list", key, title, summary, [["key","value display"],["key2","value display 2"]])
	# myModule.pkg_add("ANDROID_WALLPAPER_PROPERTIES", ["list", "testpattern", "Select test pattern", "Choose which test pattern to display", [["key","value display"],["key2","value display 2"]]])
	# myModule.pkg_add("ANDROID_WALLPAPER_PROPERTIES", ["bool", key, title, summary, ["enable string", "disable String"])
	# myModule.pkg_add("ANDROID_WALLPAPER_PROPERTIES", ["bool", "movement", "Motion", "Apply movement to test pattern", ["Moving test pattern", "Still test pattern"]
	#copy needed resources :
	for res_source, res_dest in module.package_prop["ANDROID_RESOURCES"]:
		if res_source == "":
			continue
		tools.copy_file(res_source , target.get_staging_folder(package_name) + "/res/" + res_dest + "/" + os.path.basename(res_source), force=True)
	"""
	"""
	# Doc :
	# http://asantoso.wordpress.com/2009/09/15/how-to-build-android-application-package-apk-from-the-command-line-using-the-sdk-tools-continuously-integrated-using-cruisecontrol/
	debug.print_element("pkg", "R.java", "<==", "Resources files")
	tools.create_directory_of_file(target.get_staging_folder(package_name) + "/src/noFile")
	androidToolPath = target.folder_sdk + "/build-tools/"
	# find android tool version
	dirnames = tools.get_list_sub_folder(androidToolPath)
	if len(dirnames) != 1:
		debug.error("an error occured when getting the tools for android")
	androidToolPath += dirnames[0] + "/"
	
	adModResouceFolder = ""
	if "ADMOD_ID" in module.package_prop:
		adModResouceFolder = " -S " + target.folder_sdk + "/extras/google/google_play_services/libproject/google-play-services_lib/res/ "
	cmdLine = androidToolPath + "aapt p -f " \
	          + "-M " + target.get_staging_folder(package_name) + "/AndroidManifest.xml " \
	          + "-F " + target.get_staging_folder(package_name) + "/resources.res " \
	          + "-I " + target.folder_sdk + "/platforms/android-" + str(target.boardId) + "/android.jar "\
	          + "-S " + target.get_staging_folder(package_name) + "/res/ " \
	          + adModResouceFolder \
	          + "-J " + target.get_staging_folder(package_name) + "/src/ "
	multiprocess.run_command(cmdLine)
	#aapt  package -f -M ${manifest.file} -F ${packaged.resource.file} -I ${path.to.android-jar.library} 
	#      -S ${android-resource-directory} [-m -J ${folder.to.output.the.R.java}]
	
	tools.create_directory_of_file(target.get_staging_folder(package_name) + "/build/classes/noFile")
	debug.print_element("pkg", "*.class", "<==", "*.java")
	# more information with : -Xlint
	#          + java_file_wrapper + " "\ # this generate ex: out/Android/debug/staging/tethys/src/com/edouarddupin/tethys/edn.java
	
	#generate android java files:
	filesString=""
	for element in module.package_prop["ANDROID_JAVA_FILES"]:
		if element=="DEFAULT":
			filesString += target.folder_gale + "/android/src/org/gale/GaleAudioTask.java "
			filesString += target.folder_gale + "/android/src/org/gale/GaleCallback.java "
			filesString += target.folder_gale + "/android/src/org/gale/GaleConstants.java "
			filesString += target.folder_gale + "/android/src/org/gale/Gale.java "
			filesString += target.folder_gale + "/android/src/org/gale/GaleRendererGL.java "
			filesString += target.folder_gale + "/android/src/org/gale/GaleSurfaceViewGL.java "
			filesString += target.folder_gale + "/android/src/org/gale/GaleActivity.java "
			filesString += target.folder_gale + "/android/src/org/gale/GaleWallpaper.java "
		else:
			filesString += element + " "
	
	if "ADMOD_ID" in module.package_prop:
		filesString += target.folder_sdk + "/extras/google/google_play_services/libproject/google-play-services_lib/src/android/UnusedStub.java "
		
	if len(module.package_prop["ANDROID_WALLPAPER_PROPERTIES"])!=0:
		filesString += target.folder_java_project + application_name + "Settings.java "
	
	adModJarFile = ""
	if "ADMOD_ID" in module.package_prop:
		adModJarFile = ":" + target.folder_sdk + "/extras/google/google_play_services/libproject/google-play-services_lib/libs/google-play-services.jar"
	
	cmdLine = "javac " \
	          + "-d " + target.get_staging_folder(package_name) + "/build/classes " \
	          + "-classpath " + target.folder_sdk + "/platforms/android-" + str(target.boardId) + "/android.jar" \
	          + adModJarFile + " " \
	          + filesString \
	          + java_file_wrapper + " "  \
	          + target.get_staging_folder(package_name) + "/src/R.java "
	multiprocess.run_command(cmdLine)
	
	debug.print_element("pkg", ".dex", "<==", "*.class")
	cmdLine = androidToolPath + "dx " \
	          + "--dex --no-strict " \
	          + "--output=" + target.get_staging_folder(package_name) + "/build/" + application_name + ".dex " \
	          + target.get_staging_folder(package_name) + "/build/classes/ "
	
	if "ADMOD_ID" in module.package_prop:
		cmdLine += target.folder_sdk + "/extras/google/google_play_services/libproject/google-play-services_lib/libs/google-play-services.jar "
	multiprocess.run_command(cmdLine)
	"""
	return {"files":file_list}



