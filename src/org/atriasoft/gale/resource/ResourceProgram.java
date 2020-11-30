package org.atriasoft.gale.resource;

import java.nio.FloatBuffer;
import java.util.ArrayList;
import java.util.List;

import org.atriasoft.etk.Color;
import org.atriasoft.etk.Uri;
import org.atriasoft.etk.math.Matrix4f;
import org.atriasoft.etk.math.Vector2f;
import org.atriasoft.etk.math.Vector2i;
import org.atriasoft.etk.math.Vector3f;
import org.atriasoft.etk.math.Vector3i;
import org.atriasoft.gale.internal.Log;
import org.atriasoft.gale.backend3d.OpenGL;
import org.lwjgl.BufferUtils;
import org.lwjgl.opengl.GL40;


class ProgAttributeElement {
	public String name; //!< Name of the element
	public int elementId; //!< openGl Id if this element  == > can not exist ==> @ref m_isLinked
	public boolean isAttribute; //!< true if it was an attribute element, otherwite it was an uniform
	public boolean isLinked; //!< if this element does not exist this is false
};

public class ResourceProgram extends Resource {
	static final boolean DEBUG = false; // TODO externalize this ...
	private boolean exist = false; //!< the file existed
	private int program = 0; //!< openGL id of the current program
	private ResourceShader shaderVertex = null;
	private ResourceShader shaderFragment = null;
	private List<ProgAttributeElement> elementList = new ArrayList<ProgAttributeElement>(); //!< List of all the attribute requested by the user
//	private List<Integer> listOfVBOUsed = new ArrayList<Integer>(); //!< retain the list of VBO used to disable it when unuse program ...
//	private boolean hasTexture = false; //!< A texture has been set to the current shader
//	private boolean hasTexture1 = false; //!< A texture has been set to the current shader
	/**
	 * @brief Contructor of an opengl Program.
	 * @param uri Uri of the file
	 */
	protected ResourceProgram(Uri uriVertexShader, Uri uriFragmentShader) {
		super(uriVertexShader.getValue() + "<-->" + uriFragmentShader.getValue());
		addResourceType("gale::resource::Program");
		this.resourceLevel = 1;
		Log.debug("OGL : load PROGRAM '" + uriVertexShader + "' && '" + uriFragmentShader + "'");
		shaderVertex = ResourceShader.create(uriVertexShader);
		if (shaderVertex == null) {
			Log.error("Error while getting a specific shader filename: " + uriVertexShader);
			return;
		} else {
			Log.debug("Add shader on program: '"+ uriFragmentShader + "'");
		}
		shaderFragment = ResourceShader.create(uriFragmentShader);
		if (shaderFragment == null) {
			Log.error("Error while getting a specific shader filename: " + uriFragmentShader);
			return;
		} else {
			Log.debug("Add shader on program : "+ uriFragmentShader + "frag");
		}
		if (OpenGL.hasContext() == true) {
			updateContext();
		} else {
			getManager().update(this);
		}
	}
	/**
	 * @brief Destructor, remove the current Program.
	 */
	@Override
	public void cleanUp() {
		removeContext();
		if (this.shaderFragment != null) {
			this.shaderFragment.release();
			this.shaderFragment = null;
		}
		if (this.shaderVertex != null) {
			this.shaderVertex.release();
			this.shaderVertex = null;
		}
		this.elementList.clear();
//		this.hasTexture = false;
//		this.hasTexture1 = false;
	}
	/**
	 * @brief Check If an Id is valid in the shader or not (sometime the shader have not some attribute, then we need to display some error)
	 * @return idElem Id of the Attribute that might be sended.
	 * @return true The id is valid, false otherwise
	 */
	public boolean checkIdValidity(int idElem){
		if (    idElem < 0
		     || idElem > this.elementList.size()) {
			return false;
		}
		return this.elementList.get(idElem).isLinked;
	}
	/**
	 * @brief User request an attribute on this program.
	 * @note The attribute is send to the fragment shaders
	 * @param elementName Name of the requested attribute.
	 * @return An abstract ID of the current attribute (this value is all time availlable, even if the program will be reloaded)
	 */
	public int getAttribute(String elementName) {
		// check if it exist previously :
		for(int iii=0; iii<this.elementList.size(); iii++) {
			if (this.elementList.get(iii).name.contentEquals(elementName)) {
				return iii;
			}
		}
		ProgAttributeElement tmp = new ProgAttributeElement();
		tmp.name = elementName;
		tmp.isAttribute = true;
		if (OpenGL.hasContext() == false) {
			getManager().update(this);
			tmp.elementId = -1;
			tmp.isLinked = false;
		} else if (this.exist == true) {
			tmp.elementId = OpenGL.programGetAttributeLocation(this.program, tmp.name);
			tmp.isLinked = true;
			if (tmp.elementId<0) {
				Log.warning("    {" + this.program + "}[" + this.elementList.size() + "] glGetAttribLocation(\"" + tmp.name + "\") = " + tmp.elementId);
				tmp.isLinked = false;
			} else {
				Log.debug("    {" + this.program + "}[" + this.elementList.size() + "] glGetAttribLocation(\"" + tmp.name + "\") = " + tmp.elementId);
			}
		} else {
			// program is not loaded ==> just local reister ...
			tmp.elementId = -1;
			tmp.isLinked = false;
		}
		this.elementList.add(tmp);
		return this.elementList.size()-1;
	}
	private FloatBuffer storeDataInFloatBuffer(float[] data) {
		FloatBuffer buffer = BufferUtils.createFloatBuffer(data.length);
		buffer.put(data);
		buffer.flip();
		return buffer;
	}
	public void bindAttribute(int attribute, String variableName) {
		if (this.exist == false) {
			return;
		}
		OpenGL.programBindAttribute(program, attribute, variableName);
	}
	/**
	 * @brief Send attribute table to the spefified ID attribure (not send if does not really exist in the openGL program).
	 * @param idElem Id of the Attribute that might be sended.
	 * @param vbo Reference on the buffer to send.
	 * @param index Reference on the buffer to send.
	 * @param jumpBetweenSample Number of byte to jump between 2 vertex (this permit to enterlace informations)
	 * @param offset offset of start the elements send.
	 */
//	public void sendAttributePointer(int idElem,
//            ResourceVirtualArrayObject vbo,
//            int index,
//            int coordinateSize,
//            float[] data){
//		
//		if (!this.exist) {
//			return;
//		}
//		if (    idElem < 0
//		     || (long)idElem > this.elementList.size()) {
//			Log.error("idElem = " + idElem + " not in [0.." + (this.elementList.size()-1) + "]");
//			return;
//		}
//		if (this.elementList.get(idElem).isLinked == false) {
//			return;
//		}
//		// check error of the VBO goog enought ...
//		if (vbo.getElementSize(index) <= 0) {
//			Log.error("Can not bind a VBO Buffer with an element size of : " + vbo.getElementSize(index) + " named=" + vbo.getName());
//			return;
//		}
//		
//		Log.verbose("[" + this.elementList.get(idElem).name + "] send " + vbo.getElementSize(index) + " element on oglID=" + vbo.getGLID(index) + " VBOindex=" + index);
//		OpenGL.bindBuffer(vbo.getGLID(index));
//		Log.verbose("    id=" + this.elementList.get(idElem).elementId);
//		Log.verbose("    eleme size=" + vbo.getElementSize(index));
//		OpenGL.bufferData(data, Usage.staticDraw);
//		OpenGL.vertexAttribPointerFloat(this.elementList.get(idElem).elementId, vbo.getElementSize(index)); // Pointer on the buffer
//		OpenGL.unbindBuffer();
//		//glEnableVertexAttribArray(this.elementList.get(idElem).elementId);
//		this.listOfVBOUsed.add(this.elementList.get(idElem).elementId);
//	}
//	public void sendAttributePointer2(int idElem,
//            ResourceVirtualArrayObject vbo,
//            int index) {
//        int jumpBetweenSample = 0;
//        int offset = 0;
//		if (!this.exist) {
//			return;
//		}
//		if (    idElem < 0
//		     || idElem > this.elementList.size()) {
//			Log.error("idElem = " + idElem + " not in [0.." + (this.elementList.size()-1) + "]");
//			return;
//		}
//		if (this.elementList.get(idElem).isLinked == false) {
//			return;
//		}
//		// check error of the VBO good enough ...
//		if (vbo.getElementSize(index) <= 0) {
//			Log.error("Can not bind a VBO Buffer with an element size of : " + vbo.getElementSize(index) + " named=" + vbo.getName());
//			return;
//		}
//		
//		Log.verbose("[" + elementList.get(idElem).name + "] send " + vbo.getElementSize(index) + " element on oglID=" + vbo.getGLID(index) + " VBOindex=" + index);
//		GL20.glBindBuffer(GL20.GL_ARRAY_BUFFER, vbo.getGLID(index));
//		//checkGlError("glBindBuffer", __LINE__, _idElem);
//		Log.verbose("    id=" + elementList.get(idElem).elementId);
//		Log.verbose("    eleme size=" + vbo.getElementSize(index));
//		Log.verbose("    jump sample=" + jumpBetweenSample);
//		Log.verbose("    offset=" + offset);
//		GL20.glVertexAttribPointer(elementList.get(idElem).elementId, // attribute ID of openGL
//		vbo.getElementSize(index), // number of elements per vertex, here (r,g,b,a)
//		GL11.GL_FLOAT, // the type of each element
//		false, // take our values as-is
//		0, // no extra data between each position
//		0); // Pointer on the buffer
//		//checkGlError("glVertexAttribPointer", __LINE__, _idElem);
//		GL20.glEnableVertexAttribArray(elementList.get(idElem).elementId);
//		listOfVBOUsed.add(elementList.get(idElem).elementId);
//		//checkGlError("glEnableVertexAttribArray", __LINE__, _idElem);
//	}

//	private void storeDataInAttributeList(int attributeNumber, int coordinateSize, float[] data) {
//		int vboID = GL15.glGenBuffers();
//		vbos.add(vboID);
//		OpenGL.bindBuffer(vboID);
//		FloatBuffer buffer = storeDataInFloatBuffer(data);
//		GL15.glBufferData(GL15.GL_ARRAY_BUFFER, buffer, GL15.GL_STATIC_DRAW);
//		GL20.glVertexAttribPointer(attributeNumber, coordinateSize, GL11.GL_FLOAT, false, 0, 0);
//		GL15.glBindBuffer(GL15.GL_ARRAY_BUFFER, 0);
//	}

