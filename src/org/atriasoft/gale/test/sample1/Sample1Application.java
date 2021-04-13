package org.atriasoft.gale.test.sample1;

import org.atriasoft.etk.Color;
import org.atriasoft.etk.Uri;
import org.atriasoft.etk.math.Matrix4f;
import org.atriasoft.etk.math.Vector2f;
import org.atriasoft.etk.math.Vector3f;
import org.atriasoft.gale.GaleApplication;
import org.atriasoft.gale.backend3d.OpenGL;
import org.atriasoft.gale.context.Context;
import org.atriasoft.gale.key.KeyKeyboard;
import org.atriasoft.gale.key.KeySpecial;
import org.atriasoft.gale.key.KeyStatus;
import org.atriasoft.gale.key.KeyType;
import org.atriasoft.gale.resource.ResourceProgram;
import org.atriasoft.gale.resource.ResourceVirtualArrayObject;

public class Sample1Application extends GaleApplication {
	//float[] vertices = { 0.2f, 0.1f, 0.0f, 0.3f, 0.4f, 0.0f, 0.1f, 0.4f, 0.0f };
	private static final float[] VERTICES = { -0.5f, -0.5f, -1.0f, 0.0f, 0.5f, -1.0f, 0.5f, -0.5f, -1.0f };
	private static final float[] COLORS = { 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, };
	private static final int[] INDICES = { 0, 1, 2 };
	
	private static final boolean TEST_STATIC_MODE = false;
	
	private ResourceProgram oGLprogram;
	private int oGLMatrixTransformation;
	private int oGLMatrixProjection;
	private int oGLMatrixView;
	
	private float angle;
	private ResourceVirtualArrayObject verticesVBO;
	
	@Override
	public void onCreate(final Context context) {
		//setSize(new Vector2f(800, 600));
		this.angle = 0.0f;
		this.oGLprogram = ResourceProgram.create(new Uri("DATA", "basic.vert"), new Uri("DATA", "basic.frag"));
		if (this.oGLprogram != null) {
			this.oGLMatrixTransformation = this.oGLprogram.getUniform("in_matrixTransformation");
			this.oGLMatrixProjection = this.oGLprogram.getUniform("in_matrixProjection");
			this.oGLMatrixView = this.oGLprogram.getUniform("in_matrixView");
		}
		
		// this is the properties of the buffer requested : "r"/"w" + "-" + buffer type "f"=float "i"=integer
		if (TEST_STATIC_MODE) {
			this.verticesVBO = ResourceVirtualArrayObject.create(VERTICES, COLORS, INDICES);
		} else {
			this.verticesVBO = ResourceVirtualArrayObject.createDynamic();
		}
		// TO facilitate some debugs we add a name of the VBO:
		this.verticesVBO.setName("[VBO] of basic SAMPLE");
		// update all the VBO elements ...
		this.verticesVBO.flush();
		Log.info("==> Init APPL (END)");
	}
	
	@Override
	public void onDraw(final Context context) {
		this.angle += 0.01;
		//Log.info("==> appl Draw ...");
		Vector2f size = getSize();
		// set the basic openGL view port: (position drawed in the windows)
		OpenGL.setViewPort(Vector2f.ZERO, size);
		// Clear all the stacked matrix ...
		OpenGL.setBasicMatrix(Matrix4f.IDENTITY);
		// clear background
		Color bgColor = Color.CYAN;
		OpenGL.clearColor(bgColor);
		// real clear request:
		OpenGL.clear(OpenGL.ClearFlag.clearFlag_colorBuffer);
		// create a local matrix environment.
		OpenGL.push();
		
		Matrix4f tmpProjection = Matrix4f.createMatrixOrtho(-getAspectRatio(), getAspectRatio(), -1, 1, -50, 50);
		//Matrix4f tmpProjection = Matrix4f.IDENTITY;
		// set internal matrix system:
		OpenGL.setMatrix(tmpProjection);
		if (this.oGLprogram == null) {
			Log.info("No shader ...");
			return;
		}
		//EWOL_DEBUG("    display " + this.coord.size() + " elements" );
		this.oGLprogram.use();
		
		// set Matrix: translation/positionMatrix
		Matrix4f projectionMatrix = tmpProjection; //OpenGL.getMatrix();
		Matrix4f transforamtionMatrix = Matrix4f.createMatrixRotate(new Vector3f(0, 0, 1), this.angle);
		Matrix4f viewMatrix = OpenGL.getCameraMatrix();
		//Matrix4f tmpMatrix = projMatrix * camMatrix;
		
		this.verticesVBO.bindForRendering();
		this.oGLprogram.uniformMatrix(this.oGLMatrixView, viewMatrix);
		this.oGLprogram.uniformMatrix(this.oGLMatrixProjection, projectionMatrix);
		// Change the position for each element with the same pipeline you need to render ...
		this.oGLprogram.uniformMatrix(this.oGLMatrixTransformation, transforamtionMatrix);
		
		// Request the draw of the elements:
		if (TEST_STATIC_MODE) {
			this.verticesVBO.render(OpenGL.RenderMode.triangle);
		} else {
			this.verticesVBO.renderArrays(OpenGL.RenderMode.triangle);
		}
		this.verticesVBO.unBindForRendering();
		this.oGLprogram.unUse();
		// Restore context of matrix
		OpenGL.pop();
		// mark to redraw the screen ==> demo only....
		markDrawingIsNeeded();
		
		if (!TEST_STATIC_MODE) {
			this.verticesVBO.clear();
			this.verticesVBO.setPosition(VERTICES);
			this.verticesVBO.setColors(COLORS);
			this.verticesVBO.setVertexCount(3);
			this.verticesVBO.flush();
		}
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
