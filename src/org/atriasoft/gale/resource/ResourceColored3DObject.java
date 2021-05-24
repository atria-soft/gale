package org.atriasoft.gale.resource;

import java.nio.FloatBuffer;
import java.util.ArrayList;
import java.util.List;

import org.atriasoft.etk.Color;
import org.atriasoft.etk.Uri;
import org.atriasoft.etk.math.Matrix4f;
import org.atriasoft.etk.math.Vector3f;
import org.atriasoft.gale.backend3d.OpenGL;
import org.atriasoft.gale.internal.Log;
import org.lwjgl.BufferUtils;

public class ResourceColored3DObject extends Resource {
	public static ResourceColored3DObject create() {
		final ResourceColored3DObject resource = new ResourceColored3DObject();
		getManager().localAdd(resource);
		return resource;
	}
	
	private final ResourceProgram program;
	private int oGLPosition;
	private int oGLColor;
	private int oGLMatrixTransformation;
	private int oGLMatrixProjection;
	private int oGLMatrixView;
	
	protected ResourceColored3DObject() {
		super();
		// get the shader resource :
		this.oGLPosition = 0;
		this.program = ResourceProgram.create(new Uri("DATA", "simple3D.vert", "gale"), new Uri("DATA", "simple3D.frag", "gale"));
		if (this.program != null) {
			this.oGLMatrixTransformation = this.program.getUniform("in_matrixTransformation");
			this.oGLMatrixProjection = this.program.getUniform("in_matrixProjection");
			this.oGLMatrixView = this.program.getUniform("in_matrixView");
			this.oGLPosition = this.program.getAttribute("in_position");
			this.oGLColor = this.program.getUniform("in_colors");
		}
	}
	
	@Override
	public void cleanUp() {
		// TODO Auto-generated method stub
		
	}
	
	private float[] convertInFloat(final List<Vector3f> data) {
		final float[] out = new float[data.size() * 3];
		for (int iii = 0; iii < data.size(); iii++) {
			out[iii * 3] = data.get(iii).x();
			out[iii * 3 + 1] = data.get(iii).y();
			out[iii * 3 + 2] = data.get(iii).z();
		}
		return out;
	}
	
	public void draw(final List<Vector3f> vertices, final Color color, final boolean updateDepthBuffer, final boolean depthtest) {
		if (vertices.size() <= 0) {
			return;
		}
		if (this.program == null) {
			Log.error("No shader ...");
			return;
		}
		if (depthtest) {
			OpenGL.enable(OpenGL.Flag.flag_depthTest);
			if (!updateDepthBuffer) {
				OpenGL.setDeathMask(false);
			}
		}
		//Log.debug("    display " << this.coord.size() << " elements" );
		this.program.use();
		final Matrix4f projectionMatrix = OpenGL.getMatrix();
		final Matrix4f viewMatrix = OpenGL.getCameraMatrix();
		final Matrix4f transformationMatrix = Matrix4f.IDENTITY;
		this.program.uniformMatrix(this.oGLMatrixView, viewMatrix);
		this.program.uniformMatrix(this.oGLMatrixProjection, projectionMatrix);
		this.program.uniformMatrix(this.oGLMatrixTransformation, transformationMatrix);
		// color :
		this.program.uniformColor(this.oGLColor, color);
		// position :
		final FloatBuffer buffer = storeDataInFloatBuffer(convertInFloat(vertices));
		this.program.sendAttribute(this.oGLPosition, 3, buffer, 3);
		if (color.a() < 1.0f) {
			OpenGL.enable(OpenGL.Flag.flag_blend);
		}
		// Request the draw of the elements: 
		OpenGL.drawArrays(OpenGL.RenderMode.TRIANGLE, 0, vertices.size());
		if (color.a() < 1.0f) {
			OpenGL.disable(OpenGL.Flag.flag_blend);
		}
		this.program.unUse();
		// Request the draw od the elements: 
		//glDrawArrays(oGLLINES, 0, vertices.size());
		//this.oGLprogram.UnUse();
		if (depthtest) {
			if (!updateDepthBuffer) {
				OpenGL.setDeathMask(true);
			}
			OpenGL.disable(OpenGL.Flag.flag_depthTest);
		}
	}
	