	private float[] convertInFloat(List<Vector3f> data) {
		float[] out = new float[data.size()*3];
		for (int iii=0; iii<data.size(); iii++) {
			out[iii*3] = data.get(iii).x;
			out[iii*3+1] = data.get(iii).y;
			out[iii*3+2] = data.get(iii).z;
		}
		return out;
	}
	/**
	 * @brief Send attribute table to the specified ID attribute (not send if does not really exist in the openGL program).
	 * @param idElem Id of the Attribute that might be sent.
	 * @param nbElement Specifies the number of elements that are to be modified.
	 * @param pointer Pointer on the data that might be sent.
	 * @param jumpBetweenSample Number of byte to jump between 2 vertex (this permit to interlace informations)
	 */
//	public void sendAttribute3fv(int idElem, float[] data) {
//		if (!this.exist) {
//			return;
//		}
//		if (    idElem < 0
//		     || (long)idElem > this.elementList.size()) {
//			Log.error("idElem = " + idElem + " not in [0.." + (this.elementList.size()-1) + "]");
//			return;
//		}
//		if (this.elementList.get(idElem).isLinked == false) {
//			return;
//		}
//		FloatBuffer buffer = storeDataInFloatBuffer(data);
//		//GL40.glBindVertexArray(this.elementList.get(idElem).elementId);
//		Log.error("[" + this.elementList.get(idElem).name + "] send " + data.length + " element");
//		GL40.glVertexAttribPointer(
//				this.elementList.get(idElem).elementId,
//				data.length,
//				GL40.GL_FLOAT,
//				false,
//				0,
//				buffer);
//		//checkGlError("glVertexAttribPointer", LINE, idElem);
//		GL40.glEnableVertexAttribArray(this.elementList.get(idElem).elementId);
//		//checkGlError("glEnableVertexAttribArray", LINE, idElem);
//	}
////	public void sendAttribute(int idElem,  etk::Vector<Vector2f> data) {
////		sendAttribute(idElem, 2/*u,v / x,y*/, data[0]);
////	}
//	public void sendAttribute(int idElem,  List<Vector3f> data) {
//		sendAttribute3fv(idElem, convertInFloat(data));
//	}
	public void sendAttribute(int idElem, int nbElement, FloatBuffer data, int jumpBetweenSample) {
		if (this.exist == false) {
			return;
		}
		if (    idElem < 0
		     || (long)idElem > this.elementList.size()) {
			Log.error("idElem = " + idElem + " not in [0.." + (this.elementList.size()-1) + "]");
			return;
		}
		if (this.elementList.get(idElem).isLinked == false) {
			return;
		}
		//GL40.glBindVertexArray(this.elementList.get(idElem).elementId);
		//Log.error("[" + this.elementList.get(idElem).name + "] send " + 3 + " element");
		GL40.glVertexAttribPointer(
				this.elementList.get(idElem).elementId,
				nbElement,
				GL40.GL_FLOAT,
				false,
				jumpBetweenSample*4, /* 4 is the size of float in the generic system...*/
				data);
		//checkGlError("glVertexAttribPointer", LINE, idElem);
		GL40.glEnableVertexAttribArray(this.elementList.get(idElem).elementId);
		//checkGlError("glEnableVertexAttribArray", LINE, idElem);
	}
//	public void sendAttribute(int idElem,  etk::Vector<etk::Color<float>> data) {
//		sendAttribute(idElem, 4/*r,g,b,a*/, data[0]);
//	}
//	public void sendAttribute(int idElem,  etk::Vector<float> data) {
//		sendAttribute(idElem, 1, data[0]);
//	}
	/**
	 * @brief User request an Uniform on this program.
	 * @note uniform value is availlable for all the fragment shader in the program (only one value for all)
	 * @param elementName Name of the requested uniform.
	 * @return An abstract ID of the current uniform (this value is all time availlable, even if the program will be reloaded)
	 */
	public int getUniform(String elementName){
		// check if it exist previously :
		for(int iii=0; iii<this.elementList.size(); iii++) {
			if (this.elementList.get(iii).name.contentEquals(elementName)) {
				return iii;
			}
		}
		ProgAttributeElement tmp = new ProgAttributeElement();
		tmp.name = elementName;
		tmp.isAttribute = false;
		if (OpenGL.hasContext() == false) {
			getManager().update(this);
			tmp.elementId = -1;
			tmp.isLinked = false;
		} else if (this.exist == true) {
			tmp.elementId = OpenGL.programGetUniformLocation(this.program, tmp.name);
			tmp.isLinked = true;
			if (tmp.elementId<0) {
				Log.warning("    {" + this.program + "}[" + this.elementList.size() + "] glGetUniformLocation(\"" + tmp.name + "\") = " + tmp.elementId);
				tmp.isLinked = false;
			} else {
				Log.debug("    {" + this.program + "}[" + this.elementList.size() + "] glGetUniformLocation(\"" + tmp.name + "\") = " + tmp.elementId);
			}
		} else {
			// program is not loaded ==> just local reister ...
			tmp.elementId = -1;
			tmp.isLinked = false;
		}
		this.elementList.add(tmp);
		return this.elementList.size()-1;
	}
	/**
	 * @brief Send a uniform element to the spefified ID (not send if does not really exist in the openGL program)
	 * @param idElem Id of the uniform that might be sended.
	 * @param matrix Matrix that might be sended.
	 * @param transpose Transpose the matrix (needed all the taime in the normal openGl access (only not done in the openGL-ES2 due to the fact we must done it ourself)
	 */
	public void uniformMatrix(int idElem, Matrix4f matrix) {
		uniformMatrix(idElem, matrix, true);
	}
	public void uniformMatrix(int idElem, Matrix4f matrix, boolean transpose/*=true*/) {
		if (this.exist == false) {
			return;
		}
		if (    idElem < 0
		     || (long)idElem > this.elementList.size()) {
			Log.error("idElem = " + idElem + " not in [0.." + (this.elementList.size()-1) + "]");
			return;
		}
		if (false == this.elementList.get(idElem).isLinked) {
			return;
		}
		//Log.error("[" + this.elementList.get(idElem).name + "] send 1 matrix");
		// note : Android des not supported the transposition of the matrix, then we will done it oursef:
		/*
		if (transpose == true) {
			Matrix4f tmp = matrix;
			tmp.transpose();
			programLoadUniformMatrix(this.elementList.get(idElem).elementId, tmp.mat);
		} else {
			programLoadUniformMatrix(this.elementList.get(idElem).elementId, matrix.mat);
		}
		*/
		OpenGL.programLoadUniformMatrix(this.elementList.get(idElem).elementId, matrix, transpose);
	}

