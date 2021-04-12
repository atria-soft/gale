package org.atriasoft.gale.backend3d;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.Reader;
import java.nio.ByteBuffer;
import java.nio.FloatBuffer;
import java.nio.IntBuffer;
import java.nio.charset.Charset;
import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.atriasoft.etk.Color;
import org.atriasoft.etk.Uri;
import org.atriasoft.etk.math.Matrix4f;
import org.atriasoft.etk.math.Vector2f;
import org.atriasoft.etk.math.Vector2i;
import org.atriasoft.etk.math.Vector3f;
import org.atriasoft.etk.math.Vector3i;
import org.atriasoft.gale.internal.Log;
import org.lwjgl.BufferUtils;
import org.lwjgl.opengl.GL11;
import org.lwjgl.opengl.GL13;
import org.lwjgl.opengl.GL15;
import org.lwjgl.opengl.GL20;
import org.lwjgl.opengl.GL30;
import org.lwjgl.opengl.GL40;

public class OpenGL {
	public static enum ClearFlag {
		clearFlag_colorBuffer, // !< Indicates the buffers currently enabled for color writing.
		clearFlag_depthBuffer, // !< Indicates the depth buffer.
		clearFlag_stencilBuffer // !< Indicates the stencil buffer.
	}
	
	// We map all the flag, but not all is supported by all platform...
	public static enum Flag {
		flag_blend, // !< If enabled, blend the computed fragment color values with the values in the color buffers. See glBlendFunc.
		flag_clipDistanceI, // !< If enabled, clip geometry against user-defined half space i.
		flag_colorLogigOP, // !< If enabled, apply the currently selected logical operation to the computed
		// fragment color and color buffer values. See glLogicOp.
		flag_cullFace, // !< If enabled, cull polygons based on their winding in window coordinates.
		// See glCullFace.
		flag_debugOutput, // !< If enabled, debug messages are produced by a debug context. When disabled,
		// the debug message log is silenced. Note that in a non-debug context, very
		// few, if any messages might be produced, even when GLDEBUGOUTPUT is enabled.
		flag_debugOutputSynchronous, // !< If enabled, debug messages are produced synchronously by a debug context.
		// If disabled, debug messages may be produced asynchronously. In particular,
		// they may be delayed relative to the execution of GL commands, and the debug
		// callback function may be called from a thread other than that in which the
		// commands are executed. See glDebugMessageCallback.
		flag_depthClamp, // !< If enabled, the -wcdzcdwc plane equation is ignored by view volume
		// clipping (effectively, there is no near or far plane clipping). See
		// glDepthRange.
		flag_depthTest, // !< If enabled, do depth comparisons and update the depth buffer. Note that
		// even if the depth buffer exists and the depth mask is non-zero, the depth
		// buffer is not updated if the depth test is disabled. See glDepthFunc and glDepthRange.
		flag_dither, // !< If enabled, dither color components or indices before they are written to the color buffer.
		flag_framebufferSRGB, // !< If enabled and the value of GLFRAMEBUFFERATTACHMENTCOLORENCODING for the
		// framebuffer attachment corresponding to the destination buffer is GLSRGB, the
		// R, G, and B destination color values (after conversion from fixed-point to
		// floating-point) are considered to be encoded for the sRGB color space and
		// hence are linearized prior to their use in blending.
		flag_lineSmooth, // !< If enabled, draw lines with correct filtering. Otherwise, draw aliased
		// lines. See glLineWidth.
		flag_multisample, // !< If enabled, use multiple fragment samples in computing the final color of
		// a pixel. See glSampleCoverage.
		flag_polygonOffsetFill, // !< If enabled, and if the polygon is rendered in GLFILL mode, an offset is
		// added to depth values of a polygon's fragments before the depth comparison is
		// performed. See glPolygonOffset.
		flag_polygonOffsetLine, // !< If enabled, and if the polygon is rendered in GLLINE mode, an offset is
		// added to depth values of a polygon's fragments before the depth comparison is
		// performed. See glPolygonOffset.
		flag_polygonOffsetPoint, // !< If enabled, an offset is added to depth values of a polygon's fragments
		// before the depth comparison is performed, if the polygon is rendered in
		// GLPOINT mode. See glPolygonOffset.
		flag_polygonSmooth, // !< If enabled, draw polygons with proper filtering. Otherwise, draw aliased
		// polygons. For correct antialiased polygons, an alpha buffer is needed and the
		// polygons must be sorted front to back.
		flag_primitiveRestart, // !< enables primitive restarting. If enabled, any one of the draw commands
		// which transfers a set of generic attribute array elements to the GL will
		// restart the primitive when the index of the vertex is equal to the primitive
		// restart index. See glPrimitiveRestartIndex.
		flag_primitiveRestartFixedIndex, // !< enables primitive restarting with a fixed index. If enabled, any one of
		// the draw commands which transfers a set of generic attribute array
		// elements to the GL will restart the primitive when the index of the
		// vertex is equal to the fixed primitive index for the specified index
		// type. The fixed index is equal to 2n1 where n is equal to 8 for
		// GLUNSIGNEDBYTE, 16 for GLUNSIGNEDSHORT and 32 for GLUNSIGNEDINT.
		flag_sampleAlphaToCoverage, // !< If enabled, compute a temporary coverage value where each bit is
		// determined by the alpha value at the corresponding sample location. The
		// temporary coverage value is then ANDed with the fragment coverage value.
		flag_sampleAlphaToOne, // !< If enabled, each sample alpha value is replaced by the maximum
		// representable alpha value.
		flag_sampleCoverage, // !< If enabled, the fragment's coverage is ANDed with the temporary coverage
		// value. If GLSAMPLECOVERAGEINVERT is set to GLTRUE, invert the coverage value.
		// See glSampleCoverage.
		flag_sampleShading, // !< If enabled, the active fragment shader is run once for each covered
		// sample, or at fraction of this rate as determined by the current value of
		// GLMINSAMPLESHADINGVALUE. See glMinSampleShading.
		flag_sampleMask, // !< If enabled, the sample coverage mask generated for a fragment during
		// rasterization will be ANDed with the value of GLSAMPLEMASKVALUE before
		// shading occurs. See glSampleMaski.
		flag_scissorTest, // !< If enabled, discard fragments that are outside the scissor rectangle. See
		// glScissor.
		flag_stencilTest, // !< If enabled, do stencil testing and update the stencil buffer. See
		// glStencilFunc and glStencilOp. GLTEXTURECUBEMAPSEAMLESS = 1+0, //!< If
		// enabled, cubemap textures are sampled such that when linearly sampling from
		// the border between two adjacent faces, texels from both faces are used to
		// generate the final sample value. When disabled, texels from only a single
		// face are used to ruct the final sample value.
		flag_programPointSize, // !< If enabled and a vertex or geometry shader is active, then the derived
		// point size is taken from the (potentially clipped) shader builtin
		// gthis.PointSize and clamped to the implementation-dependent point size range.
		flag_texture2D, // !<
		flag_alphaTest, // !<
		flag_fog, flag_back
	}
	
