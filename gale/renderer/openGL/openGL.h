/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license APACHE v2.0 (see license file)
 */

#ifndef __OPEN_GL_H__
#define __OPEN_GL_H__

#include <etk/types.h>
#include <vector>
#include <etk/math/Matrix4.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__TARGET_OS__Linux)
	// TO ENABLE THE SHADER api ...
	#define GL_GLEXT_PROTOTYPES
	#include <GL/gl.h>
	// TODO : Check it it work
	// This is to prevent the use of these element that is not allowed in the openGL ES
	#undef glVertexPointer
	#undef glTexCoordPointer
	#undef glColorPointer
	#undef glPopMatrix
	#undef glPushMatrix
	#undef glMatrixMode
	#undef glLoadIdentity
	#undef glTranslatef
#elif defined(__TARGET_OS__Android)
	// Include openGL ES 2
	#include <GLES2/gl2.h>
	#include <GLES2/gl2ext.h>
#elif defined(__TARGET_OS__Windows)
	// TO ENABLE THE SHADER api ...
	//#define GL_GLEXT_PROTOTYPES
	#define GLEW_STATIC
	#include <GL/glew.h>
#elif defined(__TARGET_OS__MacOs)
	#include <OpenGL/gl.h>
	#include <OpenGL/glu.h>
#elif defined(__TARGET_OS__IOs)
	#include <OpenGLES/ES2/gl.h>
#else
	#error you need to specify a __TAGET_OS__ ...
#endif

#ifdef __cplusplus
}
#endif

namespace gale {
	namespace openGL {
		/**
		 * @brief Lock the openGL context for one user only  == > better to keep flags and other things ...
		 */
		void lock();
		/**
		 * @brief Un-lock the openGL context for an other user...
		 */
		void unLock();
		/**
		 * @brief When you will done an opengl rendering, you might call this reset matrix first. It remove all the stach of the matrix pushed.
		 * @param[in] _newOne the default matrix that might be set for the graphic card for renderer. if too more pop will be done, this is the last that mmight survived
		 */
		void setBasicMatrix(const mat4& _newOne);
		/**
		 * @brief this funtion configure the current use matrix for the renderer (call @ref Push before, and @ref Pop when no more needed).
		 * @param[in] _newOne The new current matrix use for the render.
		 * @note We did not use opengl standard system, due to the fact that is not supported in opengl ES-2
		 */
		void setMatrix(const mat4& _newOne);
		/**
		 * @brief store current matrix in the matrix stack.
		 */
		void push();
		/**
		 * @brief remove the current matrix and get the last one from the matrix stack.
		 */
		void pop();
		/**
		 * @brief get a reference on the current matrix destinate to opengl renderer.
		 * @return The requested matrix.
		 */
		const mat4& getMatrix();
		/**
		 * @brief get a reference on the current matrix camera destinate to opengl renderer.
		 * @return The requested matrix.
		 */
		const mat4& getCameraMatrix();
		/**
		 * @brief set a reference on the current camera to opengl renderer.
		 * @param[in] _newOne The requested matrix.
		 */
		void setCameraMatrix(const mat4& _newOne);
		/**
		 * @brief
		 */
		void finish();
		/**
		 * @brief
		 */
		void flush();
		/**
		 * @brief
		 */
		void swap();
		