	public void uniformColor(int idElem, Color value) {
		if (this.exist == false) {
			return;
		}
		if (idElem<0 || (long)idElem>this.elementList.size()) {
			Log.error("idElem = " + idElem + " not in [0.." + (this.elementList.size()-1) + "]");
			return;
		}
		if (false == this.elementList.get(idElem).isLinked) {
			return;
		}
		OpenGL.programLoadUniformColor(this.elementList.get(idElem).elementId, value);
	}
	public void uniformVector(int idElem, Vector2f value) {
		if (this.exist == false) {
			return;
		}
		if (idElem<0 || (long)idElem>this.elementList.size()) {
			Log.error("idElem = " + idElem + " not in [0.." + (this.elementList.size()-1) + "]");
			return;
		}
		if (false == this.elementList.get(idElem).isLinked) {
			return;
		}
		OpenGL.programLoadUniformVector(this.elementList.get(idElem).elementId, value);
	}
	public void uniformVector(int idElem, Vector2i value) {
		if (this.exist == false) {
			return;
		}
		if (idElem<0 || (long)idElem>this.elementList.size()) {
			Log.error("idElem = " + idElem + " not in [0.." + (this.elementList.size()-1) + "]");
			return;
		}
		if (false == this.elementList.get(idElem).isLinked) {
			return;
		}
		OpenGL.programLoadUniformVector(this.elementList.get(idElem).elementId, value);
	}
	public void uniformVector(int idElem, Vector3f value) {
		if (this.exist == false) {
			return;
		}
		if (idElem<0 || (long)idElem>this.elementList.size()) {
			Log.error("idElem = " + idElem + " not in [0.." + (this.elementList.size()-1) + "]");
			return;
		}
		if (false == this.elementList.get(idElem).isLinked) {
			return;
		}
		OpenGL.programLoadUniformVector(this.elementList.get(idElem).elementId, value);
	}
	public void uniformVector(int idElem, Vector3i value) {
		if (this.exist == false) {
			return;
		}
		if (idElem<0 || (long)idElem>this.elementList.size()) {
			Log.error("idElem = " + idElem + " not in [0.." + (this.elementList.size()-1) + "]");
			return;
		}
		if (false == this.elementList.get(idElem).isLinked) {
			return;
		}
		OpenGL.programLoadUniformVector(this.elementList.get(idElem).elementId, value);
	}
	