	public enum RenderMode {
		point, line, lineStrip, // !< Not supported in GALE (TODO Later)
		lineLoop, triangle, triangleStrip, // !< Not supported in GALE (TODO Later)
		triangleFan, // !< Not supported in GALE (TODO Later)
		quad, // !< Not supported in OpenGL-ES2
		quadStrip, // !< Not supported in OpenGL-ES2
		polygon // !< Not supported in OpenGL-ES2
	}
	
	/* Shader wrapping : */
	public static enum ShaderType {
		vertex, fragment
	};
	
	public static class StateFlag {
		public boolean current = false;
		public boolean mustBeSet = false;
	}
	
	public enum Usage {
		streamDraw, staticDraw, dynamicDraw
	}
	
	private static final int[] TEXTURE_ID_BINDING = { GL13.GL_TEXTURE0, GL13.GL_TEXTURE1, GL13.GL_TEXTURE2, GL13.GL_TEXTURE3, GL13.GL_TEXTURE4, GL13.GL_TEXTURE5, GL13.GL_TEXTURE6, GL13.GL_TEXTURE7,
			GL13.GL_TEXTURE8, GL13.GL_TEXTURE9, GL13.GL_TEXTURE10, GL13.GL_TEXTURE11, GL13.GL_TEXTURE12, GL13.GL_TEXTURE13, GL13.GL_TEXTURE14, GL13.GL_TEXTURE15, GL13.GL_TEXTURE16, GL13.GL_TEXTURE17,
			GL13.GL_TEXTURE18, GL13.GL_TEXTURE19, GL13.GL_TEXTURE20, GL13.GL_TEXTURE21, GL13.GL_TEXTURE22, GL13.GL_TEXTURE23, GL13.GL_TEXTURE24, GL13.GL_TEXTURE25, GL13.GL_TEXTURE26,
			GL13.GL_TEXTURE27, GL13.GL_TEXTURE28, GL13.GL_TEXTURE29, GL13.GL_TEXTURE30, GL13.GL_TEXTURE31 };
	
	public static final int GL_RGB = GL11.GL_RGB;
	
	public static final int GL_RGBA = GL11.GL_RGBA;
	public static final int GL_UNSIGNED_BYTE = GL11.GL_UNSIGNED_BYTE;
	
	public static final int GL_TEXTURE_2D = GL11.GL_TEXTURE_2D;
	
	static final boolean DEBUG = false; // TODO externalize this ...
	
	static final boolean CHECKERROROPENGL = false; // TODO externalize this ...
	
	static final boolean DIRECT_MODE = false; // TODO externalize this ...;
	
	private static final List<Matrix4f> MATRIX_LIST = new ArrayList<>();
	
	private static Matrix4f matrixCamera = Matrix4f.IDENTITY;
	
	private static int programId = 0;
	private static final Map<RenderMode, Integer> CONVERT_RENDER_MODE = Map.of(RenderMode.point, GL11.GL_POINTS, RenderMode.line, GL11.GL_LINES, RenderMode.lineStrip, GL11.GL_LINE_STRIP,
			RenderMode.lineLoop, GL11.GL_LINE_LOOP, RenderMode.triangle, GL11.GL_TRIANGLES, RenderMode.triangleStrip, GL11.GL_TRIANGLE_STRIP, RenderMode.triangleFan, GL11.GL_TRIANGLE_FAN,
			RenderMode.quad, GL11.GL_QUADS, RenderMode.quadStrip, GL11.GL_QUAD_STRIP, RenderMode.polygon, GL11.GL_POLYGON);
	
	private static final Map<Flag, Integer> BASIC_FLAG;
	private static boolean flagsStatesChange = false;
	
