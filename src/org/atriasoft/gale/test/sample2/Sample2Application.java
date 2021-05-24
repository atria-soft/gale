package org.atriasoft.gale.test.sample2;

import org.atriasoft.etk.Color;
import org.atriasoft.etk.Uri;
import org.atriasoft.etk.math.Matrix4f;
import org.atriasoft.etk.math.Vector2f;
import org.atriasoft.etk.math.Vector3f;
import org.atriasoft.gale.GaleApplication;
import org.atriasoft.gale.backend3d.OpenGL;
import org.atriasoft.gale.context.GaleContext;
import org.atriasoft.gale.key.KeyKeyboard;
import org.atriasoft.gale.key.KeySpecial;
import org.atriasoft.gale.key.KeyStatus;
import org.atriasoft.gale.key.KeyType;
import org.atriasoft.gale.resource.ResourceProgram;
import org.atriasoft.gale.resource.ResourceTexture;
import org.atriasoft.gale.resource.ResourceVirtualArrayObject;

public class Sample2Application extends GaleApplication {
	private ResourceProgram oGLprogram;
	private int oGLMatrixTransformation;
	private int oGLMatrixProjection;
	private int oGLMatrixView;
	private float angleX = 0;
	private float angleY = 0;
	private float angleZ = 0;
	private ResourceVirtualArrayObject verticesVBO;
	private ResourceTexture texture;
	
	@Override
	public void onCreate(final GaleContext context) {
		setSize(new Vector2f(800, 600));
		this.oGLprogram = ResourceProgram.create(new Uri("DATA", "basic.vert"), new Uri("DATA", "basic.frag"));
		if (this.oGLprogram != null) {
			this.oGLMatrixTransformation = this.oGLprogram.getUniform("in_matrixTransformation");
			this.oGLMatrixProjection = this.oGLprogram.getUniform("in_matrixProjection");
			this.oGLMatrixView = this.oGLprogram.getUniform("in_matrixView");
		}
		//@formatter:off
		float[] vertices = {			
				-0.5f,0.5f,-0.5f,	
				-0.5f,-0.5f,-0.5f,	
				0.5f,-0.5f,-0.5f,	
				0.5f,0.5f,-0.5f,		
				
				-0.5f,0.5f,0.5f,	
				-0.5f,-0.5f,0.5f,	
				0.5f,-0.5f,0.5f,	
				0.5f,0.5f,0.5f,
				
				0.5f,0.5f,-0.5f,	
				0.5f,-0.5f,-0.5f,	
				0.5f,-0.5f,0.5f,	
				0.5f,0.5f,0.5f,
				
				-0.5f,0.5f,-0.5f,	
				-0.5f,-0.5f,-0.5f,	
				-0.5f,-0.5f,0.5f,	
				-0.5f,0.5f,0.5f,
				
				-0.5f,0.5f,0.5f,
				-0.5f,0.5f,-0.5f,
				0.5f,0.5f,-0.5f,
				0.5f,0.5f,0.5f,
				
				-0.5f,-0.5f,0.5f,
				-0.5f,-0.5f,-0.5f,
				0.5f,-0.5f,-0.5f,
				0.5f,-0.5f,0.5f
				
		};
							 		
		float[] textureCoords = {
				0,0,				0,1,				1,1,				1,0,			
				0,0,				0,1,				1,1,				1,0,			
				0,0,				0,1,				1,1,				1,0,
				0,0,				0,1,				1,1,				1,0,
				0,0,				0,1,				1,1,				1,0,
				0,0,				0,1,				1,1,				1,0
		};
							 		
		int[] indices = {
				1,0,3,					1,3,2,	
				4,5,7,					7,5,6, 
				9,8,11,					9,11,10,
				12,13,15,				15,13,14,	
				17,16,19,				17,19,18,
				20,21,23,				23,21,22

		};
		//@formatter:on
		
		// this is the properties of the buffer requested : "r"/"w" + "-" + buffer type "f"=float "i"=integer
		this.verticesVBO = ResourceVirtualArrayObject.create(vertices, textureCoords, null, indices);
		// TO facilitate some debugs we add a name of the VBO:
		this.verticesVBO.setName("[VBO] of basic SAMPLE");
		// update all the VBO elements ...
		this.verticesVBO.flush();
		
		this.texture = ResourceTexture.createFromPng(new Uri("DATA", "tree_sample.png"));
		if (this.texture == null) {
			Log.error("can not instanciate Texture ...");
			return;
		}
		Log.info("==> Init APPL (END)");
	}
	