	/**
	 * @brief Send 1 float uniform element to the spefified ID (not send if does not really exist in the openGL program)
	 * @param idElem Id of the uniform that might be sended.
	 * @param value1 Value to send at the Uniform
	 */
	public void uniformFloat(int idElem, float value1){
		if (this.exist == false) {
			return;
		}
		if (idElem<0 || (long)idElem>this.elementList.size()) {
			Log.error("idElem = " + idElem + " not in [0.." + (this.elementList.size()-1) + "]");
			return;
		}
		if (false == this.elementList.get(idElem).isLinked) {
			return;
		}
		OpenGL.programLoadUniformFloat(this.elementList.get(idElem).elementId, value1);
	}
	/**
	 * @brief Send 2 float uniform element to the spefified ID (not send if does not really exist in the openGL program)
	 * @param idElem Id of the uniform that might be sended.
	 * @param value1 Value to send at the Uniform
	 * @param value2 Value to send at the Uniform
	 */
	public void uniformFloat(int idElem, float value1, float value2) {
		
		if (this.exist == false) {
			return;
		}
		if (idElem<0 || (long)idElem>this.elementList.size()) {
			Log.error("idElem = " + idElem + " not in [0.." + (this.elementList.size()-1) + "]");
			return;
		}
		if (false == this.elementList.get(idElem).isLinked) {
			return;
		}
		OpenGL.programLoadUniformFloat(this.elementList.get(idElem).elementId, value1, value2);
	}
	/**
	 * @brief Send 3 float uniform element to the spefified ID (not send if does not really exist in the openGL program)
	 * @param idElem Id of the uniform that might be sended.
	 * @param value1 Value to send at the Uniform
	 * @param value2 Value to send at the Uniform
	 * @param value3 Value to send at the Uniform
	 */
	public void uniformFloat(int idElem, float value1, float value2, float value3){
		
		if (this.exist == false) {
			return;
		}
		if (idElem<0 || (long)idElem>this.elementList.size()) {
			Log.error("idElem = " + idElem + " not in [0.." + (this.elementList.size()-1) + "]");
			return;
		}
		if (false == this.elementList.get(idElem).isLinked) {
			return;
		}
		OpenGL.programLoadUniformFloat(this.elementList.get(idElem).elementId, value1, value2, value3);
	}
	/**
	 * @brief Send 4 float uniform element to the spefified ID (not send if does not really exist in the openGL program)
	 * @param idElem Id of the uniform that might be sended.
	 * @param value1 Value to send at the Uniform
	 * @param value2 Value to send at the Uniform
	 * @param value3 Value to send at the Uniform
	 * @param value4 Value to send at the Uniform
	 */
	public void uniformFloat(int idElem, float value1, float value2, float value3, float value4) {
		
		if (this.exist == false) {
			return;
		}
		if (idElem<0 || (long)idElem>this.elementList.size()) {
			Log.error("idElem = " + idElem + " not in [0.." + (this.elementList.size()-1) + "]");
			return;
		}
		if (false == this.elementList.get(idElem).isLinked) {
			return;
		}
		OpenGL.programLoadUniformFloat(this.elementList.get(idElem).elementId, value1, value2, value3, value4);
	}
	