	private static final Map<Flag, StateFlag> FLAGS_STATES = new HashMap<>();
	public static Map<Usage, Integer> convertUsage;
	static {
		FLAGS_STATES.put(Flag.flag_blend, new StateFlag());
		FLAGS_STATES.put(Flag.flag_clipDistanceI, new StateFlag());
		FLAGS_STATES.put(Flag.flag_colorLogigOP, new StateFlag());
		FLAGS_STATES.put(Flag.flag_cullFace, new StateFlag());
		FLAGS_STATES.put(Flag.flag_debugOutput, new StateFlag());
		FLAGS_STATES.put(Flag.flag_debugOutputSynchronous, new StateFlag());
		FLAGS_STATES.put(Flag.flag_depthClamp, new StateFlag());
		FLAGS_STATES.put(Flag.flag_depthTest, new StateFlag());
		FLAGS_STATES.put(Flag.flag_dither, new StateFlag());
		FLAGS_STATES.put(Flag.flag_framebufferSRGB, new StateFlag());
		FLAGS_STATES.put(Flag.flag_lineSmooth, new StateFlag());
		FLAGS_STATES.put(Flag.flag_multisample, new StateFlag());
		FLAGS_STATES.put(Flag.flag_polygonOffsetFill, new StateFlag());
		FLAGS_STATES.put(Flag.flag_polygonOffsetLine, new StateFlag());
		FLAGS_STATES.put(Flag.flag_polygonOffsetPoint, new StateFlag());
		FLAGS_STATES.put(Flag.flag_polygonSmooth, new StateFlag());
		FLAGS_STATES.put(Flag.flag_primitiveRestart, new StateFlag());
		FLAGS_STATES.put(Flag.flag_primitiveRestartFixedIndex, new StateFlag());
		FLAGS_STATES.put(Flag.flag_sampleAlphaToCoverage, new StateFlag());
		FLAGS_STATES.put(Flag.flag_sampleAlphaToOne, new StateFlag());
		FLAGS_STATES.put(Flag.flag_sampleCoverage, new StateFlag());
		FLAGS_STATES.put(Flag.flag_sampleShading, new StateFlag());
		FLAGS_STATES.put(Flag.flag_sampleMask, new StateFlag());
		FLAGS_STATES.put(Flag.flag_scissorTest, new StateFlag());
		FLAGS_STATES.put(Flag.flag_stencilTest, new StateFlag());
		FLAGS_STATES.put(Flag.flag_programPointSize, new StateFlag());
		FLAGS_STATES.put(Flag.flag_texture2D, new StateFlag());
		FLAGS_STATES.put(Flag.flag_alphaTest, new StateFlag());
		FLAGS_STATES.put(Flag.flag_fog, new StateFlag());
		FLAGS_STATES.put(Flag.flag_back, new StateFlag());
		BASIC_FLAG = new HashMap<>();
		BASIC_FLAG.put(Flag.flag_blend, GL11.GL_BLEND);
		// basicFlag.put(Flag.flag_clipDistanceI, GL_CLIP_DISTANCE0);
		// basicFlag.put(Flag.flag_colorLogigOP, GL_COLOR_LOGIC_OP);
		BASIC_FLAG.put(Flag.flag_cullFace, GL11.GL_CULL_FACE);
		// basicFlag.put(Flag.flag_debugOutput, GLDEBUGOUTPUT);
		// basicFlag.put(Flag.flag_debugOutputSynchronous, GLDEBUGOUTPUTSYNCHRONOUS);
		// basicFlag.put(Flag.flag_depthClamp, GLDEPTHCLAMP);
		BASIC_FLAG.put(Flag.flag_depthTest, GL11.GL_DEPTH_TEST);
		BASIC_FLAG.put(Flag.flag_dither, GL11.GL_DITHER);
		// basicFlag.put(Flag.flag_framebufferSRGB, GLFRAMEBUFFERSRGB);
		// basicFlag.put(Flag.flag_lineSmooth, GLLINESMOOTH);
		// basicFlag.put(Flag.flag_multisample, GLMULTISAMPLE);
		BASIC_FLAG.put(Flag.flag_polygonOffsetFill, GL11.GL_POLYGON_OFFSET_FILL);
		// basicFlag.put(Flag.flag_polygonOffsetLine, GLPOLYGONOFFSETLINE);
		// basicFlag.put(Flag.flag_polygonOffsetPoint, GLPOLYGONOFFSETPOINT);
		// basicFlag.put(Flag.flag_polygonSmooth, GLPOLYGONSMOOTH);
		// basicFlag.put(Flag.flag_primitiveRestart, GLPRIMITIVERESTART);
		// basicFlag.put(Flag.flag_primitiveRestartFixedIndex,
		// GLPRIMITIVERESTARTFIXEDINDEX);
		BASIC_FLAG.put(Flag.flag_sampleAlphaToCoverage, GL20.GL_SAMPLE_ALPHA_TO_COVERAGE);
		// basicFlag.put(Flag.flag_sampleAlphaToOne, GLSAMPLEALPHATOONE);
		BASIC_FLAG.put(Flag.flag_sampleCoverage, GL30.GL_SAMPLE_COVERAGE);
		// basicFlag.put(Flag.flag_sampleShading, GLSAMPLESHADING);
		// basicFlag.put(Flag.flag_sampleMask, GLSAMPLEMASK);
		BASIC_FLAG.put(Flag.flag_scissorTest, GL11.GL_SCISSOR_TEST);
		BASIC_FLAG.put(Flag.flag_stencilTest, GL30.GL_STENCIL_TEST);
		// basicFlag.put(Flag.flag_programPointSize, GLPROGRAMPOINTSIZE);
		BASIC_FLAG.put(Flag.flag_texture2D, GL11.GL_TEXTURE_2D);
		// basicFlag.put(Flag.flag_alphaTest, GLALPHATEST);
		// basicFlag.put(Flag.flag_fog, GLFOG);
		BASIC_FLAG.put(Flag.flag_back, GL11.GL_BACK);
		convertUsage = new HashMap<>();
		convertUsage.put(Usage.streamDraw, GL20.GL_STREAM_DRAW);
		convertUsage.put(Usage.staticDraw, GL20.GL_STATIC_DRAW);
		convertUsage.put(Usage.dynamicDraw, GL20.GL_DYNAMIC_DRAW);
	}
	private static final Map<Long, Boolean> THREAD_HAS_CONTEXT = new HashMap<>();
	private static final Map<ClearFlag, Integer> BASIC_FLAG_CLEAR = Map.of(ClearFlag.clearFlag_colorBuffer, GL11.GL_COLOR_BUFFER_BIT, ClearFlag.clearFlag_depthBuffer, GL11.GL_DEPTH_BUFFER_BIT,
			ClearFlag.clearFlag_stencilBuffer, GL11.GL_STENCIL_BUFFER_BIT);
	
	/**
	 * enable Texture on the system
	 * @param textureID Id of the texture 0 .. 13
	 */
	public static void activeTexture(final int textureID) {
		if (programId >= 0) {
			GL13.glActiveTexture(TEXTURE_ID_BINDING[textureID]);
			checkGlError("glActiveTexture");
		} else if (DEBUG) {
			Log.error("try to bind texture with no program set");
		}
	}
	
	public static void bindBuffer(final int bufferId) {
		GL20.glBindBuffer(GL20.GL_ARRAY_BUFFER, bufferId);
		checkGlError("glBindBuffer");
	}
	
	public static void bindTexture2D(final int texId) {
		GL11.glBindTexture(GL11.GL_TEXTURE_2D, texId);
	}
	
	public static void blendFuncAuto() {
		GL40.glBlendFunc(GL11.GL_SRC_ALPHA, GL11.GL_ONE_MINUS_SRC_ALPHA);
	}
	
	public static void bufferData(final Color[] data, final Usage usage) {
		final FloatBuffer buffer = storeDataInFloatBuffer(data);
		GL15.glBufferData(GL15.GL_ARRAY_BUFFER, buffer, convertUsage.get(usage));
		checkGlError("glBufferData");
	}
	
	public static void bufferData(final float[] data, final Usage usage) {
		final FloatBuffer buffer = storeDataInFloatBuffer(data);
		GL15.glBufferData(GL15.GL_ARRAY_BUFFER, buffer, convertUsage.get(usage));
		checkGlError("glBufferData");
	}
	
	public static void bufferData(final int[] data, final Usage usage) {
		final IntBuffer buffer = storeDataInFloatBuffer(data);
		GL15.glBufferData(GL15.GL_ARRAY_BUFFER, buffer, convertUsage.get(usage));
		checkGlError("glBufferData");
	}
	
