package org.atriasoft.gale.resource;

import java.nio.FloatBuffer;
import java.util.ArrayList;
import java.util.List;

import org.atriasoft.etk.Color;
import org.atriasoft.etk.Uri;
import org.atriasoft.etk.math.Matrix4f;
import org.atriasoft.etk.math.Vector3f;
import org.atriasoft.gale.internal.Log;
import org.atriasoft.gale.backend3d.OpenGL;
import org.lwjgl.BufferUtils;

public class ResourceColored3DObject extends Resource {
	private ResourceProgram program;
	private int oGLPosition;
	private int oGLColor;
	private int oGLMatrixTransformation;
	private int oGLMatrixProjection;
	private int oGLMatrixView;
	protected ResourceColored3DObject() {
		super();
		addResourceType("ResourceColored3DObject");
		// get the shader resource :
		this.oGLPosition = 0;
		this.program = ResourceProgram.create(new Uri("DATA_EGE", "simple3D.vert"), new Uri("DATA_EGE", "simple3D.frag"));
		if (this.program != null) {
			this.oGLMatrixTransformation = this.program.getUniform("in_matrixTransformation");
			this.oGLMatrixProjection     = this.program.getUniform("in_matrixProjection");
			this.oGLMatrixView           = this.program.getUniform("in_matrixView");
			this.oGLPosition             = this.program.getAttribute("in_position");
			this.oGLColor                = this.program.getUniform("in_colors");
		}
	}
	public void draw(List<Vector3f> vertices,
			Color color,
			boolean updateDepthBuffer,
			boolean depthtest) {
		if (vertices.size() <= 0) {
			return;
		}
		if (this.program == null) {
			Log.error("No shader ...");
			return;
		}
		if (true == depthtest) {
			OpenGL.enable(OpenGL.Flag.flag_depthTest);
			if (false == updateDepthBuffer) {
				OpenGL.setDeathMask(false);
			}
		}
		//Log.debug("    display " << this.coord.size() << " elements" );
		this.program.use();
		Matrix4f projectionMatrix = OpenGL.getMatrix();
		Matrix4f viewMatrix = OpenGL.getCameraMatrix();
		Matrix4f transformationMatrix = Matrix4f.identity();
		this.program.uniformMatrix(this.oGLMatrixView, viewMatrix);
		this.program.uniformMatrix(this.oGLMatrixProjection, projectionMatrix);
		this.program.uniformMatrix(this.oGLMatrixTransformation, transformationMatrix);
		// color :
		this.program.uniformColor(this.oGLColor, color);
		// position :
		FloatBuffer buffer = storeDataInFloatBuffer(convertInFloat(vertices));
		this.program.sendAttribute(this.oGLPosition, 3, buffer, 3);
		if (color.a < 1.0f) {
			OpenGL.enable(OpenGL.Flag.flag_blend);
		}
		// Request the draw of the elements: 
		OpenGL.drawArrays(OpenGL.RenderMode.triangle, 0, vertices.size());
		if (color.a < 1.0f) {
			OpenGL.disable(OpenGL.Flag.flag_blend);
		}
		this.program.unUse();
		// Request the draw od the elements: 
		//glDrawArrays(oGLLINES, 0, vertices.size());
		//this.oGLprogram.UnUse();
		if (true == depthtest) {
			if (false == updateDepthBuffer) {
				OpenGL.setDeathMask(true);
			}
			OpenGL.disable(OpenGL.Flag.flag_depthTest);
		}
	}
	private float[] convertInFloat(List<Vector3f> data) {
		float[] out = new float[data.size()*3];
		for (int iii=0; iii<data.size(); iii++) {
			out[iii*3] = data.get(iii).x;
			out[iii*3+1] = data.get(iii).y;
			out[iii*3+2] = data.get(iii).z;
		}
		return out;
	}
	private FloatBuffer storeDataInFloatBuffer(float[] data) {
		FloatBuffer buffer = BufferUtils.createFloatBuffer(data.length);
		buffer.put(data);
		buffer.flip();
		return buffer;
	}
	public void draw(List<Vector3f> vertices,
			Color color,
			Matrix4f transformationMatrix,
			boolean updateDepthBuffer,
			boolean depthtest) {
		if (vertices.size() <= 0) {
			return;
		}
		if (this.program == null) {
			Log.error("No shader ...");
			return;
		}
		if (true == depthtest) {
			OpenGL.enable(OpenGL.Flag.flag_depthTest);
			if (false == updateDepthBuffer) {
				OpenGL.setDeathMask(false);
			}
		}
		//Log.debug("    display " << this.coord.size() << " elements" );
		this.program.use();
		// set Matrix: translation/positionMatrix
		Matrix4f projectionMatrix = OpenGL.getMatrix();
		Matrix4f viewMatrix = OpenGL.getCameraMatrix();
		this.program.uniformMatrix(this.oGLMatrixView, viewMatrix);
		this.program.uniformMatrix(this.oGLMatrixProjection, projectionMatrix);
		this.program.uniformMatrix(this.oGLMatrixTransformation, transformationMatrix);
		// position :
		FloatBuffer buffer = storeDataInFloatBuffer(convertInFloat(vertices));
		this.program.sendAttribute(this.oGLPosition, 3, buffer, 3);
		// color :
		//Log.info("color= " + color + " " + this.oGLPosition);
		this.program.uniformColor(this.oGLColor, color);
		if (color.a < 1.0f) {
			OpenGL.enable(OpenGL.Flag.flag_blend);
		}
		// Request the draw of the elements: 
		OpenGL.drawArrays(OpenGL.RenderMode.triangle, 0, vertices.size());
		if (color.a < 1.0f) {
			OpenGL.disable(OpenGL.Flag.flag_blend);
		}
		this.program.unUse();
		if (true == depthtest) {
			if (false == updateDepthBuffer) {
				OpenGL.setDeathMask(true);
			}
			OpenGL.disable(OpenGL.Flag.flag_depthTest);
		}
	}
	public void drawLine(List<Vector3f> vertices,
			Color color,
			Matrix4f transformationMatrix,
			boolean updateDepthBuffer,
			boolean depthtest) {
		if (vertices.size() <= 0) {
			return;
		}
		if (this.program == null) {
			Log.error("No shader ...");
			return;
		}
		if (true == depthtest) {
			OpenGL.enable(OpenGL.Flag.flag_depthTest);
			if (false == updateDepthBuffer) {
				OpenGL.setDeathMask(false);
			}
		}
		//Log.debug("    display " << this.coord.size() << " elements" );
		this.program.use();
		// set Matrix: translation/positionMatrix
		Matrix4f projectionMatrix = OpenGL.getMatrix();
		Matrix4f viewMatrix = OpenGL.getCameraMatrix();
		this.program.uniformMatrix(this.oGLMatrixView, viewMatrix);
		this.program.uniformMatrix(this.oGLMatrixProjection, projectionMatrix);
		this.program.uniformMatrix(this.oGLMatrixTransformation, transformationMatrix);
		// position :
		FloatBuffer buffer = storeDataInFloatBuffer(convertInFloat(vertices));
		this.program.sendAttribute(this.oGLPosition, 3, buffer, 3);
		// color :
		this.program.uniformColor(this.oGLColor, color);
		if (color.a < 1.0f) {
			OpenGL.enable(OpenGL.Flag.flag_blend);
		}
		// Request the draw od the elements: 
		OpenGL.drawArrays(OpenGL.RenderMode.line, 0, vertices.size());
		if (color.a < 1.0f) {
			OpenGL.disable(OpenGL.Flag.flag_blend);
		}
		this.program.unUse();
		if (true == depthtest) {
			if (false == updateDepthBuffer) {
				OpenGL.setDeathMask(true);
			}
			OpenGL.disable(OpenGL.Flag.flag_depthTest);
		}
	}
	public void drawCubeLine(Vector3f min,
			Vector3f max,
			Color color,
			Matrix4f transformationMatrix,
			boolean updateDepthBuffer,
			boolean depthtest) {
		List<Vector3f> vertices = new ArrayList<Vector3f>();
		vertices.add(new Vector3f(min.x, min.y,min.z));
		vertices.add(new Vector3f(max.x, min.y,min.z));
		
		vertices.add(new Vector3f(max.x, min.y,min.z));
		vertices.add(new Vector3f(max.x, min.y,max.z));
		
		vertices.add(new Vector3f(max.x, min.y,max.z));
		vertices.add(new Vector3f(min.x, min.y,max.z));
		
		vertices.add(new Vector3f(min.x, min.y,max.z));
		vertices.add(new Vector3f(min.x, min.y,min.z));
		
		
		vertices.add(new Vector3f(min.x, max.y,min.z));
		vertices.add(new Vector3f(max.x, max.y,min.z));
		
		vertices.add(new Vector3f(max.x, max.y,min.z));
		vertices.add(new Vector3f(max.x, max.y,max.z));
		
		vertices.add(new Vector3f(max.x, max.y,max.z));
		vertices.add(new Vector3f(min.x, max.y,max.z));
		
		vertices.add(new Vector3f(min.x, max.y,max.z));
		vertices.add(new Vector3f(min.x, max.y,min.z));
		
		
		vertices.add(new Vector3f(min.x, min.y,min.z));
		vertices.add(new Vector3f(min.x, max.y,min.z));
		
		vertices.add(new Vector3f(max.x, min.y,min.z));
		vertices.add(new Vector3f(max.x, max.y,min.z));
		
		vertices.add(new Vector3f(max.x, min.y,max.z));
		vertices.add(new Vector3f(max.x, max.y,max.z));
		
		vertices.add(new Vector3f(min.x, min.y,max.z));
		vertices.add(new Vector3f(min.x, max.y,max.z));
		drawLine(vertices, color, transformationMatrix, updateDepthBuffer, depthtest);
	}
	public void drawSquare(Vector3f size,
			Matrix4f transformationMatrix,
			Color tmpColor) {
		List<Vector3f> tmpVertices = new ArrayList<Vector3f>();
		int[] indices = { 0,1,2,	3,2,1,	4,0,6,
		                           6,0,2,	5,1,4,	4,1,0,
		                           7,3,1,	7,1,5,	5,4,7,
		                           7,4,6,	7,2,3,	7,6,2};
		Vector3f[] vertices={ new Vector3f(size.x,size.y,size.z),
				new Vector3f(-size.x,size.y,size.z),
				new Vector3f(size.x,-size.y,size.z),
				new Vector3f(-size.x,-size.y,size.z),
				new Vector3f(size.x,size.y,-size.z),
				new Vector3f(-size.x,size.y,-size.z),
				new Vector3f(size.x,-size.y,-size.z),
				new Vector3f(-size.x,-size.y,-size.z)};
		tmpVertices.clear();
		for (int iii=0 ; iii<36 ; iii+=3) {
			// normal calculation :
			//btVector3 normal = (vertices[indices[iii+2]]-vertices[indices[iii]]).cross(vertices[indices[iii+1]]-vertices[indices[iii]]);
			//normal.normalize ();
			tmpVertices.add(vertices[indices[iii]]);
			tmpVertices.add(vertices[indices[iii+1]]);
			tmpVertices.add(vertices[indices[iii+2]]);
		}
		draw(tmpVertices, tmpColor, transformationMatrix, true, true);
	}
	public void drawSphere(float radius,
	                int lats,
	                int longs,
	                Matrix4f transformationMatrix,
	                Color tmpColor) {
		List<Vector3f> tmpVertices = new ArrayList<Vector3f>();
		for(int iii=0; iii<=lats; ++iii) {
			float lat0 = (float)Math.PI * (-0.5f + (float)(iii - 1) / lats);
			float z0  = radius*(float)Math.sin(lat0);
			float zr0 = radius*(float)Math.cos(lat0);
			
			float lat1 = (float)Math.PI * (-0.5f + (float)(iii) / lats);
			float z1 = radius*(float)Math.sin(lat1);
			float zr1 = radius*(float)Math.cos(lat1);
			
			for(int jjj=0; jjj<longs; ++jjj) {
				float lng = 2.0f * (float)Math.PI * (float)(jjj - 1) / longs;
				float x = (float)Math.cos(lng);
				float y = (float)Math.sin(lng);
				Vector3f v1 = new Vector3f(x * zr1, y * zr1, z1);
				Vector3f v4 = new Vector3f(x * zr0, y * zr0, z0);
				
				lng = 2 * (float)Math.PI * (float)(jjj) / longs;
				x = (float)Math.cos(lng);
				y = (float)Math.sin(lng);
				Vector3f v2 = new Vector3f(x * zr1, y * zr1, z1);
				Vector3f v3 = new Vector3f(x * zr0, y * zr0, z0);
				
				tmpVertices.add(v1);
				tmpVertices.add(v2);
				tmpVertices.add(v3);
				
				tmpVertices.add(v1);
				tmpVertices.add(v3);
				tmpVertices.add(v4);
			}
		}
		draw(tmpVertices, tmpColor, transformationMatrix, true, true);
	}
	public void drawCylinder(float radius,
	                  float size,
	                  int lats,
	                  int longs,
	                  Matrix4f transformationMatrix,
	                  Color tmpColor) {
		List<Vector3f> tmpVertices = new ArrayList<Vector3f>();
		// center to border (TOP)
		
		// center to border (TOP)
		for(int jjj=0; jjj<longs; ++jjj) {
			float lng = 2.0f * (float)Math.PI * (float)(jjj - 1) / longs;
			
			float z = size*0.5f;
			Vector3f v1 = new Vector3f(0.0f, 0.0f, z);
			
			float x = (float) Math.cos(lng)*radius;
			float y = (float) Math.sin(lng)*radius;
			Vector3f v2 = new Vector3f(x, y, z);
			
			lng = 2.0f * (float)Math.PI * (float)(jjj) / longs;
			x = (float) Math.cos(lng)*radius;
			y = (float)Math.sin(lng)*radius;
			Vector3f v3 = new Vector3f(x, y, z);
			tmpVertices.add(v1);
			tmpVertices.add(v3);
			tmpVertices.add(v2);
		}
		// Cylinder
		for(int jjj=0; jjj<longs; ++jjj) {
			float lng = 2.0f * (float)Math.PI * (float)(jjj - 1) / longs;
			
			float z = size*0.5f;
			
			float x = (float) Math.cos(lng)*radius;
			float y = (float) Math.sin(lng)*radius;
			Vector3f v2  = new Vector3f(x, y, z);
			Vector3f v2b = new Vector3f(x, y, -z);
			
			lng = 2.0f * (float)Math.PI * (float)(jjj) / longs;
			x = (float) Math.cos(lng)*radius;
			y = (float) Math.sin(lng)*radius;
			Vector3f v3  = new Vector3f(x, y, z);
			Vector3f v3b = new Vector3f(x, y, -z);
			
			tmpVertices.add(v2);
			tmpVertices.add(v3);
			tmpVertices.add(v3b);
			
			tmpVertices.add(v2);
			tmpVertices.add(v3b);
			tmpVertices.add(v2b);
		}
		// center to border (BUTTOM)
		for(int jjj=0; jjj<longs; ++jjj) {
			float lng = 2.0f * (float)Math.PI * (float)(jjj - 1) / longs;
			
			float z = size*-0.5f;
			Vector3f v1 = new Vector3f(0.0f, 0.0f, z);
			
			float x = (float)Math.cos(lng)*radius;
			float y = (float)Math.sin(lng)*radius;
			Vector3f v2 = new Vector3f(x, y, z);
			
			lng = 2.0f * (float)Math.PI * (float)(jjj) / longs;
			x = (float)Math.cos(lng)*radius;
			y = (float)Math.sin(lng)*radius;
			Vector3f v3 = new Vector3f(x, y, z);
			tmpVertices.add(v1);
			tmpVertices.add(v2);
			tmpVertices.add(v3);
		}
		draw(tmpVertices, tmpColor, transformationMatrix, true, true);
	}
	public void drawCapsule(float radius,
	                 float size,
	                 int lats,
	                 int longs,
	                 Matrix4f transformationMatrix,
	                 Color tmpColor) {
		List<Vector3f> tmpVertices = new ArrayList<Vector3f>();
		lats = (int)(lats / 2)*2;
		
		// center to border (TOP)
		float offset = size*0.5f;
		for(int iii=lats/2+1; iii<=lats; ++iii) {
			float lat0 = (float)Math.PI * (-0.5f + (float)(iii - 1) / lats);
			float z0  = radius*(float)Math.sin(lat0);
			float zr0 = radius*(float)Math.cos(lat0);
			
			float lat1 = (float)Math.PI * (-0.5f + (float)(iii) / lats);
			float z1 = radius*(float)Math.sin(lat1);
			float zr1 = radius*(float)Math.cos(lat1);
			
			for(int jjj=0; jjj<longs; ++jjj) {
				float lng = 2.0f * (float)Math.PI * (float)(jjj - 1) / longs;
				float x = (float)Math.cos(lng);
				float y = (float)Math.sin(lng);
				Vector3f v1 = new Vector3f(x * zr1, y * zr1, z1+offset);
				Vector3f v4 = new Vector3f(x * zr0, y * zr0, z0+offset);
				
				lng = 2 * (float)Math.PI * (float)(jjj) / longs;
				x = (float)Math.cos(lng);
				y = (float)Math.sin(lng);
				Vector3f v2 = new Vector3f(x * zr1, y * zr1, z1+offset);
				Vector3f v3 = new Vector3f(x * zr0, y * zr0, z0+offset);
				tmpVertices.add(v1);
				tmpVertices.add(v2);
				tmpVertices.add(v3);
				
				tmpVertices.add(v1);
				tmpVertices.add(v3);
				tmpVertices.add(v4);
			}
		}
		// Cylinder
		for(int jjj=0; jjj<longs; ++jjj) {
			float lng = 2.0f * (float)Math.PI * (float)(jjj - 1) / longs;
			
			float z = size*0.5f;
			
			float x = (float)Math.cos(lng)*radius;
			float y = (float)Math.sin(lng)*radius;
			Vector3f v2  = new Vector3f(x, y, z);
			Vector3f v2b = new Vector3f(x, y, -z);
			
			lng = 2.0f * (float)Math.PI * (float)(jjj) / longs;
			x = (float)Math.cos(lng)*radius;
			y = (float)Math.sin(lng)*radius;
			Vector3f v3  = new Vector3f(x, y, z);
			Vector3f v3b = new Vector3f(x, y, -z);
			
			tmpVertices.add(v2);
			tmpVertices.add(v3);
			tmpVertices.add(v3b);
			
			tmpVertices.add(v2);
			tmpVertices.add(v3b);
			tmpVertices.add(v2b);
		}
		// center to border (BUTTOM)
		offset = -size*0.5f;
		for(int iii=0; iii<=lats/2; ++iii) {
			float lat0 = (float)Math.PI * (-0.5f + (float)(iii - 1) / lats);
			float z0  = radius*(float)Math.sin(lat0);
			float zr0 = radius*(float)Math.cos(lat0);
			
			float lat1 = (float)Math.PI * (-0.5f + (float)(iii) / lats);
			float z1 = radius*(float)Math.sin(lat1);
			float zr1 = radius*(float)Math.cos(lat1);
			
			for(int jjj=0; jjj<longs; ++jjj) {
				float lng = 2.0f * (float)Math.PI * (float)(jjj - 1) / longs;
				float x = (float)Math.cos(lng);
				float y = (float)Math.sin(lng);
				Vector3f v1 = new Vector3f(x * zr1, y * zr1, z1+offset);
				Vector3f v4 = new Vector3f(x * zr0, y * zr0, z0+offset);
				
				lng = 2 * (float)Math.PI * (float)(jjj) / longs;
				x = (float)Math.cos(lng);
				y = (float)Math.sin(lng);
				Vector3f v2 = new Vector3f(x * zr1, y * zr1, z1+offset);
				Vector3f v3 = new Vector3f(x * zr0, y * zr0, z0+offset);
				tmpVertices.add(v1);
				tmpVertices.add(v2);
				tmpVertices.add(v3);
				
				tmpVertices.add(v1);
				tmpVertices.add(v3);
				tmpVertices.add(v4);
			}
		}
		draw(tmpVertices, tmpColor, transformationMatrix, true, true);
	}
	public void drawCone(float radius,
	              float size,
	              int lats,
	              int longs,
	              Matrix4f transformationMatrix,
	              Color tmpColor) {
		List<Vector3f> tmpVertices = new ArrayList<Vector3f>();
		// center to border (TOP)
		for(int jjj=0; jjj<longs; ++jjj) {
			float lng = 2.0f * (float)Math.PI * (float)(jjj - 1) / longs;
			Vector3f v1 = new Vector3f(0.0f, 0.0f, -size/2);
			
			float x = (float)Math.cos(lng)*radius;
			float y = (float)Math.sin(lng)*radius;
			Vector3f v2 = new Vector3f(x, y, size/2);
			
			lng = 2.0f * (float)Math.PI * (float)(jjj) / longs;
			x = (float)Math.cos(lng)*radius;
			y = (float)Math.sin(lng)*radius;
			Vector3f v3 = new Vector3f(x, y, size/2);
			tmpVertices.add(v1);
			tmpVertices.add(v3);
			tmpVertices.add(v2);
		}
		// center to border (BUTTOM)
		for(int jjj=0; jjj<longs; ++jjj) {
			float lng = 2.0f * (float)Math.PI * (float)(jjj - 1) / longs;
			
			Vector3f v1 = new Vector3f(0.0f, 0.0f, size/2);
			
			float x = (float)Math.cos(lng)*radius;
			float y = (float)Math.sin(lng)*radius;
			Vector3f v2 = new Vector3f(x, y, size/2);
			
			lng = 2.0f * (float)Math.PI * (float)(jjj) / longs;
			x = (float)Math.cos(lng)*radius;
			y = (float)Math.sin(lng)*radius;
			Vector3f v3 = new Vector3f(x, y, size/2);
			tmpVertices.add(v1);
			tmpVertices.add(v2);
			tmpVertices.add(v3);
		}
		draw(tmpVertices, tmpColor, transformationMatrix, true, true);
	}
	public void drawTriangles(List<Vector3f> vertex,
	                   List<Integer> indice,
	                   Matrix4f transformationMatrix,
	                   Color tmpColor,
	                   Vector3f offset) {// = new Vector3f(0,0,0.1f)
		List<Vector3f> tmpVertices = new ArrayList<Vector3f>();
		for (int iii=0; iii<indice.size()/3; ++iii) {
			tmpVertices.add(vertex.get(indice.get(iii*3 + 0)).addNew(offset));
			tmpVertices.add(vertex.get(indice.get(iii*3 + 1)).addNew(offset));
			tmpVertices.add(vertex.get(indice.get(iii*3 + 2)).addNew(offset));
			//Log.info("  indices " << indice[iii*3 + 0] << " " << indice[iii*3 + 1] << " " << indice[iii*3 + 2]);
			//Log.info(" triangle " << vertex[indice[iii*3 + 0]] << " " << vertex[indice[iii*3 + 1]] << " " << vertex[indice[iii*3 + 2]]);
		}
		//Log.info("display " << tmpVertices.size() << " vertices form " << indice.size());
		draw(tmpVertices, tmpColor, transformationMatrix, true, true);
	}

	@Override
	public void cleanUp() {
		// TODO Auto-generated method stub
	
	}
	public static ResourceColored3DObject create() {
		ResourceColored3DObject resource = new ResourceColored3DObject();
		if (resource.resourceHasBeenCorectlyInit() == false) {
			Log.critical("resource Is not correctly init: ResourceColored3DObject");
		}
		getManager().localAdd(resource);
		return resource;
	}
}