	public void draw(final List<Vector3f> vertices, final Color color, final Matrix4f transformationMatrix, final boolean updateDepthBuffer, final boolean depthtest) {
		if (vertices.size() <= 0) {
			return;
		}
		if (this.program == null) {
			Log.error("No shader ...");
			return;
		}
		if (depthtest) {
			OpenGL.enable(OpenGL.Flag.flag_depthTest);
			if (!updateDepthBuffer) {
				OpenGL.setDeathMask(false);
			}
		}
		//Log.debug("    display " << this.coord.size() << " elements" );
		this.program.use();
		// set Matrix: translation/positionMatrix
		final Matrix4f projectionMatrix = OpenGL.getMatrix();
		final Matrix4f viewMatrix = OpenGL.getCameraMatrix();
		this.program.uniformMatrix(this.oGLMatrixView, viewMatrix);
		this.program.uniformMatrix(this.oGLMatrixProjection, projectionMatrix);
		this.program.uniformMatrix(this.oGLMatrixTransformation, transformationMatrix);
		// position :
		final FloatBuffer buffer = storeDataInFloatBuffer(convertInFloat(vertices));
		this.program.sendAttribute(this.oGLPosition, 3, buffer, 3);
		// color :
		//Log.info("color= " + color + " " + this.oGLPosition);
		this.program.uniformColor(this.oGLColor, color);
		if (color.a() < 1.0f) {
			OpenGL.enable(OpenGL.Flag.flag_blend);
		}
		// Request the draw of the elements: 
		OpenGL.drawArrays(OpenGL.RenderMode.TRIANGLE, 0, vertices.size());
		if (color.a() < 1.0f) {
			OpenGL.disable(OpenGL.Flag.flag_blend);
		}
		this.program.unUse();
		if (depthtest) {
			if (!updateDepthBuffer) {
				OpenGL.setDeathMask(true);
			}
			OpenGL.disable(OpenGL.Flag.flag_depthTest);
		}
	}
	
