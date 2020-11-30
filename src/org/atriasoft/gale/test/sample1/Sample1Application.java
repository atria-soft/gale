package org.atriasoft.gale.test.sample1;

import org.atriasoft.etk.Color;
import org.atriasoft.etk.Uri;
import org.atriasoft.etk.math.Matrix4f;
import org.atriasoft.etk.math.Vector2f;
import org.atriasoft.etk.math.Vector3f;
import org.atriasoft.gale.Application;
import org.atriasoft.gale.backend3d.OpenGL;
import org.atriasoft.gale.context.Context;
import org.atriasoft.gale.resource.ResourceProgram;
import org.atriasoft.gale.resource.ResourceVirtualArrayObject;
import org.atriasoft.gale.key.KeyKeyboard;
import org.atriasoft.gale.key.KeySpecial;
import org.atriasoft.gale.key.KeyStatus;
import org.atriasoft.gale.key.KeyType;

public class Sample1Application extends Application {
	private ResourceProgram oGLprogram;
	private int oGLMatrixTransformation;
	private int oGLMatrixProjection;
	private int oGLMatrixView;
	private float angle;
	private ResourceVirtualArrayObject verticesVBO;
	
	
	@Override
	public void onCreate(Context context) {
		this.canDraw = true;
		setSize(new Vector2f(800, 600));
		this.angle = 0.0f;
		this.oGLprogram = ResourceProgram.create(new Uri("DATA", "basic.vert"), new Uri("DATA", "basic.frag"));
		if (this.oGLprogram != null) {
			this.oGLMatrixTransformation = this.oGLprogram.getUniform("matrixTransformation");
			this.oGLMatrixProjection     = this.oGLprogram.getUniform("matrixProjection");
			this.oGLMatrixView           = this.oGLprogram.getUniform("matrixView");
		}
		
		float[] vertices = {
				-0.5f, -0.5f, -1.0f,
				0.0f, 0.5f, -1.0f,
				0.5f,-0.5f, -1.0f
		};
		float[] colors = {
				1.0f, 0.0f, 0.0f, 1.0f,
				0.0f, 1.0f, 0.0f, 1.0f,
				0.0f, 0.0f, 1.0f, 1.0f,
		};
		int[] indices = {
				0, 1, 2,
		};
		// this is the properties of the buffer requested : "r"/"w" + "-" + buffer type "f"=float "i"=integer
		this.verticesVBO = ResourceVirtualArrayObject.create(vertices, colors, indices);
		if (this.verticesVBO == null) {
			Log.error("can not instanciate VBO ...");
			return;
		}
		// TO facilitate some debugs we add a name of the VBO:
		this.verticesVBO.setName("[VBO] of basic SAMPLE");
		// update all the VBO elements ...
		this.verticesVBO.flush();
		Log.info("==> Init APPL (END)");
	}
	@Override
	public void onDraw(Context context) {
		this.angle += 0.01;
		//Log.info("==> appl Draw ...");
		Vector2f size = getSize();
		// set the basic openGL view port: (position drawed in the windows)
		OpenGL.setViewPort(new Vector2f(0,0), size);
		// Clear all the stacked matrix ...
		OpenGL.setBasicMatrix(Matrix4f.identity());
		// clear background
		Color bgColor = new Color(0.0f, 1.0f, 1.0f, 0.75f);
		OpenGL.clearColor(bgColor);
		// real clear request:
		OpenGL.clear(OpenGL.ClearFlag.clearFlag_colorBuffer);
		// create a local matrix environnement.
		OpenGL.push();
		
		Matrix4f tmpProjection = Matrix4f.createMatrixOrtho(-getAspectRatio(), getAspectRatio(), -1, 1, -50, 50);
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
		Matrix4f transforamtionMatrix = Matrix4f.createMatrixRotate(new Vector3f(0,0,1),this.angle);
		Matrix4f viewMatrix = OpenGL.getCameraMatrix();
		//Matrix4f tmpMatrix = projMatrix * camMatrix;

		this.verticesVBO.bindForRendering();
		this.oGLprogram.uniformMatrix(this.oGLMatrixView, viewMatrix);
		this.oGLprogram.uniformMatrix(this.oGLMatrixProjection, projectionMatrix);
		// Change the position for each element with the same pipeline you need to render ...
		this.oGLprogram.uniformMatrix(this.oGLMatrixTransformation, transforamtionMatrix);
		
		// Request the draw od the elements:
		this.verticesVBO.render(OpenGL.RenderMode.triangle);
		
		this.verticesVBO.unBindForRendering();
		this.oGLprogram.unUse();
		// Restore context of matrix
		OpenGL.pop();
		this.markDrawingIsNeeded();
	}
	@Override
	public void onPointer(KeySpecial special,
			KeyType type,
	               int pointerID,
	                Vector2f pos,
	               KeyStatus state) {
//		Log.info("input event: type=" + type);
//		Log.info("               id=" + pointerID);
//		Log.info("              pos=" + pos);
//		Log.info("            state=" + state);
	}
	@Override
	public void onKeyboard( KeySpecial special,
	                KeyKeyboard type,
	                Character value,
	                KeyStatus state) {
		Log.info("Keyboard event: special=" + special);
		Log.info("                   type=" + type);
		Log.info("                  value='" + value + "'");
		Log.info("                  state=" + state);
	}
}
