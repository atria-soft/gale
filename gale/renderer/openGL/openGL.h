/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license APACHE v2.0 (see license file)
 */
#pragma once

#include <etk/types.h>
#include <vector>
#include <etk/math/Matrix4.h>
#include <etk/math/Vector2D.h>
#include <etk/Color.h>

namespace gale {
	namespace openGL {
		#ifdef GALE_SIMULATION_OPENGL_AVAILLABLE
			/**
			 * @brief Start the simulation mode of Open GL (can be usebull when no HW is availlable for testing System like buildfarm (eg: travis-ci)
			 * This generate a non access on the openGL IO, and all function work corectly.
			 */
			void startSimulationMode();
		#endif
		/**
		 * @brief Get the current thread context status.
		 * @return true The thread can acces to openGL.
		 * @return false The Thread Can not acces to OpenGL.
		 */
		bool hasContext();
		/**
		 * @brief must be called by the thread that has openGl context to notify the system
		 * @note Call @ref gale::openGL::threadHasNoMoreContext when ended
		 */
		void threadHasContext();
		/**
		 * @brief At the end of the thread exection, set the thead has no more openGL cotext
		 */
		void threadHasNoMoreContext();
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
		void setViewPort(const ivec2& _start, const ivec2& _stop);
		void setViewPort(const vec2& _start, const vec2& _stop);
		
		/**
		 * @brief Specifies the clear color value When clear is requested
		 * @param[in] _value to set [0..1]
		 */
		void clearColor(const etk::Color<float>& _color);
		/**
		 * @brief Specifies the depth value used when the depth buffer is cleared. The initial value is 1.
		 * @param[in] _value to set [0..1]
		 */
		void clearDepth(float _value);
		/**
		 * @brief Specifies the index used by clear to clear the stencil buffer. s is masked with 2 m - 1 , where m is the number of bits in the stencil buffer.
		 * @param[in] _value 
		 */
		void clearStencil(int32_t _value);
		
		enum clearFlag {
			clearFlag_colorBuffer = 1<<0, //!< Indicates the buffers currently enabled for color writing.
			clearFlag_depthBuffer = 1<<1, //!< Indicates the depth buffer.
			clearFlag_stencilBuffer = 1<<2 //!< Indicates the stencil buffer.
		};
		/**
		 * @brief clear sets the bitplane area of the window to values previously selected by clearColor, clearDepth, and clearStencil. Multiple color buffers can be cleared simultaneously by selecting more than one buffer at a time using drawBuffer.
		 * The pixel ownership test, the scissor test, dithering, and the buffer writemasks affect the operation of clear. The scissor box bounds the cleared region. Alpha function, blend function, logical operation, stenciling, texture mapping, and depth-buffering are ignored by clear.
		 * @param[in] _flags This is the bitwise OR of several values indicating which buffer is to be cleared.
		 */
		void clear(uint32_t _flags);
		