		enum openGlFlags {
			FLAG_BLEND = 1<<0, //!< If enabled, blend the computed fragment color values with the values in the color buffers. See glBlendFunc.
			FLAG_CLIP_DISTANCE_I = 1<<1, //!< If enabled, clip geometry against user-defined half space i.
			FLAG_COLOR_LOGIC_OP = 1<<2, //!< If enabled, apply the currently selected logical operation to the computed fragment color and color buffer values. See glLogicOp.
			FLAG_CULL_FACE = 1<<3, //!< If enabled, cull polygons based on their winding in window coordinates. See glCullFace.
			FLAG_DEBUG_OUTPUT = 1<<4, //!< If enabled, debug messages are produced by a debug context. When disabled, the debug message log is silenced. Note that in a non-debug context, very few, if any messages might be produced, even when GL_DEBUG_OUTPUT is enabled.
			FLAG_DEBUG_OUTPUT_SYNCHRONOUS = 1<<5, //!< If enabled, debug messages are produced synchronously by a debug context. If disabled, debug messages may be produced asynchronously. In particular, they may be delayed relative to the execution of GL commands, and the debug callback function may be called from a thread other than that in which the commands are executed. See glDebugMessageCallback.
			FLAG_DEPTH_CLAMP = 1<<6, //!< If enabled, the -wc≤zc≤wc plane equation is ignored by view volume clipping (effectively, there is no near or far plane clipping). See glDepthRange.
			FLAG_DEPTH_TEST = 1<<7, //!< If enabled, do depth comparisons and update the depth buffer. Note that even if the depth buffer exists and the depth mask is non-zero, the depth buffer is not updated if the depth test is disabled. See glDepthFunc and glDepthRange.
			FLAG_DITHER = 1<<8, //!< If enabled, dither color components or indices before they are written to the color buffer.
			FLAG_FRAMEBUFFER_SRGB = 1<<9, //!< If enabled and the value of GL_FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING for the framebuffer attachment corresponding to the destination buffer is GL_SRGB, the R, G, and B destination color values (after conversion from fixed-point to floating-point) are considered to be encoded for the sRGB color space and hence are linearized prior to their use in blending.
			FLAG_LINE_SMOOTH = 1<<10, //!< If enabled, draw lines with correct filtering. Otherwise, draw aliased lines. See glLineWidth.
			FLAG_MULTISAMPLE = 1<<11, //!< If enabled, use multiple fragment samples in computing the final color of a pixel. See glSampleCoverage.
			FLAG_POLYGON_OFFSET_FILL = 1<<12, //!< If enabled, and if the polygon is rendered in GL_FILL mode, an offset is added to depth values of a polygon's fragments before the depth comparison is performed. See glPolygonOffset.
			FLAG_POLYGON_OFFSET_LINE = 1<<13, //!< If enabled, and if the polygon is rendered in GL_LINE mode, an offset is added to depth values of a polygon's fragments before the depth comparison is performed. See glPolygonOffset.
			FLAG_POLYGON_OFFSET_POINT = 1<<14, //!< If enabled, an offset is added to depth values of a polygon's fragments before the depth comparison is performed, if the polygon is rendered in GL_POINT mode. See glPolygonOffset.
			FLAG_POLYGON_SMOOTH = 1<<15, //!< If enabled, draw polygons with proper filtering. Otherwise, draw aliased polygons. For correct antialiased polygons, an alpha buffer is needed and the polygons must be sorted front to back.
			FLAG_PRIMITIVE_RESTART = 1<<16, //!< enables primitive restarting.  If enabled, any one of the draw commands which transfers a set of generic attribute array elements to the GL will restart the primitive when the index of the vertex is equal to the primitive restart index. See glPrimitiveRestartIndex.
			FLAG_PRIMITIVE_RESTART_FIXED_INDEX = 1<<17, //!< enables primitive restarting with a fixed index. If enabled, any one of the draw commands which transfers a set of generic attribute array elements to the GL will restart the primitive when the index of the vertex is equal to the fixed primitive index for the specified index type. The fixed index is equal to 2n−1 where n is equal to 8 for GL_UNSIGNED_BYTE, 16 for GL_UNSIGNED_SHORT and 32 for GL_UNSIGNED_INT.
			FLAG_SAMPLE_ALPHA_TO_COVERAGE = 1<<18, //!< If enabled, compute a temporary coverage value where each bit is determined by the alpha value at the corresponding sample location.  The temporary coverage value is then ANDed with the fragment coverage value.
			FLAG_SAMPLE_ALPHA_TO_ONE = 1<<19, //!< If enabled, each sample alpha value is replaced by the maximum representable alpha value.
			FLAG_SAMPLE_COVERAGE = 1<<20, //!< If enabled, the fragment's coverage is ANDed with the temporary coverage value. If GL_SAMPLE_COVERAGE_INVERT is set to GL_TRUE, invert the coverage value. See glSampleCoverage.
			FLAG_SAMPLE_SHADING = 1<<21, //!< If enabled, the active fragment shader is run once for each covered sample, or at fraction of this rate as determined by the current value of GL_MIN_SAMPLE_SHADING_VALUE. See glMinSampleShading.
			FLAG_SAMPLE_MASK = 1<<22, //!< If enabled, the sample coverage mask generated for a fragment during rasterization will be ANDed with the value of GL_SAMPLE_MASK_VALUE before shading occurs. See glSampleMaski.
			FLAG_SCISSOR_TEST = 1<<23, //!< If enabled, discard fragments that are outside the scissor rectangle. See glScissor.
			FLAG_STENCIL_TEST = 1<<24, //!< If enabled, do stencil testing and update the stencil buffer. See glStencilFunc and glStencilOp. GL_TEXTURE_CUBE_MAP_SEAMLESS = 1<<0, //!< If enabled, cubemap textures are sampled such that when linearly sampling from the border between two adjacent faces, texels from both faces are used to generate the final sample value. When disabled, texels from only a single face are used to construct the final sample value.
			FLAG_PROGRAM_POINT_SIZE = 1<<25, //!< If enabled and a vertex or geometry shader is active, then the derived point size is taken from the (potentially clipped) shader builtin gl_PointSize and clamped to the implementation-dependent point size range.
			FLAG_TEXTURE_2D = 1<<26, //!< 
			FLAG_ALPHA_TEST = 1<<27, //!< 
			FLAG_FOG = 1<<28, //!< 
		};
		enum renderMode {
			renderPoint = GL_POINTS,
			renderLine = GL_LINES,
			renderLineStrip = GL_LINE_STRIP, //!< Not supported in GALE (TODO : Later)
			renderLineLoop = GL_LINE_LOOP,
			renderTriangle = GL_TRIANGLES,
			renderTriangleStrip = GL_TRIANGLE_STRIP, //!< Not supported in GALE (TODO : Later)
			renderTriangleFan = GL_TRIANGLE_FAN, //!< Not supported in GALE (TODO : Later)
			#if (!defined(__TARGET_OS__IOs) && !defined(__TARGET_OS__Android))
				renderQuad = GL_QUADS, //!< Not supported in OpenGL-ES2
				renderQuadStrip = GL_QUAD_STRIP, //!< Not supported in OpenGL-ES2
				renderPolygon = GL_POLYGON //!< Not supported in OpenGL-ES2
			#else
				renderQuad, //!< Not supported in OpenGL-ES2
				renderQuadStrip, //!< Not supported in OpenGL-ES2
				renderPolygon //!< Not supported in OpenGL-ES2
			#endif
		};
		