	public static void bufferData(final Vector2f[] data, final Usage usage) {
		final FloatBuffer buffer = storeDataInFloatBuffer(data);
		GL15.glBufferData(GL15.GL_ARRAY_BUFFER, buffer, convertUsage.get(usage));
		checkGlError("glBufferData");
	}
	
	public static void bufferData(final Vector3f[] data, final Usage usage) {
		final FloatBuffer buffer = storeDataInFloatBuffer(data);
		GL15.glBufferData(GL15.GL_ARRAY_BUFFER, buffer, convertUsage.get(usage));
		checkGlError("glBufferData");
	}
	
	public static void checkGlError(final String op) {
		//int localLine = Thread.currentThread().getStackTrace()[2].getLineNumber();
		//		if (CHECKERROROPENGL) {
		//			boolean hasError = false;
		//			for (int error = GL11.glGetError(); error != null; error = GL11.glGetError()) {
		//				Log.error("after " + op + "():" + localLine + " glError(" + error + ")");
		//				hasError = true;
		//			}
		//			if (hasError) {
		//				Log.critical("plop");
		//			}
		//			for (GLint error = glGetError(); error; error = glGetError()) {
		//				Log.error("after " + op + "() glError (" + error + ")");
		//			}
		//		}
	}
	
	/**
	 * clear sets the bitplane area of the window to values previously
	 *        selected by clearColor, clearDepth, and clearStencil. Multiple color
	 *        buffers can be cleared simultaneously by selecting more than one
	 *        buffer at a time using drawBuffer. The pixel ownership test, the
	 *        scissor test, dithering, and the buffer writemasks affect the
	 *        operation of clear. The scissor box bounds the cleared region. Alpha
	 *        function, blend function, logical operation, stenciling, texture
	 *        mapping, and depth-buffering are ignored by clear.
	 * @param flag This is the bitwise OR of several values indicating which buffer
	 *              is to be cleared.
	 */
	public static void clear(final ClearFlag flag) {
		GL11.glClear(BASIC_FLAG_CLEAR.get(flag));
		checkGlError("glClear");
	}
	
	/**
	 *  Specifies the clear color value When clear is requested
	 * @param color cleear color of the screen
	 */
	public static void clearColor(final Color color) {
		GL11.glClearColor(color.r(), color.g(), color.b(), color.a());
		checkGlError("glClearColor");
	}
	
	/**
	 *  Specifies the depth value used when the depth buffer is cleared. The
	 *        initial value is 1.
	 * @param value to set [0..1]
	 */
	public static void clearDepth(final float value) {
		GL11.glClearDepth(value);
		checkGlError("glClearDepth");
	}
	
	private static void clearFlagState() {
		for (final Map.Entry<Flag, StateFlag> elem : FLAGS_STATES.entrySet()) {
			elem.getValue().current = false;
			elem.getValue().mustBeSet = false;
		}
	}
	
	/**
	 *  Specifies the index used by clear to clear the stencil buffer. s is
	 *        masked with 2 m - 1 , where m is the number of bits in the stencil
	 *        buffer.
	 * @param value
	 */
	public static void clearStencil(final int value) {
		GL11.glClearStencil(value);
		checkGlError("glClearStencil");
	}
	
	public static boolean deleteBuffers(final int[] buffers) {
		if (buffers.length == 0) {
			Log.warning("try to delete vector buffer with size 0");
			return true;
		}
		// TODO Check if we are in the correct thread
		GL20.glDeleteBuffers(buffers);
		checkGlError("glDeleteBuffers");
		for (int iii = 0; iii < buffers.length; iii++) {
			buffers[iii] = -1;
		}
		return true;
	}
	
	/**
	 *  disable Texture on the system
	 * @param flagID The flag requested
	 */
	// TODO rename Disable
	public static void desActiveTexture(final int flagID) {
		// if (this.programId >= 0) {
		//
		// }
	}
	
	/**
	 *  disable a flag on the system
	 * @param flagID The flag requested
	 */
	public static void disable(final Flag flagID) {
		// Log.info("Disable : " + flagID);
		if (DIRECT_MODE) {
			GL11.glDisable(BASIC_FLAG.get(flagID));
			checkGlError("glDisable");
		} else {
			// Log.debug("Disable FLAGS = " + this.flagsStates);
			FLAGS_STATES.get(flagID).mustBeSet = false;
			flagsStatesChange = true;
			// Log.debug(" == >" + this.flagsStates);
		}
	}
	
	/**
	 *  draw a specific array == > this enable mode difference ...
	 */
	public static void drawArrays(final RenderMode mode, final int first, final int count) {
		if (programId >= 0) {
			updateAllFlags();
			GL20.glDrawArrays(CONVERT_RENDER_MODE.get(mode), first, count);
			checkGlError("glDrawArrays");
		}
	}
	
	public static void drawElements(final RenderMode mode, final int vertexCount) {
		GL11.glDrawElements(CONVERT_RENDER_MODE.get(mode), vertexCount, GL11.GL_UNSIGNED_INT, 0);
	}
	
	public static void drawTriangleRed(final Vector3f aaa, final Vector3f bbb, final Vector3f ccc) {
		GL11.glFinish();
	}
	
	/**
	 *  enable a flag on the system
	 * @param flagID The flag requested
	 */
	public static void enable(final Flag flagID) {
		// Log.info("Enable : " + flagID);
		if (DIRECT_MODE) {
			GL11.glEnable(BASIC_FLAG.get(flagID));
			checkGlError("glEnable");
		} else {
			// Log.debug("Enable FLAGS = " + this.flagsStates);
			FLAGS_STATES.get(flagID).mustBeSet = true;
			flagsStatesChange = true;
			// Log.debug(" == >" + this.flagsStates);
		}
	}
	
	/**
	 * 
	 */
	public static void finish() {
		programId = -1;
	}
	
	/**
	 * 
	 */
	public static void flush() {
		programId = -1;
		GL11.glFlush();
		// checkGlError("glFlush");
		// Log.info("========================" );
		// Log.info("== FLUSH OPEN GL ==" );
		// Log.info("========================");
	}
	
	public static int genBuffers() {
		return GL15.glGenBuffers();
	}
	
	public static boolean genBuffers(final int[] buffers) {
		if (buffers.length == 0) {
			Log.warning("try to generate vector buffer with size 0");
			return true;
		}
		Log.info("Create N=" + buffers.length + " Buffer");
		GL20.glGenBuffers(buffers);
		checkGlError("glGenBuffers");
		boolean hasError = false;
		for (int iii = 0; iii < buffers.length; iii++) {
			if (buffers[iii] == 0) {
				Log.error("[" + iii + "] error to create a buffer id=" + buffers[iii]);
				hasError = true;
			}
		}
		return hasError;
	}
	