		enum flag {
			flag_blend = 1<<0, //!< If enabled, blend the computed fragment color values with the values in the color buffers. See glBlendFunc.
			flag_clipDistanceI = 1<<1, //!< If enabled, clip geometry against user-defined half space i.
			flag_colorLogigOP = 1<<2, //!< If enabled, apply the currently selected logical operation to the computed fragment color and color buffer values. See glLogicOp.
			flag_cullFace = 1<<3, //!< If enabled, cull polygons based on their winding in window coordinates. See glCullFace.
			flag_debugOutput = 1<<4, //!< If enabled, debug messages are produced by a debug context. When disabled, the debug message log is silenced. Note that in a non-debug context, very few, if any messages might be produced, even when GL_DEBUG_OUTPUT is enabled.
			flag_debugOutputSynchronous = 1<<5, //!< If enabled, debug messages are produced synchronously by a debug context. If disabled, debug messages may be produced asynchronously. In particular, they may be delayed relative to the execution of GL commands, and the debug callback function may be called from a thread other than that in which the commands are executed. See glDebugMessageCallback.
			flag_depthClamp = 1<<6, //!< If enabled, the -wc≤zc≤wc plane equation is ignored by view volume clipping (effectively, there is no near or far plane clipping). See glDepthRange.
			flag_depthTest = 1<<7, //!< If enabled, do depth comparisons and update the depth buffer. Note that even if the depth buffer exists and the depth mask is non-zero, the depth buffer is not updated if the depth test is disabled. See glDepthFunc and glDepthRange.
			flag_dither = 1<<8, //!< If enabled, dither color components or indices before they are written to the color buffer.
			flag_framebufferSRGB = 1<<9, //!< If enabled and the value of GL_FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING for the framebuffer attachment corresponding to the destination buffer is GL_SRGB, the R, G, and B destination color values (after conversion from fixed-point to floating-point) are considered to be encoded for the sRGB color space and hence are linearized prior to their use in blending.
			flag_lineSmooth = 1<<10, //!< If enabled, draw lines with correct filtering. Otherwise, draw aliased lines. See glLineWidth.
			flag_multisample = 1<<11, //!< If enabled, use multiple fragment samples in computing the final color of a pixel. See glSampleCoverage.
			flag_polygonOffsetFill = 1<<12, //!< If enabled, and if the polygon is rendered in GL_FILL mode, an offset is added to depth values of a polygon's fragments before the depth comparison is performed. See glPolygonOffset.
			flag_polygonOffsetLine = 1<<13, //!< If enabled, and if the polygon is rendered in GL_LINE mode, an offset is added to depth values of a polygon's fragments before the depth comparison is performed. See glPolygonOffset.
			flag_polygonOffsetPoint = 1<<14, //!< If enabled, an offset is added to depth values of a polygon's fragments before the depth comparison is performed, if the polygon is rendered in GL_POINT mode. See glPolygonOffset.
			flag_polygonSmooth = 1<<15, //!< If enabled, draw polygons with proper filtering. Otherwise, draw aliased polygons. For correct antialiased polygons, an alpha buffer is needed and the polygons must be sorted front to back.
			flag_primitiveRestart = 1<<16, //!< enables primitive restarting.  If enabled, any one of the draw commands which transfers a set of generic attribute array elements to the GL will restart the primitive when the index of the vertex is equal to the primitive restart index. See glPrimitiveRestartIndex.
			flag_primitiveRestartFixedIndex = 1<<17, //!< enables primitive restarting with a fixed index. If enabled, any one of the draw commands which transfers a set of generic attribute array elements to the GL will restart the primitive when the index of the vertex is equal to the fixed primitive index for the specified index type. The fixed index is equal to 2n−1 where n is equal to 8 for GL_UNSIGNED_BYTE, 16 for GL_UNSIGNED_SHORT and 32 for GL_UNSIGNED_INT.
			flag_sampleAlphaToCoverage = 1<<18, //!< If enabled, compute a temporary coverage value where each bit is determined by the alpha value at the corresponding sample location.  The temporary coverage value is then ANDed with the fragment coverage value.
			flag_sampleAlphaToOne = 1<<19, //!< If enabled, each sample alpha value is replaced by the maximum representable alpha value.
			flag_sampleCoverage = 1<<20, //!< If enabled, the fragment's coverage is ANDed with the temporary coverage value. If GL_SAMPLE_COVERAGE_INVERT is set to GL_TRUE, invert the coverage value. See glSampleCoverage.
			flag_sampleShading = 1<<21, //!< If enabled, the active fragment shader is run once for each covered sample, or at fraction of this rate as determined by the current value of GL_MIN_SAMPLE_SHADING_VALUE. See glMinSampleShading.
			flag_sampleMask = 1<<22, //!< If enabled, the sample coverage mask generated for a fragment during rasterization will be ANDed with the value of GL_SAMPLE_MASK_VALUE before shading occurs. See glSampleMaski.
			flag_scissorTest = 1<<23, //!< If enabled, discard fragments that are outside the scissor rectangle. See glScissor.
			flag_stencilTest = 1<<24, //!< If enabled, do stencil testing and update the stencil buffer. See glStencilFunc and glStencilOp. GL_TEXTURE_CUBE_MAP_SEAMLESS = 1<<0, //!< If enabled, cubemap textures are sampled such that when linearly sampling from the border between two adjacent faces, texels from both faces are used to generate the final sample value. When disabled, texels from only a single face are used to construct the final sample value.
			flag_programPointSize = 1<<25, //!< If enabled and a vertex or geometry shader is active, then the derived point size is taken from the (potentially clipped) shader builtin gl_PointSize and clamped to the implementation-dependent point size range.
			flag_texture2D = 1<<26, //!< 
			flag_alphaTest = 1<<27, //!< 
			flag_fog = 1<<28, //!< 
		};
		enum renderMode {
			render_point,
			render_line,
			render_lineStrip, //!< Not supported in GALE (TODO : Later)
			render_lineLoop,
			render_triangle,
			render_triangleStrip, //!< Not supported in GALE (TODO : Later)
			render_triangleFan, //!< Not supported in GALE (TODO : Later)
			render_quad, //!< Not supported in OpenGL-ES2
			render_quadStrip, //!< Not supported in OpenGL-ES2
			render_polygon //!< Not supported in OpenGL-ES2
		};
		
		/**
		 * @brief enable a flag on the system
		 * @param[in] flagID The flag requested
		 */
		void enable(enum flag _flagID);
		/**
		 * @brief disable a flag on the system
		 * @param[in] flagID The flag requested
		 */
		void disable(enum flag _flagID);
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
		// TODO : rename Disable
		void desActiveTexture(uint32_t _flagID);
		/**
		 * @brief draw a specific array  == > this enable mode difference ...
		 */
		void drawArrays(enum renderMode _mode, int32_t _first, int32_t _count);
		void drawElements  (enum renderMode _mode, const std::vector<uint32_t>& _indices);
		void drawElements16(enum renderMode _mode, const std::vector<uint16_t>& _indices);
		void drawElements8 (enum renderMode _mode, const std::vector<uint8_t>& _indices);
		/**
		 * @brief Use openGL program
		 * @param[in] id Id of the program that might be used
		 */
		void useProgram(int32_t _id);
		void reset();
		
		
		bool genBuffers(std::vector<uint32_t>& _buffers);
		bool deleteBuffers(std::vector<uint32_t>& _buffers);
		bool bindBuffer(uint32_t _bufferId);
		enum usage {
			usage_streamDraw,
			usage_staticDraw,
			usage_dynamicDraw
		};
		bool bufferData(size_t _size, const void* _data, enum gale::openGL::usage _usage);
		bool unbindBuffer();
		/* Shader wrapping : */
		namespace shader {
			enum type {
				type_vertex,
				type_fragment
			};
			int64_t create(enum gale::openGL::shader::type _type);
			void remove(int64_t& _shader);
			bool compile(int64_t _shader, const std::string& _data);
		};
		namespace program {
			int64_t create();
			void remove(int64_t& _prog);
			bool attach(int64_t _prog, int64_t _shader);
			bool compile(int64_t _prog);
			int32_t getAttributeLocation(int64_t _prog, const std::string& _name);
			int32_t getUniformLocation(int64_t _prog, const std::string& _name);
		};
	};
	std::ostream& operator <<(std::ostream& _os, const enum openGL::flag& _obj);
	std::ostream& operator <<(std::ostream& _os, const enum openGL::renderMode& _obj);
}