		/**
		 * @brief enable a flag on the system
		 * @param[in] flagID The flag requested
		 */
		void enable(enum openGlFlags _flagID);
		/**
		 * @brief disable a flag on the system
		 * @param[in] flagID The flag requested
		 */
		void disable(enum openGlFlags _flagID);
		/**
		 * @brieg update all the internal flag needed to be set from tre previous element set ...
		 */
		void updateAllFlags();
		/**
		 * @brief enable Texture on the system
		 * @param[in] flagID The flag requested
		 */
		void activeTexture(uint32_t _flagID);
		/**
		 * @brief disable Texture on the system
		 * @param[in] flagID The flag requested
		 */
		void desActiveTexture(uint32_t _flagID);
		/**
		 * @brief draw a specific array  == > this enable mode difference ...
		 */
		void drawArrays(uint32_t _mode, int32_t _first, int32_t _count);
		void drawElements  (uint32_t _mode, const std::vector<uint32_t>& _indices);
		void drawElements16(uint32_t _mode, const std::vector<uint16_t>& _indices);
		void drawElements8 (uint32_t _mode, const std::vector<uint8_t>& _indices);
		/**
		 * @brief Use openGL program
		 * @param[in] id Id of the program that might be used
		 */
		void useProgram(int32_t _id);
		void reset();
		
		
		bool genBuffers(std::vector<GLuint>& _buffers);
		bool deleteBuffers(std::vector<GLuint>& _buffers);
		bool bindBuffer(GLuint _bufferId);
		bool bufferData(size_t _size, const void* _data, GLenum _usage);
		bool unbindBuffer();
	};
	std::ostream& operator <<(std::ostream& _os, const enum openGL::openGlFlags& _obj);
	std::ostream& operator <<(std::ostream& _os, const enum openGL::renderMode& _obj);
};


#endif