	@Override
	public void onDraw(final GaleContext context) {
		this.angleX += 0.001;
		this.angleY += 0.005;
		this.angleZ += 0.01;
		//Log.info("==> appl Draw ...");
		Vector2f size = getSize();
		//Log.info("==> Windows size = " + size);
		// set the basic openGL view port: (position drawed in the windows)
		OpenGL.setViewPort(new Vector2f(0, 0), size);
		// Clear all the stacked matrix ...
		OpenGL.setBasicMatrix(Matrix4f.IDENTITY);
		// clear background
		Color bgColor = new Color(0.0f, 1.0f, 1.0f, 0.75f);
		OpenGL.enable(OpenGL.Flag.flag_depthTest);
		OpenGL.clearColor(bgColor);
		// real clear request:
		OpenGL.clear(OpenGL.ClearFlag.clearFlag_colorBuffer);
		OpenGL.clear(OpenGL.ClearFlag.clearFlag_depthBuffer);
		// create a local matrix environnement.
		OpenGL.push();
		
		//Matrix4f tmpProjection = Matrix4f.createMatrixOrtho(-getAspectRatio(), getAspectRatio(), -1, 1, -50, 50);
		Matrix4f tmpProjection = Matrix4f.createMatrixPerspective(1.30f, getAspectRatio(), 1, 50);
		// set internal matrix system:
		OpenGL.setMatrix(tmpProjection);
		if (this.oGLprogram == null) {
			Log.info("No shader ...");
			return;
		}
		//EWOL_DEBUG("    display " + this.coord.size() + " elements" );
		this.oGLprogram.use();
		
		// set Matrix : translation/positionMatrix
		Matrix4f projectionMatrix = tmpProjection; //OpenGL.getMatrix();
		Matrix4f transforamtionMatrix = Matrix4f.IDENTITY;
		transforamtionMatrix = transforamtionMatrix.multiply(Matrix4f.createMatrixTranslate(new Vector3f(0, 0, -1)));
		transforamtionMatrix = transforamtionMatrix.multiply(Matrix4f.createMatrixRotate(new Vector3f(1, 0, 0), this.angleX));
		transforamtionMatrix = transforamtionMatrix.multiply(Matrix4f.createMatrixRotate(new Vector3f(0, 1, 0), this.angleY));
		transforamtionMatrix = transforamtionMatrix.multiply(Matrix4f.createMatrixRotate(new Vector3f(0, 0, 1), this.angleZ));
		Matrix4f viewMatrix = OpenGL.getCameraMatrix().multiply(Matrix4f.createMatrixTranslate(new Vector3f(0, 0, -2)));
		//Matrix4f tmpMatrix = projMatrix * camMatrix;
		this.verticesVBO.bindForRendering();
		this.oGLprogram.uniformMatrix(this.oGLMatrixView, viewMatrix);
		this.oGLprogram.uniformMatrix(this.oGLMatrixProjection, projectionMatrix);
		// Change the position for each element with the same pipeline you need to render ...
		this.oGLprogram.uniformMatrix(this.oGLMatrixTransformation, transforamtionMatrix);
		this.texture.bindForRendering(0);
		// update of flags is done asyncronously ==> need update befor drawing...
		OpenGL.updateAllFlags();
		// Request the draw od the elements:
		this.verticesVBO.render(OpenGL.RenderMode.TRIANGLE);
		
		this.verticesVBO.unBindForRendering();
		this.texture.unBindForRendering();
		this.oGLprogram.unUse();
		// Restore context of matrix
		OpenGL.pop();
		markDrawingIsNeeded();
	}
	
	@Override
	public void onKeyboard(final KeySpecial special, final KeyKeyboard type, final Character value, final KeyStatus state) {
		Log.info("Keyboard event: special=" + special);
		Log.info("                   type=" + type);
		Log.info("                  value='" + value + "'");
		Log.info("                  state=" + state);
	}
	
	@Override
	public void onPointer(final KeySpecial special, final KeyType type, final int pointerID, final Vector2f pos, final KeyStatus state) {
		//		Log.info("input event: type=" + type);
		//		Log.info("               id=" + pointerID);
		//		Log.info("              pos=" + pos);
		//		Log.info("            state=" + state);
	}
}
