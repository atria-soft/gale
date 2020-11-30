package org.atriasoft.gale.resource;

import org.atriasoft.etk.Uri;
import org.atriasoft.gale.internal.Log;
import org.atriasoft.gale.backend3d.OpenGL;
import org.atriasoft.gale.backend3d.OpenGL.ShaderType;

public class ResourceShader extends Resource {

	private boolean exist = false; //!< The shader file existed and has been loaded
	private String fileData = ""; //!< A copy of the data loaded from the file (usefull only when opengl context is removed)
	private int shader = -1; //!< opengl id of this element
	private final ShaderType type; //!< Type of the current shader(vertex/fragment)
	private final Uri uri;
	/**
	 * @brief Contructor of an opengl Shader
	 * @param filename Standard file name format. see @ref etk::FSNode
	 */
	protected ResourceShader(Uri uri) {
		super(uri);
		this.uri = uri;
		addResourceType("gale::Shader");
		this.resourceLevel = 0;
		Log.debug("OGL : load SHADER '" + uri + "'");
		// load data from file "all the time ..."
		
		if (uri.get().endsWith(".frag")) {
			this.type = ShaderType.fragment;
		} else if (uri.get().endsWith(".vert")) {
			this.type = ShaderType.vertex;
		} else {
			Log.error("File does not have extention '.vert' for Vertex Shader or '.frag' for Fragment Shader. but : \"" + uri + "\"");
			this.type = ShaderType.vertex;
			return;
		}
		reload();
	}
	/**
	 * @brief Destructor, remove the current Shader
	 */
	public void cleanUp() {
		OpenGL.shaderRemove(this.shader);
		this.exist = false;
	}
	/**
	 * @brief get the opengl reference id of this shader.
	 * @return The opengl id.
	 */
	public int getGLID() {
		return this.shader;
	};
	/**
	 * @brief get the opengl type of this shader.
	 * @return The type of this loaded shader.
	 */
	public ShaderType getShaderType() {
		return this.type;
	};
	/**
	 * @brief This load/reload the data in the opengl context, needed when removed previously.
	 */
	public boolean updateContext(){
		if (this.exist == true) {
			// Do nothing  == > too dangerous ...
		} else {
			this.shader = OpenGL.shaderLoad(this.uri.get(), type);
			// create the Shader
			if (this.shader < 0) {
				return true;
			}
			this.exist = true;
		}
		return true;
	}
	/**
	 * @brief remove the data from the opengl context.
	 */
	public void removeContext(){
		if (true == this.exist) {
			OpenGL.shaderRemove(this.shader);
			this.shader = -1;
			this.exist = false;
		}
	}
	/**
	 * @brief Special android spec! It inform us that all context is removed and after notify us...
	 */
	public void removeContextToLate(){
		this.exist = false;
		this.shader = -1;
	}
	/**
	 * @brief Relode the shader from the file. used when a request of resouces reload is done.
	 * @note this is really usefull when we tested the new themes or shader developpements.
	 */
	public void reload() {
		Log.verbose("load shader:\n-----------------------------------------------------------------\n" + this.fileData + "\n-----------------------------------------------------------------");
		// now change the OGL context ...
		if (OpenGL.hasContext() == true) {
			Log.debug("OGL : load SHADER '" + this.name + "' ==> call update context (direct)");
			removeContext();
			updateContext();
		} else {
			Log.debug("OGL : load SHADER '" + this.name + "' ==> tagged has update context needed");
			// TODO Check this, this is a leek ==> in the GPU ... really bad ...
			this.exist = false;
			this.shader = 0;
			getManager().update(this);
		}
	}
	public static ResourceShader create(Uri uriShader) {
		ResourceShader resource;
		Resource resource2;
		String name = uriShader.getValue();
		if (name.isEmpty() == false && name != "---") {
			resource2 = getManager().localKeep(name);
		} else {
			Log.error("Can not create a shader without a filaname");
			return null;
		}
		if (resource2 != null) {
			if (resource2 instanceof ResourceShader) {
				resource2.keep();
				return (ResourceShader)resource2;
			}
			Log.critical("Request resource file : '" + name + "' With the wrong type (dynamic cast error)");
			return null;
		}
		resource = new ResourceShader(uriShader);
		if (resource == null) {
			Log.error("allocation error of a resource : " + name);
			return null;
		}
		if (resource.resourceHasBeenCorectlyInit() == false) {
			Log.critical("resource Is not correctly init : ResourceProgram" );
			return null;
		}
		getManager().localAdd(resource);
		return resource;
	}
}