	/**
	 * @brief Send 1 signed integer uniform element to the spefified ID (not send if does not really exist in the openGL program)
	 * @param idElem Id of the uniform that might be sended.
	 * @param value1 Value to send at the Uniform
	 */
	public void uniformInt(int idElem, int value1){
		
		if (this.exist == false) {
			return;
		}
		if (idElem<0 || (long)idElem>this.elementList.size()) {
			Log.error("idElem = " + idElem + " not in [0.." + (this.elementList.size()-1) + "]");
			return;
		}
		if (false == this.elementList.get(idElem).isLinked) {
			return;
		}
		OpenGL.programLoadUniformInt(this.elementList.get(idElem).elementId, value1);
	}
	/**
	 * @brief Send 2 signed integer uniform element to the spefified ID (not send if does not really exist in the openGL program)
	 * @param idElem Id of the uniform that might be sended.
	 * @param value1 Value to send at the Uniform
	 * @param value2 Value to send at the Uniform
	 */
	public void uniformInt(int idElem, int value1, int value2) {
		
		if (this.exist == false) {
			return;
		}
		if (idElem<0 || (long)idElem>this.elementList.size()) {
			Log.error("idElem = " + idElem + " not in [0.." + (this.elementList.size()-1) + "]");
			return;
		}
		if (false == this.elementList.get(idElem).isLinked) {
			return;
		}
		OpenGL.programLoadUniformInt(this.elementList.get(idElem).elementId, value1, value2);
	}
	/**
	 * @brief Send 3 signed integer uniform element to the spefified ID (not send if does not really exist in the openGL program)
	 * @param idElem Id of the uniform that might be sended.
	 * @param value1 Value to send at the Uniform
	 * @param value2 Value to send at the Uniform
	 * @param value3 Value to send at the Uniform
	 */
	public void uniformInt(int idElem, int value1, int value2, int value3){
		
		if (this.exist == false) {
			return;
		}
		if (idElem<0 || (long)idElem>this.elementList.size()) {
			Log.error("idElem = " + idElem + " not in [0.." + (this.elementList.size()-1) + "]");
			return;
		}
		if (false == this.elementList.get(idElem).isLinked) {
			return;
		}
		OpenGL.programLoadUniformInt(this.elementList.get(idElem).elementId, value1, value2, value3);
	}
	/**
	 * @brief Send 4 signed integer uniform element to the spefified ID (not send if does not really exist in the openGL program)
	 * @param idElem Id of the uniform that might be sended.
	 * @param value1 Value to send at the Uniform
	 * @param value2 Value to send at the Uniform
	 * @param value3 Value to send at the Uniform
	 * @param value4 Value to send at the Uniform
	 */
	public void uniformInt(int idElem, int value1, int value2, int value3, int value4){
		
		if (this.exist == false) {
			return;
		}
		if (idElem<0 || (long)idElem>this.elementList.size()) {
			Log.error("idElem = " + idElem + " not in [0.." + (this.elementList.size()-1) + "]");
			return;
		}
		if (false == this.elementList.get(idElem).isLinked) {
			return;
		}
		OpenGL.programLoadUniformInt(this.elementList.get(idElem).elementId, value1, value2, value3, value4);
	}
	