	public void drawCapsule(final float radius, final float size, int lats, final int longs, final Matrix4f transformationMatrix, final Color tmpColor) {
		final List<Vector3f> tmpVertices = new ArrayList<>();
		lats = lats / 2 * 2;
		
		// center to border (TOP)
		float offset = size * 0.5f;
		for (int iii = lats / 2 + 1; iii <= lats; ++iii) {
			final float lat0 = (float) Math.PI * (-0.5f + (float) (iii - 1) / lats);
			final float z0 = radius * (float) Math.sin(lat0);
			final float zr0 = radius * (float) Math.cos(lat0);
			
			final float lat1 = (float) Math.PI * (-0.5f + (float) (iii) / lats);
			final float z1 = radius * (float) Math.sin(lat1);
			final float zr1 = radius * (float) Math.cos(lat1);
			
			for (int jjj = 0; jjj < longs; ++jjj) {
				float lng = 2.0f * (float) Math.PI * (jjj - 1) / longs;
				float x = (float) Math.cos(lng);
				float y = (float) Math.sin(lng);
				final Vector3f v1 = new Vector3f(x * zr1, y * zr1, z1 + offset);
				final Vector3f v4 = new Vector3f(x * zr0, y * zr0, z0 + offset);
				
				lng = 2 * (float) Math.PI * (jjj) / longs;
				x = (float) Math.cos(lng);
				y = (float) Math.sin(lng);
				final Vector3f v2 = new Vector3f(x * zr1, y * zr1, z1 + offset);
				final Vector3f v3 = new Vector3f(x * zr0, y * zr0, z0 + offset);
				tmpVertices.add(v1);
				tmpVertices.add(v2);
				tmpVertices.add(v3);
				
				tmpVertices.add(v1);
				tmpVertices.add(v3);
				tmpVertices.add(v4);
			}
		}
		// Cylinder
		for (int jjj = 0; jjj < longs; ++jjj) {
			float lng = 2.0f * (float) Math.PI * (jjj - 1) / longs;
			
			final float z = size * 0.5f;
			
			float x = (float) Math.cos(lng) * radius;
			float y = (float) Math.sin(lng) * radius;
			final Vector3f v2 = new Vector3f(x, y, z);
			final Vector3f v2b = new Vector3f(x, y, -z);
			
			lng = 2.0f * (float) Math.PI * (jjj) / longs;
			x = (float) Math.cos(lng) * radius;
			y = (float) Math.sin(lng) * radius;
			final Vector3f v3 = new Vector3f(x, y, z);
			final Vector3f v3b = new Vector3f(x, y, -z);
			
			tmpVertices.add(v2);
			tmpVertices.add(v3);
			tmpVertices.add(v3b);
			
			tmpVertices.add(v2);
			tmpVertices.add(v3b);
			tmpVertices.add(v2b);
		}
		// center to border (BUTTOM)
		offset = -size * 0.5f;
		for (int iii = 0; iii <= lats / 2; ++iii) {
			final float lat0 = (float) Math.PI * (-0.5f + (float) (iii - 1) / lats);
			final float z0 = radius * (float) Math.sin(lat0);
			final float zr0 = radius * (float) Math.cos(lat0);
			
			final float lat1 = (float) Math.PI * (-0.5f + (float) (iii) / lats);
			final float z1 = radius * (float) Math.sin(lat1);
			final float zr1 = radius * (float) Math.cos(lat1);
			
			for (int jjj = 0; jjj < longs; ++jjj) {
				float lng = 2.0f * (float) Math.PI * (jjj - 1) / longs;
				float x = (float) Math.cos(lng);
				float y = (float) Math.sin(lng);
				final Vector3f v1 = new Vector3f(x * zr1, y * zr1, z1 + offset);
				final Vector3f v4 = new Vector3f(x * zr0, y * zr0, z0 + offset);
				
				lng = 2 * (float) Math.PI * (jjj) / longs;
				x = (float) Math.cos(lng);
				y = (float) Math.sin(lng);
				final Vector3f v2 = new Vector3f(x * zr1, y * zr1, z1 + offset);
				final Vector3f v3 = new Vector3f(x * zr0, y * zr0, z0 + offset);
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
	
	public void drawCone(final float radius, final float size, final int lats, final int longs, final Matrix4f transformationMatrix, final Color tmpColor) {
		final List<Vector3f> tmpVertices = new ArrayList<>();
		// center to border (TOP)
		for (int jjj = 0; jjj < longs; ++jjj) {
			float lng = 2.0f * (float) Math.PI * (jjj - 1) / longs;
			final Vector3f v1 = new Vector3f(0.0f, 0.0f, -size / 2);
			
			float x = (float) Math.cos(lng) * radius;
			float y = (float) Math.sin(lng) * radius;
			final Vector3f v2 = new Vector3f(x, y, size / 2);
			
			lng = 2.0f * (float) Math.PI * (jjj) / longs;
			x = (float) Math.cos(lng) * radius;
			y = (float) Math.sin(lng) * radius;
			final Vector3f v3 = new Vector3f(x, y, size / 2);
			tmpVertices.add(v1);
			tmpVertices.add(v3);
			tmpVertices.add(v2);
		}
		// center to border (BUTTOM)
		for (int jjj = 0; jjj < longs; ++jjj) {
			float lng = 2.0f * (float) Math.PI * (jjj - 1) / longs;
			
			final Vector3f v1 = new Vector3f(0.0f, 0.0f, size / 2);
			
			float x = (float) Math.cos(lng) * radius;
			float y = (float) Math.sin(lng) * radius;
			final Vector3f v2 = new Vector3f(x, y, size / 2);
			
			lng = 2.0f * (float) Math.PI * (jjj) / longs;
			x = (float) Math.cos(lng) * radius;
			y = (float) Math.sin(lng) * radius;
			final Vector3f v3 = new Vector3f(x, y, size / 2);
			tmpVertices.add(v1);
			tmpVertices.add(v2);
			tmpVertices.add(v3);
		}
		draw(tmpVertices, tmpColor, transformationMatrix, true, true);
	}
	
	public void drawCubeLine(final Vector3f min, final Vector3f max, final Color color, final Matrix4f transformationMatrix, final boolean updateDepthBuffer, final boolean depthtest) {
		final List<Vector3f> vertices = new ArrayList<>();
		vertices.add(new Vector3f(min.x(), min.y(), min.z()));
		vertices.add(new Vector3f(max.x(), min.y(), min.z()));
		
		vertices.add(new Vector3f(max.x(), min.y(), min.z()));
		vertices.add(new Vector3f(max.x(), min.y(), max.z()));
		
		vertices.add(new Vector3f(max.x(), min.y(), max.z()));
		vertices.add(new Vector3f(min.x(), min.y(), max.z()));
		
		vertices.add(new Vector3f(min.x(), min.y(), max.z()));
		vertices.add(new Vector3f(min.x(), min.y(), min.z()));
		
		vertices.add(new Vector3f(min.x(), max.y(), min.z()));
		vertices.add(new Vector3f(max.x(), max.y(), min.z()));
		
		vertices.add(new Vector3f(max.x(), max.y(), min.z()));
		vertices.add(new Vector3f(max.x(), max.y(), max.z()));
		
		vertices.add(new Vector3f(max.x(), max.y(), max.z()));
		vertices.add(new Vector3f(min.x(), max.y(), max.z()));
		
		vertices.add(new Vector3f(min.x(), max.y(), max.z()));
		vertices.add(new Vector3f(min.x(), max.y(), min.z()));
		
		vertices.add(new Vector3f(min.x(), min.y(), min.z()));
		vertices.add(new Vector3f(min.x(), max.y(), min.z()));
		
		vertices.add(new Vector3f(max.x(), min.y(), min.z()));
		vertices.add(new Vector3f(max.x(), max.y(), min.z()));
		
		vertices.add(new Vector3f(max.x(), min.y(), max.z()));
		vertices.add(new Vector3f(max.x(), max.y(), max.z()));
		
		vertices.add(new Vector3f(min.x(), min.y(), max.z()));
		vertices.add(new Vector3f(min.x(), max.y(), max.z()));
		drawLine(vertices, color, transformationMatrix, updateDepthBuffer, depthtest);
	}
	
	public void drawCylinder(final float radius, final float size, final int lats, final int longs, final Matrix4f transformationMatrix, final Color tmpColor) {
		final List<Vector3f> tmpVertices = new ArrayList<>();
		// center to border (TOP)
		
		// center to border (TOP)
		for (int jjj = 0; jjj < longs; ++jjj) {
			float lng = 2.0f * (float) Math.PI * (jjj - 1) / longs;
			
			final float z = size * 0.5f;
			final Vector3f v1 = new Vector3f(0.0f, 0.0f, z);
			
			float x = (float) Math.cos(lng) * radius;
			float y = (float) Math.sin(lng) * radius;
			final Vector3f v2 = new Vector3f(x, y, z);
			
			lng = 2.0f * (float) Math.PI * (jjj) / longs;
			x = (float) Math.cos(lng) * radius;
			y = (float) Math.sin(lng) * radius;
			final Vector3f v3 = new Vector3f(x, y, z);
			tmpVertices.add(v1);
			tmpVertices.add(v3);
			tmpVertices.add(v2);
		}
		// Cylinder
		for (int jjj = 0; jjj < longs; ++jjj) {
			float lng = 2.0f * (float) Math.PI * (jjj - 1) / longs;
			
			final float z = size * 0.5f;
			
			float x = (float) Math.cos(lng) * radius;
			float y = (float) Math.sin(lng) * radius;
			final Vector3f v2 = new Vector3f(x, y, z);
			final Vector3f v2b = new Vector3f(x, y, -z);
			
			lng = 2.0f * (float) Math.PI * (jjj) / longs;
			x = (float) Math.cos(lng) * radius;
			y = (float) Math.sin(lng) * radius;
			final Vector3f v3 = new Vector3f(x, y, z);
			final Vector3f v3b = new Vector3f(x, y, -z);
			
			tmpVertices.add(v2);
			tmpVertices.add(v3);
			tmpVertices.add(v3b);
			
			tmpVertices.add(v2);
			tmpVertices.add(v3b);
			tmpVertices.add(v2b);
		}
		// center to border (BUTTOM)
		for (int jjj = 0; jjj < longs; ++jjj) {
			float lng = 2.0f * (float) Math.PI * (jjj - 1) / longs;
			
			final float z = size * -0.5f;
			final Vector3f v1 = new Vector3f(0.0f, 0.0f, z);
			
			float x = (float) Math.cos(lng) * radius;
			float y = (float) Math.sin(lng) * radius;
			final Vector3f v2 = new Vector3f(x, y, z);
			
			lng = 2.0f * (float) Math.PI * (jjj) / longs;
			x = (float) Math.cos(lng) * radius;
			y = (float) Math.sin(lng) * radius;
			final Vector3f v3 = new Vector3f(x, y, z);
			tmpVertices.add(v1);
			tmpVertices.add(v2);
			tmpVertices.add(v3);
		}
		draw(tmpVertices, tmpColor, transformationMatrix, true, true);
	}
	
	public void drawLine(final List<Vector3f> vertices, final Color color, final Matrix4f transformationMatrix, final boolean updateDepthBuffer, final boolean depthtest) {
		if (vertices.size() <= 0) {
			return;
		}
		if (this.program == null) {
			Log.error("No shader ...");
			return;
		}
		if (depthtest) {
			OpenGL.enable(OpenGL.Flag.flag_depthTest);
			if (!updateDepthBuffer) {
				OpenGL.setDeathMask(false);
			}
		}
		//Log.debug("    display " << this.coord.size() << " elements" );
		this.program.use();
		// set Matrix: translation/positionMatrix
		final Matrix4f projectionMatrix = OpenGL.getMatrix();
		final Matrix4f viewMatrix = OpenGL.getCameraMatrix();
		this.program.uniformMatrix(this.oGLMatrixView, viewMatrix);
		this.program.uniformMatrix(this.oGLMatrixProjection, projectionMatrix);
		this.program.uniformMatrix(this.oGLMatrixTransformation, transformationMatrix);
		// position :
		final FloatBuffer buffer = storeDataInFloatBuffer(convertInFloat(vertices));
		this.program.sendAttribute(this.oGLPosition, 3, buffer, 3);
		// color :
		this.program.uniformColor(this.oGLColor, color);
		if (color.a() < 1.0f) {
			OpenGL.enable(OpenGL.Flag.flag_blend);
		}
		// Request the draw od the elements: 
		OpenGL.drawArrays(OpenGL.RenderMode.LINE, 0, vertices.size());
		if (color.a() < 1.0f) {
			OpenGL.disable(OpenGL.Flag.flag_blend);
		}
		this.program.unUse();
		if (depthtest) {
			if (!updateDepthBuffer) {
				OpenGL.setDeathMask(true);
			}
			OpenGL.disable(OpenGL.Flag.flag_depthTest);
		}
	}
	
	public void drawSphere(final float radius, final int lats, final int longs, final Matrix4f transformationMatrix, final Color tmpColor) {
		final List<Vector3f> tmpVertices = new ArrayList<>();
		for (int iii = 0; iii <= lats; ++iii) {
			final float lat0 = (float) Math.PI * (-0.5f + (float) (iii - 1) / lats);
			final float z0 = radius * (float) Math.sin(lat0);
			final float zr0 = radius * (float) Math.cos(lat0);
			
			final float lat1 = (float) Math.PI * (-0.5f + (float) (iii) / lats);
			final float z1 = radius * (float) Math.sin(lat1);
			final float zr1 = radius * (float) Math.cos(lat1);
			
			for (int jjj = 0; jjj < longs; ++jjj) {
				float lng = 2.0f * (float) Math.PI * (jjj - 1) / longs;
				float x = (float) Math.cos(lng);
				float y = (float) Math.sin(lng);
				final Vector3f v1 = new Vector3f(x * zr1, y * zr1, z1);
				final Vector3f v4 = new Vector3f(x * zr0, y * zr0, z0);
				
				lng = 2 * (float) Math.PI * (jjj) / longs;
				x = (float) Math.cos(lng);
				y = (float) Math.sin(lng);
				final Vector3f v2 = new Vector3f(x * zr1, y * zr1, z1);
				final Vector3f v3 = new Vector3f(x * zr0, y * zr0, z0);
				
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
	
	public void drawSquare(final Vector3f size, final Matrix4f transformationMatrix, final Color tmpColor) {
		final List<Vector3f> tmpVertices = new ArrayList<>();
		final int[] indices = { 0, 1, 2, 3, 2, 1, 4, 0, 6, 6, 0, 2, 5, 1, 4, 4, 1, 0, 7, 3, 1, 7, 1, 5, 5, 4, 7, 7, 4, 6, 7, 2, 3, 7, 6, 2 };
		final Vector3f[] vertices = { new Vector3f(size.x(), size.y(), size.z()), new Vector3f(-size.x(), size.y(), size.z()), new Vector3f(size.x(), -size.y(), size.z()),
				new Vector3f(-size.x(), -size.y(), size.z()), new Vector3f(size.x(), size.y(), -size.z()), new Vector3f(-size.x(), size.y(), -size.z()), new Vector3f(size.x(), -size.y(), -size.z()),
				new Vector3f(-size.x(), -size.y(), -size.z()) };
		tmpVertices.clear();
		for (int iii = 0; iii < 36; iii += 3) {
			// normal calculation :
			//btVector3 normal = (vertices[indices[iii+2]]-vertices[indices[iii]]).cross(vertices[indices[iii+1]]-vertices[indices[iii]]);
			//normal.normalize ();
			tmpVertices.add(vertices[indices[iii]]);
			tmpVertices.add(vertices[indices[iii + 1]]);
			tmpVertices.add(vertices[indices[iii + 2]]);
		}
		draw(tmpVertices, tmpColor, transformationMatrix, true, true);
	}
	
	public void drawTriangles(final List<Vector3f> vertex, final List<Integer> indice, final Matrix4f transformationMatrix, final Color tmpColor) {
		drawTriangles(vertex, indice, transformationMatrix, tmpColor, new Vector3f(0.0f, 0.0f, 0.1f));
	}
	
	public void drawTriangles(final List<Vector3f> vertex, final List<Integer> indice, final Matrix4f transformationMatrix, final Color tmpColor, final Vector3f offset) {
		final List<Vector3f> tmpVertices = new ArrayList<>();
		for (int iii = 0; iii < indice.size() / 3; ++iii) {
			tmpVertices.add(vertex.get(indice.get(iii * 3 + 0)).add(offset));
			tmpVertices.add(vertex.get(indice.get(iii * 3 + 1)).add(offset));
			tmpVertices.add(vertex.get(indice.get(iii * 3 + 2)).add(offset));
			//Log.info("  indices " << indice[iii*3 + 0] << " " << indice[iii*3 + 1] << " " << indice[iii*3 + 2]);
			//Log.info(" triangle " << vertex[indice[iii*3 + 0]] << " " << vertex[indice[iii*3 + 1]] << " " << vertex[indice[iii*3 + 2]]);
		}
		//Log.info("display " << tmpVertices.size() << " vertices form " << indice.size());
		draw(tmpVertices, tmpColor, transformationMatrix, true, true);
	}
	
	private FloatBuffer storeDataInFloatBuffer(final float[] data) {
		final FloatBuffer buffer = BufferUtils.createFloatBuffer(data.length);
		buffer.put(data);
		buffer.flip();
		return buffer;
	}
}