	/**
	 *  get a reference on the current matrix camera destinate to opengl
	 *        renderer.
	 * @return The requested matrix.
	 */
	public static Matrix4f getCameraMatrix() {
		return matrixCamera;
	}
	
	/**
	 *  get a reference on the current matrix destinate to opengl renderer.
	 * @return The requested matrix.
	 */
	public static Matrix4f getMatrix() {
		if (MATRIX_LIST.size() == 0) {
			Log.error("set matrix list is not corect size in the stack: 0");
			MATRIX_LIST.add(Matrix4f.IDENTITY);
		}
		return MATRIX_LIST.get(MATRIX_LIST.size() - 1);
	}
	
	public static void glDeleteTextures(final int textureId) {
		GL11.glDeleteTextures(textureId);
	}
	
	public static int glGenTextures() {
		return GL11.glGenTextures();
	}
	
	public static void glTexImage2D(final int level, final int internalFormat, final int width, final int height, final int border, final int format, final int sizeObject, final byte[] data) {
		final ByteBuffer dataBuffer = ByteBuffer.allocateDirect(data.length);
		for (int iii = 0; iii < data.length; iii++) {
			dataBuffer.put(data[iii]);
		}
		dataBuffer.flip();
		GL11.glTexImage2D(GL11.GL_TEXTURE_2D, level, internalFormat, width, height, border, format, sizeObject, dataBuffer);
	}
	
	public static void glTexImage2D(final int level, final int internalFormat, final int width, final int height, final int border, final int format, final int sizeObject, final ByteBuffer data) {
		GL11.glTexImage2D(GL11.GL_TEXTURE_2D, level, internalFormat, width, height, border, format, sizeObject, data);
	}
	
	public static void glTexSubImage2D(final int level, final int xOffset, final int yOffset, final int width, final int height, final int format, final int sizeObject, final byte[] data) {
		final ByteBuffer dataBuffer = ByteBuffer.allocateDirect(data.length);
		for (int iii = 0; iii < data.length; iii++) {
			dataBuffer.put(data[iii]);
		}
		dataBuffer.flip();
		GL11.glTexSubImage2D(GL11.GL_TEXTURE_2D, level, xOffset, yOffset, width, height, format, sizeObject, dataBuffer);
	}
	
	public static void glTexSubImage2D(final int level, final int xOffset, final int yOffset, final int width, final int height, final int format, final int sizeObject, final ByteBuffer data) {
		GL11.glTexSubImage2D(GL11.GL_TEXTURE_2D, level, xOffset, yOffset, width, height, format, sizeObject, data);
	}
	
	/**
	 *  Get the current thread context status.
	 * @return true The threflagsStates.putn acces to
	 *         openGL.flagsStates.put(Flag.@return false The Thread Can not acces to
	 *         OpenGL.
	 */
	public static boolean hasContext() {
		final long curentThreadId = Thread.currentThread().getId();
		if (!THREAD_HAS_CONTEXT.containsKey(curentThreadId)) {
			return false;
		}
		return THREAD_HAS_CONTEXT.get(curentThreadId);
	}
	
	/**
	 *  Lock the openGL context for one user only == > better to keep flags
	 *        and other things ...
	 */
	public static void lock() {
		// mutexOpenGl().lock();
		MATRIX_LIST.clear();
		final Matrix4f tmpMat = Matrix4f.IDENTITY;
		MATRIX_LIST.add(tmpMat);
		matrixCamera = Matrix4f.IDENTITY;
		clearFlagState();
		programId = -1;
	}
	
	/**
	 *  remove the current matrix and get the last one from the matrix stack.
	 */
	public static void pop() {
		Log.verbose("Pop OpenGl Matrix: " + MATRIX_LIST.size());
		if (MATRIX_LIST.size() <= 1) {
			Log.error("set matrix list is not corect size in the stack : " + MATRIX_LIST.size());
			MATRIX_LIST.clear();
			MATRIX_LIST.add(Matrix4f.IDENTITY);
			matrixCamera = Matrix4f.IDENTITY;
			return;
		}
		MATRIX_LIST.remove(MATRIX_LIST.size() - 1);
		matrixCamera = Matrix4f.IDENTITY;
	}
	
	public static boolean programAttach(final int prog, final int shader) {
		if (prog < 0) {
			Log.error("wrong program ID");
			return false;
		}
		if (shader < 0) {
			Log.error("wrong shader ID");
			return false;
		}
		GL20.glAttachShader(prog, shader);
		checkGlError("glAttachShader");
		return true;
	}
	
	public static void programBindAttribute(final int prog, final int attribute, final String variableName) {
		if (prog < 0) {
			Log.error("wrong program ID");
			return;
		}
		GL20.glBindAttribLocation(prog, attribute, variableName);
	}
	
	public static boolean programCompile(final int prog) {
		if (prog < 0) {
			Log.error("wrong program ID");
			return false;
		}
		GL20.glLinkProgram(prog);
		checkGlError("glLinkProgram");
		GL20.glValidateProgram(prog);
		
		// GLint linkStatus = GLFALSE;
		// glGetProgramiv(GLint(prog), GLLINKSTATUS, linkStatus);
		// checkGlError("glGetProgramiv");
		// if (linkStatus != GLTRUE) {
		// GLint bufLength = 0;
		// this.bufferDisplayError[0] = '\0';
		// glGetProgramInfoLog(GLint(prog), LOGOGLINTERNALBUFFERLEN, bufLength,
		// this.bufferDisplayError);
		// char tmpLog[256];
		// int idOut=0;
		// Log.error("Could not compile 'PROGRAM':");
		// for (sizet iii=0; iii<LOGOGLINTERNALBUFFERLEN ; iii++) {
		// tmpLog[idOut] = this.bufferDisplayError[iii];
		// if ( tmpLog[idOut] == '\n'
		// || tmpLog[idOut] == '\0'
		// || idOut >= 256) {
		// tmpLog[idOut] = '\0';
		// Log.error(" == > " + tmpLog);
		// idOut=0;
		// } else {
		// idOut++;
		// }
		// if (this.bufferDisplayError[iii] == '\0') {
		// break;
		// }
		// }
		// if (idOut != 0) {
		// tmpLog[idOut] = '\0';
		// Log.error(" == > " + tmpLog);
		// }
		// return false;
		// }
		return true;
	}
	
