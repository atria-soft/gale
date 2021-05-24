package org.atriasoft.gale.resource;

import org.atriasoft.etk.Uri;
import org.atriasoft.gale.backend3d.OpenGL;
import org.atriasoft.gale.backend3d.OpenGL.ShaderType;
import org.atriasoft.gale.internal.Log;

public class ResourceShader extends Resource {
	
	public static ResourceShader create(final Uri uriShader) {
		ResourceShader resource;
		Resource resource2;
		final String name = uriShader.getValue();
		if (!name.isEmpty() && !name.equals("---")) {
			resource2 = getManager().localKeep(name);
		} else {
			Log.error("Can not create a shader without a filaname");
			return null;
		}
		if (resource2 != null) {
			if (resource2 instanceof ResourceShader) {
				resource2.keep();
				return (ResourceShader) resource2;
			}
			Log.critical("Request resource file : '" + name + "' With the wrong type (dynamic cast error)");
			return null;
		}
		resource = new ResourceShader(uriShader);
		getManager().localAdd(resource);
		return resource;
	}
	
	private boolean exist = false; //!< The shader file existed and has been loaded
	private int shader = -1; //!< opengl id of this element
	private final ShaderType type; //!< Type of the current shader(vertex/fragment)
	private final Uri uri;
	
	/**
	 * Constructor of an opengl Shader
	 * @param uri Standard file name format. see @ref etk::FSNode
	 */
	protected ResourceShader(final Uri uri) {
		super(uri);
		this.uri = uri;
		this.resourceLevel = 0;
		Log.debug("OGL : load SHADER '" + uri + "'");
		// load data from file "all the time ..."
		
		if (uri.get().endsWith(".frag")) {
			this.type = ShaderType.FRAGMENT;
		} else if (uri.get().endsWith(".vert")) {
			this.type = ShaderType.VERTEX;
		} else {
			Log.error("File does not have extention '.vert' for Vertex Shader or '.frag' for Fragment Shader. but : \"" + uri + "\"");
			this.type = ShaderType.VERTEX;
			return;
		}
		reload();
	}
	
	/**
	 * Destructor, remove the current Shader
	 */
	@Override
	public void cleanUp() {
		OpenGL.shaderRemove(this.shader);
		this.exist = false;
	};
	
	/**
	 * get the opengl reference id of this shader.
	 * @return The opengl id.
	 */
	public int getGLID() {
		return this.shader;
	};
	
	/**
	 * get the opengl type of this shader.
	 * @return The type of this loaded shader.
	 */
	public ShaderType getShaderType() {
		return this.type;
	}
	
	/**
	 * Reloaded the shader from the file. used when a request of resources reload is done.
	 * @note this is really useful when we tested the new themes or shader developments.
	 */
	@Override
	public void reload() {
		//!< A copy of the data loaded from the file (useful only when opengl context is removed)
		String fileData = "";
		Log.verbose("load shader:\n-----------------------------------------------------------------\n" + fileData + "\n-----------------------------------------------------------------");
		// now change the OGL context ...
		if (OpenGL.hasContext()) {
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
	
	/**
	 * remove the data from the opengl context.
	 */
	@Override
	public void removeContext() {
		if (this.exist) {
			OpenGL.shaderRemove(this.shader);
			this.shader = -1;
			this.exist = false;
		}
	}
	
	/**
	 * Special android spec! It inform us that all context is removed and after notify us...
	 */
	@Override
	public void removeContextToLate() {
		this.exist = false;
		this.shader = -1;
	}
	
	/**
	 * This load/reload the data in the opengl context, needed when removed previously.
	 */
	@Override
	public boolean updateContext() {
		if (!this.exist) {
			this.shader = OpenGL.shaderLoad(this.uri, this.type);
			// create the Shader
			if (this.shader < 0) {
				return true;
			}
			this.exist = true;
		}
		return true;
	}
}