	/**
	 * @brief Request the processing of this program
	 */
	public void use() {
		//Log.verbose("Will use program : " + this.program);
		// event if it was 0  == > set it to prevent other use of the previous shader display ...
		OpenGL.programUse(this.program);
	}
//	/**
//	 * @brief set the testure Id on the specify uniform element.
//	 * @param idElem Id of the uniform that might be sended.
//	 * @param textureOpenGlID Real openGL texture ID
//	 */
//	public void setTexture0(int idElem, int textureOpenGlID){
//		
//		if (!this.exist) {
//			return;
//		}
//		if (    idElem < 0
//		     || (long)idElem > this.elementList.size()) {
//			return;
//		}
//		if (this.elementList.get(idElem).isLinked == false) {
//			return;
//		}
//		OpenGL.activeTexture(GL13.GL_TEXTURE0);
//		// set the textureID
//		GL13.glBindTexture(GL13.GL_TEXTURE_2D, textureOpenGlID);
//		// set the texture on the uniform attribute
//		GL20.glUniform1i(this.elementList.get(idElem).elementId, /*GLTEXTURE*/0);
//		this.hasTexture = true;
//	}
//	public void setTexture1(int idElem, int textureOpenGlID) {
//		if (!this.exist) {
//			return;
//		}
//		if (    idElem < 0
//		     || (long)idElem > this.elementList.size()) {
//			return;
//		}
//		if (this.elementList.get(idElem).isLinked == false) {
//			return;
//		}
//		OpenGL.activeTexture(GL13.GL_TEXTURE1);
//		// set the textureID
//		GL13.glBindTexture(GL13.GL_TEXTURE_2D, textureOpenGlID);
//		// set the texture on the uniform attribute
//		GL20.glUniform1i(this.elementList.get(idElem).elementId, /*GLTEXTURE*/1);
//		this.hasTexture1 = true;
//	}
	/**
	 * @brief Stop the processing of this program
	 */
	public void unUse() {
		//Log.verbose("Will UN-use program : " + this.program);
		
		if (this.exist == false) {
			return;
		}
//		for (Integer it : this.listOfVBOUsed) {
//			GL20.glDisableVertexAttribArray(it);
//		}
//		this.listOfVBOUsed.clear();
		// no need to disable program  == > this only generate perturbation on speed ...
		OpenGL.programUse(-1);
	}

	
	/**
	 * @brief This load/reload the data in the opengl context, needed when removed previously.
	 */
	public boolean updateContext(){
		if (this.exist == true) {
			// Do nothing  == > too dangerous ...
		} else {
			// create the Shader
			Log.debug("Create the Program ...'" + this.name + "'");
			this.program = OpenGL.programCreate();
			if (this.program < 0) {
				return true;
			}
			// first attach vertex shader, and after fragment shader
			if (this.shaderVertex != null) {
				OpenGL.programAttach(this.program, this.shaderVertex.getGLID());
			}
			if (this.shaderFragment != null) {
				OpenGL.programAttach(this.program, this.shaderFragment.getGLID());
			}
			
			OpenGL.programBindAttribute(this.program, ResourceVirtualArrayObject.INDICE_VBO_POSITIONS, "in_position");
			OpenGL.programBindAttribute(this.program, ResourceVirtualArrayObject.INDICE_VBO_TEXTURE_COORDINATES, "tin_extureCoords");
			OpenGL.programBindAttribute(this.program, ResourceVirtualArrayObject.INDICE_VBO_NORMALS, "in_normal");
			OpenGL.programBindAttribute(this.program, ResourceVirtualArrayObject.INDICE_VBO_COLORS, "in_colors");
			
			if (OpenGL.programCompile(this.program) == false) {
				Log.error("Could not compile'PROGRAM':'" + this.name + "'");
				OpenGL.programRemove(this.program);
				return true;
			}
			// now get the old attribute requested priviously ...
			long iii = 0;
			for(ProgAttributeElement it : this.elementList) {
				if (it.isAttribute == true) {
					it.elementId = OpenGL.programGetAttributeLocation(this.program, it.name);
					it.isLinked = true;
					if (it.elementId<0) {
						Log.warning("    {" + this.program + "}[" + iii + "] openGL::getAttributeLocation(\"" + it.name + "\") = " + it.elementId);
						it.isLinked = false;
					} else {
						Log.debug("    {" + this.program + "}[" + iii + "] openGL::getAttributeLocation(\"" + it.name + "\") = " + it.elementId);
					}
				} else {
					it.elementId = OpenGL.programGetUniformLocation(this.program, it.name);
					it.isLinked = true;
					if (it.elementId < 0) {
						Log.warning("    {" + this.program + "}[" + iii + "] openGL::getUniformLocation(\"" + it.name + "\") = " + it.elementId);
						it.isLinked = false;
					} else {
						Log.debug("    {" + this.program + "}[" + iii + "] openGL::getUniformLocation(\"" + it.name + "\") = " + it.elementId);
					}
				}
				iii++;
			}
			// It will existed only when all is updated...
			this.exist = true;
		}
		return true;
	}
	/**
	 * @brief remove the data from the opengl context.
	 */
	public void removeContext(){
		if (this.exist == true) {
			OpenGL.programRemove(this.program);
			this.program = 0;
			this.exist = false;
			for(ProgAttributeElement it : this.elementList) {
				it.elementId=0;
				it.isLinked = false;
			}
		}
	}
	/**
	 * @brief Special android spec! It inform us that all context is removed and after notify us...
	 */
	public void removeContextToLate(){
		
		this.exist = false;
		this.program = 0;
	}
	/**
	 * @brief Relode the shader from the file. used when a request of resouces reload is done.
	 * @note this is really usefull when we tested the new themes or shader developpements.
	 */
	public void reload(){
		/* TODO ...
		etk::file file(this.name, etk::FILETYPEDATA);
		if (file.Exist() == false) {
			Log.error("File does not Exist :"" + file + "\"");
			return;
		}
		
		int fileSize = file.size();
		if (fileSize == 0) {
			Log.error("This file is empty : " + file);
			return;
		}
		if (file.fOpenRead() == false) {
			Log.error("Can not open the file : " + file);
			return;
		}
		// remove previous data ...
		if (this.fileData != null) {
			del ete[] this.fileData;
			this.fileData = 0;
		}
		// allocate data
		this.fileData = ne w char[fileSize+5];
		if (this.fileData == null) {
			Log.error("Error Memory allocation size=" + fileSize);
			return;
		}
		memset(this.fileData, 0, (fileSize+5)*sizeof(char));
		// load data from the file :
		file.fRead(this.fileData, 1, fileSize);
		// close the file:
		file.fClose();
	*/
		// now change the OGL context ...
		removeContext();
		updateContext();
	}
	
	public static ResourceProgram create(Uri uriVertexShader, Uri uriFragmentShader) {
		ResourceProgram resource;
		Resource resource2 = null;
		String name = uriVertexShader.getValue() + "<-->" + uriFragmentShader.getValue();
		if (name.isEmpty() == false && name != "---") {
			resource2 = getManager().localKeep(name);
		}
		if (resource2 != null) {
			if (resource2 instanceof ResourceProgram) {
				resource2.keep();
				return (ResourceProgram)resource2;
			}
			Log.critical("Request resource file : '" + name + "' With the wrong type (dynamic cast error)");
			return null;
		}
		resource = new ResourceProgram(uriVertexShader, uriFragmentShader);
		if (resource.resourceHasBeenCorectlyInit() == false) {
			Log.critical("resource Is not correctly init : ResourceProgram" );
			return null;
		}
		getManager().localAdd(resource);
		return resource;
	}
	
}