	// ------------------------------------------------------------------------------------
	// -- Open GL program ...
	// ------------------------------------------------------------------------------------
	public static int programCreate() {
		final int programId = GL20.glCreateProgram();
		if (programId == 0) {
			Log.error("program creation return error ...");
			checkGlError("glCreateProgram");
			return -1;
		}
		Log.debug("Create program with oglID=" + programId);
		return programId;
	}
	
	public static boolean programDetach(final int prog, final int shader) {
		if (prog < 0) {
			Log.error("wrong program ID");
			return false;
		}
		if (shader < 0) {
			Log.error("wrong shader ID");
			return false;
		}
		GL20.glDetachShader(prog, shader);
		checkGlError("glDetachShader");
		return true;
	}
	
	public static int programGetAttributeLocation(final int prog, final String name) {
		if (prog < 0) {
			Log.error("wrong program ID");
			return -1;
		}
		if (name.length() == 0) {
			Log.error("wrong name of attribure");
			return -1;
		}
		final int val = GL20.glGetAttribLocation(prog, name);
		if (val < 0) {
			checkGlError("glGetAttribLocation");
			Log.warning("glGetAttribLocation('" + name + "') = " + val);
			return -1;
		}
		return val;
	}
	
	public static int programGetUniformLocation(final int prog, final String name) {
		if (prog < 0) {
			Log.error("wrong program ID");
			return -1;
		}
		if (name.length() == 0) {
			Log.error("wrong name of uniform");
			return -1;
		}
		final int val = GL20.glGetUniformLocation(prog, name);
		if (val == GL20.GL_INVALID_VALUE) {
			checkGlError("glGetUniformLocation");
			Log.warning("glGetUniformLocation('" + name + "') = GL_INVALID_VALUE");
		} else if (val == GL20.GL_INVALID_OPERATION) {
			checkGlError("glGetUniformLocation");
			Log.warning("glGetUniformLocation('" + name + "') = GL_INVALID_OPERATION");
		} else if (val < 0) {
			checkGlError("glGetUniformLocation");
			Log.warning("glGetUniformLocation('" + name + "') = " + val);
		}
		return val;
	}
	
	public static void programLoadUniformBoolean(final int location, final boolean value) {
		// System.out.println("set value " + value + " " + (value==true?1.0f:0.0f));
		GL20.glUniform1f(location, value ? 1.0f : 0.0f);
	}
	
	public static void programLoadUniformColor(final int location, final Color value) {
		GL20.glUniform4f(location, value.r(), value.g(), value.b(), value.a());
	}
	
	public static void programLoadUniformFloat(final int location, final float value) {
		GL20.glUniform1f(location, value);
	}
	
	public static void programLoadUniformFloat(final int location, final float value, final float value2) {
		GL20.glUniform2f(location, value, value2);
	}
	
	public static void programLoadUniformFloat(final int location, final float value, final float value2, final float value3) {
		GL20.glUniform3f(location, value, value2, value3);
	}
	
	public static void programLoadUniformFloat(final int location, final float value, final float value2, final float value3, final float value4) {
		GL20.glUniform4f(location, value, value2, value3, value4);
	}
	
	public static void programLoadUniformInt(final int location, final int value) {
		GL20.glUniform1i(location, value);
	}
	
	public static void programLoadUniformInt(final int location, final int value, final int value2) {
		GL20.glUniform2i(location, value, value2);
	}
	
	public static void programLoadUniformInt(final int location, final int value, final int value2, final int value3) {
		GL20.glUniform3i(location, value, value2, value3);
	}
	
	public static void programLoadUniformInt(final int location, final int value, final int value2, final int value3, final int value4) {
		GL20.glUniform4i(location, value, value2, value3, value4);
	};
	
	public static void programLoadUniformMatrix(final int location, final Matrix4f value) {
		GL20.glUniformMatrix4fv(location, true, value.asArray());
	}
	
	public static void programLoadUniformMatrix(final int location, final Matrix4f value, final boolean transpose) {
		GL20.glUniformMatrix4fv(location, transpose, value.asArray());
	}
	
	public static void programLoadUniformVector(final int location, final Vector2f value) {
		GL20.glUniform2f(location, value.x(), value.y());
	}
	
	public static void programLoadUniformVector(final int location, final Vector2i value) {
		GL20.glUniform2i(location, value.x(), value.y());
	}
	
	public static void programLoadUniformVector(final int location, final Vector3f value) {
		GL20.glUniform3f(location, value.x(), value.y(), value.z());
	}
	
	public static void programLoadUniformVector(final int location, final Vector3i value) {
		GL20.glUniform3i(location, value.x(), value.y(), value.z());
	}
	
	public static void programRemove(final int prog) {
		if (prog < 0) {
			return;
		}
		// TODO Check if we are in the correct thread
		GL20.glDeleteProgram(prog);
		checkGlError("glDeleteProgram");
	}
	
	public static void programUnUse(final int id) {
		// nothing to do ...
	}
	
	// public static void drawElements(RenderMode mode, List<Integer> indices) {
	// if (this.programId >= 0) {
	// updateAllFlags();
	// //Log.debug("Request draw of " + indices.size() + "elements");
	// GL15.glDrawElements(convertRenderMode.get(mode), indices.size(),
	// GL11.GL_UNSIGNED_INT);//, &indices[0]);
	// checkGlError("glDrawElements");
	// }
	// }
	// public static void drawElements16(RenderMode mode, List<int> indices) {
	// if (this.programId >= 0) {
	// updateAllFlags();
	// GL20.glDrawElements(convertRenderMode.get(mode), indices.size(),
	// GLUNSIGNEDSHORT, &indices[0]);
	// checkGlError("glDrawElements");
	// }
	// }
	// public static void drawElements8 (enum renderMode mode, List<int> indices) {
	// if (this.programId >= 0) {
	// updateAllFlags();
	// GL20.glDrawElements(convertRenderMode[int(mode)], indices.size(),
	// GLUNSIGNEDBYTE, &indices[0]);
	// checkGlError("glDrawElements");
	// }
	// }
	/**
	 *  Use openGL program
	 * @param id Id of the program that might be used
	 */
	public static void programUse(final int id) {
		// Log.verbose("USE prog : " + id);
		// note : In normal openGL case, the system might call with the program ID and
		// at the end with 0,
		// here, we wrap this use to prevent over call of glUseProgram == > then we set
		// -1 when the
		// user no more use this program, and just stop grnerating. (chen 0 == > this is
		// an errored program ...
		if (id == -1) {
			// not used == > because it is unneded
			return;
		}
		if (programId != id) {
			programId = id;
			GL20.glUseProgram(programId);
		}
		checkGlError("glUseProgram");
	}
	
	/**
	 *  store current matrix in the matrix stack.
	 */
	public static void push() {
		Log.verbose("push OpenGl Matrix: " + MATRIX_LIST.size());
		if (MATRIX_LIST.size() == 0) {
			Log.error("set matrix list is not corect size in the stack : " + MATRIX_LIST.size());
			MATRIX_LIST.add(Matrix4f.IDENTITY);
			return;
		}
		final Matrix4f tmp = MATRIX_LIST.get(MATRIX_LIST.size() - 1);
		MATRIX_LIST.add(tmp);
	}
	
	protected static StringBuilder readLocalFile(final String name) {
		final StringBuilder fileSource = new StringBuilder();
		try {
			final BufferedReader reader = new BufferedReader(new FileReader(name));
			String line;
			while ((line = reader.readLine()) != null) {
				fileSource.append(line).append("\n");
			}
			reader.close();
		} catch (final IOException e) {
			Log.critical("Could not read the file!");
		}
		return fileSource;
	}
	
	protected static StringBuilder readLocalFile(final Uri name) {
		final StringBuilder fileSource = new StringBuilder();
		try {
			final InputStream inputStream = Uri.getStream(name);
			if (inputStream == null) {
				Log.critical("Could not read the file! " + name);
			}
			final Reader reader = new BufferedReader(new InputStreamReader(inputStream, Charset.forName(StandardCharsets.UTF_8.name())));
			int c = 0;
			while ((c = reader.read()) != -1) {
				fileSource.append((char) c);
			}
		} catch (final IOException e) {
			Log.error("Could not read the file! " + name);
			e.printStackTrace();
			System.exit(-1);
		}
		return fileSource;
	}
	
	public static void reset() {
		if (DIRECT_MODE) {
			Log.error("TODO ...");
		} else {
			clearFlagState();
			programId = -1;
			updateAllFlags();
		}
	};
	
	public static void resetFlagState() {
		for (final Map.Entry<Flag, StateFlag> elem : FLAGS_STATES.entrySet()) {
			elem.getValue().mustBeSet = false;
		}
		flagsStatesChange = true;
	}
	
	/**
	 *  When you will done an opengl rendering, you might call this reset
	 *        matrix first. It remove all the stach of the matrix pushed.
	 * @param newOne the default matrix that might be set for the graphic card for
	 *               renderer. if too more pop will be done, this is the last that
	 *               mmight survived
	 */
	public static void setBasicMatrix(final Matrix4f newOne) {
		if (MATRIX_LIST.size() != 1) {
			Log.error("matrix is not corect size in the stack : " + MATRIX_LIST.size());
		}
		MATRIX_LIST.clear();
		MATRIX_LIST.add(newOne);
	}
	
	/**
	 *  set a reference on the current camera to opengl renderer.
	 * @param newOne The requested matrix.
	 */
	public static void setCameraMatrix(final Matrix4f newOne) {
		matrixCamera = newOne;
	}
	
	public static void setDeathMask(final boolean state) {
		GL40.glDepthMask(state);
	}
	
	/**
	 *  this funtion configure the current use matrix for the renderer
	 *        (call @ref Push before, and @ref Pop when no more needed).
	 * @param newOne The new current matrix use for the render.
	 * @note We did not use opengl standard system, due to the fact that is not
	 *       supported in opengl ES-2
	 */
	public static void setMatrix(final Matrix4f newOne) {
		if (MATRIX_LIST.size() == 0) {
			Log.error("set matrix list is not corect size in the stack : " + MATRIX_LIST.size());
			MATRIX_LIST.add(newOne);
			return;
		}
		MATRIX_LIST.set(MATRIX_LIST.size() - 1, newOne);
	}
	
	public static void setTexture2DFilterLinear() {
		GL11.glTexParameteri(GL11.GL_TEXTURE_2D, GL11.GL_TEXTURE_MAG_FILTER, GL11.GL_LINEAR);
		GL11.glTexParameteri(GL11.GL_TEXTURE_2D, GL11.GL_TEXTURE_MIN_FILTER, GL11.GL_LINEAR);
	}
	
	public static void setTexture2DFilterNearest() {
		GL11.glTexParameteri(GL11.GL_TEXTURE_2D, GL11.GL_TEXTURE_MAG_FILTER, GL11.GL_NEAREST);
		GL11.glTexParameteri(GL11.GL_TEXTURE_2D, GL11.GL_TEXTURE_MIN_FILTER, GL11.GL_NEAREST);
	}
	
	public static void setTexture2DWrapClampToEdge() {
		GL11.glTexParameteri(GL11.GL_TEXTURE_2D, GL11.GL_TEXTURE_WRAP_S, GL40.GL_CLAMP_TO_EDGE);
		GL11.glTexParameteri(GL11.GL_TEXTURE_2D, GL11.GL_TEXTURE_WRAP_T, GL40.GL_CLAMP_TO_EDGE);
	}
	
	public static void setTexture2DWrapRepeat() {
		GL11.glTexParameteri(OpenGL.GL_TEXTURE_2D, GL11.GL_TEXTURE_WRAP_S, GL11.GL_REPEAT);
		GL11.glTexParameteri(OpenGL.GL_TEXTURE_2D, GL11.GL_TEXTURE_WRAP_T, GL11.GL_REPEAT);
	}
	
	public static void setViewPort(final Vector2f start, final Vector2f stop) {
		// Log.info("setViewport " + start + " " + stop);
		GL11.glViewport((int) start.x(), (int) start.y(), (int) stop.x(), (int) stop.y());
		checkGlError("glViewport");
	}
	
	public static void setViewPort(final Vector2i start, final Vector2i stop) {
		// Log.info("setViewport " + start + " " + stop);
		GL11.glViewport(start.x(), start.y(), stop.x(), stop.y());
		checkGlError("glViewport");
	}
	
	private static int shaderCreate(final ShaderType type) {
		int shaderId = 0;
		if (type == ShaderType.vertex) {
			Log.verbose("create shader: VERTEX");
			shaderId = GL20.glCreateShader(GL20.GL_VERTEX_SHADER);
		} else if (type == ShaderType.fragment) {
			Log.verbose("create shader: FRAGMENT");
			shaderId = GL20.glCreateShader(GL20.GL_FRAGMENT_SHADER);
		} else {
			Log.error("create shader with wrong type ...");
			return -1;
		}
		Log.verbose("create shader: ... (done)");
		if (shaderId == 0) {
			Log.error("glCreateShader return error ...");
			checkGlError("glCreateShader");
			return -1;
		}
		return shaderId;
	}
	
	public static int shaderLoad(final Uri file, final ShaderType type) {
		System.out.println("Load shader: '" + file + "'");
		final StringBuilder shaderSource = readLocalFile(file);
		final int shaderID = shaderCreate(type);
		GL20.glShaderSource(shaderID, shaderSource);
		GL20.glCompileShader(shaderID);
		if (GL20.glGetShaderi(shaderID, GL20.GL_COMPILE_STATUS) == GL11.GL_FALSE) {
			Log.error(GL20.glGetShaderInfoLog(shaderID, 500));
			Log.error("Could not compile the shader");
			return -1;
		}
		return shaderID;
	}
	
	public static void shaderRemove(final int shader) {
		if (shader < 0) {
			return;
		}
		// TODO Check if we are in the correct thread
		GL20.glDeleteShader(shader);
		checkGlError("glDeleteShader");
	}
	
	public static FloatBuffer storeDataInFloatBuffer(final Color[] data) {
		float[] tmpData = new float[data.length * 4];
		for (int iii = 0; iii < data.length; iii++) {
			tmpData[iii * 4] = data[iii].r();
			tmpData[iii * 4 + 1] = data[iii].g();
			tmpData[iii * 4 + 2] = data[iii].b();
			tmpData[iii * 4 + 3] = data[iii].a();
		}
		return storeDataInFloatBuffer(tmpData);
		// does not work...
		/*
		final FloatBuffer buffer = FloatBuffer.allocate(data.length * 4);
		for (int iii = 0; iii < data.length; iii++) {
			buffer.put(iii * 4, data[iii].r());
			buffer.put(iii * 4 + 1, data[iii].g());
			buffer.put(iii * 4 + 2, data[iii].b());
			buffer.put(iii * 4 + 3, data[iii].a());
		}
		buffer.flip();
		return buffer;
		*/
	}
	
	private static FloatBuffer storeDataInFloatBuffer(final float[] data) {
		final FloatBuffer buffer = BufferUtils.createFloatBuffer(data.length);
		buffer.put(data);
		buffer.flip();
		return buffer;
	}
	
	private static IntBuffer storeDataInFloatBuffer(final int[] data) {
		final IntBuffer buffer = BufferUtils.createIntBuffer(data.length);
		buffer.put(data);
		buffer.flip();
		return buffer;
	}
	
	public static FloatBuffer storeDataInFloatBuffer(final Vector2f[] data) {
		float[] tmpData = new float[data.length * 2];
		for (int iii = 0; iii < data.length; iii++) {
			tmpData[iii * 2] = data[iii].x();
			tmpData[iii * 2 + 1] = data[iii].y();
		}
		return storeDataInFloatBuffer(tmpData);
		// does not work...
		/*
		final FloatBuffer buffer = FloatBuffer.allocate(data.length * 2);
		for (int iii = 0; iii < data.length; iii++) {
			buffer.put(iii * 2, data[iii].x());
			buffer.put(iii * 2 + 1, data[iii].y());
		}
		buffer.flip();
		return buffer;
		*/
	}
	
	public static FloatBuffer storeDataInFloatBuffer(final Vector3f[] data) {
		float[] tmpData = new float[data.length * 3];
		for (int iii = 0; iii < data.length; iii++) {
			tmpData[iii * 3] = data[iii].x();
			tmpData[iii * 3 + 1] = data[iii].y();
			tmpData[iii * 3 + 2] = data[iii].z();
		}
		return storeDataInFloatBuffer(tmpData);
		// does not work...
		/*
		final FloatBuffer buffer = FloatBuffer.allocate(data.length * 3);
		for (int iii = 0; iii < data.length; iii++) {
			buffer.put(data[iii].x());
			buffer.put(data[iii].y());
			buffer.put(data[iii].z());
		}
		//buffer.flip();
		//buffer.limit(data.length * 3);
		//return buffer.asReadOnlyBuffer();
		return buffer;
		*/
	}
	
	/**
	 * 
	 */
	public static void swap() {
		
	}
	
	/**
	 *  must be called by the thread that has openGl context to notify the
	 *        system
	 * @note Call @ref gale::openGL::threadHasNoMoreContext when ended
	 */
	public static void threadHasContext() {
		final long curentThreadId = Thread.currentThread().getId();
		THREAD_HAS_CONTEXT.put(curentThreadId, true);
	}
	
	/**
	 *  At the end of the thread exection, set the thead has no more openGL
	 *        cotext
	 */
	public static void threadHasNoMoreContext() {
		final long curentThreadId = Thread.currentThread().getId();
		THREAD_HAS_CONTEXT.remove(curentThreadId);
	}
	
	public static boolean unbindBuffer() {
		GL20.glBindBuffer(GL20.GL_ARRAY_BUFFER, 0);
		checkGlError("glBindBuffer(0)");
		return true;
	}
	
	/**
	 *  Un-lock the openGL context for an other user...
	 */
	public static void unLock() {
		// mutexOpenGl().unLock();
	}
	
	/**
	 * @brieg update all the internal flag needed to be set from tre previous
	 *        element set ...
	 */
	public static void updateAllFlags() {
		if (DIRECT_MODE) {
			return;
		}
		// check if flags has change :
		if (!flagsStatesChange) {
			return;
		}
		flagsStatesChange = false;
		for (final Map.Entry<Flag, StateFlag> elem : FLAGS_STATES.entrySet()) {
			final StateFlag value = elem.getValue();
			if (value.current != value.mustBeSet) {
				value.current = value.mustBeSet;
				if (value.current) {
					GL11.glEnable(BASIC_FLAG.get(elem.getKey()));
					checkGlError("glEnable");
					// Log.info(" enable : " + elem.getKey() + " " + basicFlag.get(elem.getKey()));
				} else {
					GL11.glDisable(BASIC_FLAG.get(elem.getKey()));
					checkGlError("glDisable");
					// Log.info(" disable : " + elem.getKey());
				}
			}
		}
	}
	
	public static void vertexAttribPointerFloat(final int id, final int size) {
		GL20.glVertexAttribPointer(id, size, GL11.GL_FLOAT, false, 0, 0);
		checkGlError("glVertexAttribPointer");
	}
	
	private OpenGL() {}
	
}
