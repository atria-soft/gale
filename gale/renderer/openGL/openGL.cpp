/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license APACHE v2.0 (see license file)
 */

#include <vector>
#include <gale/debug.h>
#include <gale/renderer/openGL/openGL.h>
#include <etk/stdTools.h>
#include <mutex>
//#define DIRECT_MODE


extern "C" {
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
}


#define CHECK_ERROR_OPENGL

static void checkGlError(const char* _op, int32_t _localLine) {
	#ifdef CHECK_ERROR_OPENGL
	bool hasError = false;
	for (GLint error = glGetError(); error; error = glGetError()) {
		GALE_ERROR("after " << _op << "():" << _localLine << " glError(" << error << ")");
		hasError = true;
	}
	if (hasError == true) {
		GALE_CRITICAL("plop");
	}
	#endif
}

#define OPENGL_ERROR(data) do { } while (false)
//#define OPENGL_ERROR(data) GALE_ERROR(data)
#define OPENGL_WARNING(data) do { } while (false)
//#define OPENGL_WARNING(data) GALE_WARNING(data)
#define OPENGL_INFO(data) do { } while (false)
//#define OPENGL_INFO(data) GALE_INFO(data)


/**
 * @brief get the draw mutex (gale render).
 * @note due ti the fact that the system can be called for multiple instance, for naw we just limit the acces to one process at a time.
 * @return the main inteface Mutex
 */
static std::mutex& mutexOpenGl() {
	static std::mutex s_drawMutex;
	return s_drawMutex;
}

std::vector<mat4> l_matrixList;
mat4 l_matrixCamera;
static uint32_t l_flagsCurrent = 0;
static uint32_t l_flagsMustBeSet = 0;
static uint32_t l_textureflags = 0;
static int32_t  l_programId = 0;


void gale::openGL::lock() {
	mutexOpenGl().lock();
	l_matrixList.clear();
	mat4 tmpMat;
	l_matrixList.push_back(tmpMat);
	l_matrixCamera.identity();
	l_flagsCurrent = 0;
	l_flagsMustBeSet = 0;
	l_textureflags = 0;
	l_programId = -1;
}

void gale::openGL::unLock() {
	mutexOpenGl().unlock();
}

void gale::openGL::setBasicMatrix(const mat4& _newOne) {
	if (l_matrixList.size()!=1) {
		GALE_ERROR("matrix is not corect size in the stack : " << l_matrixList.size());
	}
	l_matrixList.clear();
	l_matrixList.push_back(_newOne);
}

void gale::openGL::setMatrix(const mat4& _newOne) {
	if (l_matrixList.size() == 0) {
		GALE_ERROR("set matrix list is not corect size in the stack : " << l_matrixList.size());
		l_matrixList.push_back(_newOne);
		return;
	}
	l_matrixList[l_matrixList.size()-1] = _newOne;
}

void gale::openGL::push() {
	if (l_matrixList.size() == 0) {
		GALE_ERROR("set matrix list is not corect size in the stack : " << l_matrixList.size());
		mat4 tmp;
		l_matrixList.push_back(tmp);
		return;
	}
	mat4 tmp = l_matrixList[l_matrixList.size()-1];
	l_matrixList.push_back(tmp);
}

void gale::openGL::pop() {
	if (l_matrixList.size() <= 1) {
		GALE_ERROR("set matrix list is not corect size in the stack : " << l_matrixList.size());
		l_matrixList.clear();
		mat4 tmp;
		l_matrixList.push_back(tmp);
		l_matrixCamera.identity();
		return;
	}
	l_matrixList.pop_back();
	l_matrixCamera.identity();
}

const mat4& gale::openGL::getMatrix() {
	if (l_matrixList.size() == 0) {
		GALE_ERROR("set matrix list is not corect size in the stack : " << l_matrixList.size());
		mat4 tmp;
		l_matrixList.push_back(tmp);
	}
	return l_matrixList[l_matrixList.size()-1];
}

const mat4& gale::openGL::getCameraMatrix() {
	return l_matrixCamera;
}

void gale::openGL::setCameraMatrix(const mat4& _newOne) {
	l_matrixCamera = _newOne;
}

void gale::openGL::finish() {
	l_programId = -1;
	l_textureflags = 0;
}

void gale::openGL::flush() {
	l_programId = -1;
	l_textureflags = 0;
	glFlush();
	OPENGL_INFO("========================" );
	OPENGL_INFO("==   FLUSH OPEN GL    ==" );
	OPENGL_INFO("========================");
}

void gale::openGL::swap() {
	
}


struct correspondenceTableClear {
	enum gale::openGL::clearFlag curentFlag;
	GLbitfield OGlFlag;
};

static struct correspondenceTableClear basicFlagClear[] = {
	{gale::openGL::clearFlag_colorBuffer, GL_COLOR_BUFFER_BIT},
	{gale::openGL::clearFlag_depthBuffer, GL_DEPTH_BUFFER_BIT},
	{gale::openGL::clearFlag_stencilBuffer, GL_STENCIL_BUFFER_BIT}
};

static int32_t basicFlagClearCount = sizeof(basicFlagClear) / sizeof(struct correspondenceTableClear);


void gale::openGL::clearColor(const etk::Color<float>& _color) {
	glClearColor(_color.r(), _color.g(), _color.b(), _color.a());
}

void gale::openGL::clearDepth(float _value) {
	glClearDepth(_value);
}
void gale::openGL::clearStencil(int32_t _value) {
	glClearStencil(_value);
}

void gale::openGL::clear(uint32_t _flags) {
	GLbitfield field = 0;
	for (int32_t iii=0; iii<basicFlagClearCount ; iii++) {
		if ( (basicFlagClear[iii].curentFlag & _flagID) != 0) {
			field |= basicFlagClear[iii].OGlFlag;
		}
	}
	glClear(field);
}

std::ostream& gale::operator <<(std::ostream& _os, const enum openGL::flag& _obj) {
	static std::vector<std::pair<enum openGL::flag, const char*>> list = {
		std::make_pair(openGL::flag_blend, "FLAG_BLEND"),
		std::make_pair(openGL::flag_clipDistanceI, "FLAG_CLIP_DISTANCE_I"),
		std::make_pair(openGL::flag_colorLogigOP, "FLAG_COLOR_LOGIC_OP"),
		std::make_pair(openGL::flag_cullFace, "FLAG_CULL_FACE"),
		std::make_pair(openGL::flag_debugOutput, "FLAG_DEBUG_OUTPUT"),
		std::make_pair(openGL::flag_debugOutputSynchronous, "flag_debugOutput_SYNCHRONOUS"),
		std::make_pair(openGL::flag_depthClamp, "FLAG_DEPTH_CLAMP"),
		std::make_pair(openGL::flag_depthTest, "FLAG_DEPTH_TEST"),
		std::make_pair(openGL::flag_dither, "FLAG_DITHER"),
		std::make_pair(openGL::flag_framebufferSRGB, "FLAG_FRAMEBUFFER_SRGB"),
		std::make_pair(openGL::flag_lineSmooth, "FLAG_LINE_SMOOTH"),
		std::make_pair(openGL::flag_multisample, "FLAG_MULTISAMPLE"),
		std::make_pair(openGL::flag_polygonOffsetFill, "FLAG_POLYGON_OFFSET_FILL"),
		std::make_pair(openGL::flag_polygonOffsetLine, "FLAG_POLYGON_OFFSET_LINE"),
		std::make_pair(openGL::flag_polygonOffsetPoint, "FLAG_POLYGON_OFFSET_POINT"),
		std::make_pair(openGL::flag_polygonSmooth, "FLAG_POLYGON_SMOOTH"),
		std::make_pair(openGL::flag_primitiveRestart, "FLAG_PRIMITIVE_RESTART"),
		std::make_pair(openGL::flag_primitiveRestartFixedIndex, "flag_primitiveRestart_FIXED_INDEX"),
		std::make_pair(openGL::flag_sampleAlphaToCoverage, "FLAG_SAMPLE_ALPHA_TO_COVERAGE"),
		std::make_pair(openGL::flag_sampleAlphaToOne, "FLAG_SAMPLE_ALPHA_TO_ONE"),
		std::make_pair(openGL::flag_sampleCoverage, "FLAG_SAMPLE_COVERAGE"),
		std::make_pair(openGL::flag_sampleShading, "FLAG_SAMPLE_SHADING"),
		std::make_pair(openGL::flag_sampleMask, "FLAG_SAMPLE_MASK"),
		std::make_pair(openGL::flag_scissorTest, "FLAG_SCISSOR_TEST"),
		std::make_pair(openGL::flag_stencilTest, "FLAG_STENCIL_TEST"),
		std::make_pair(openGL::flag_programPointSize, "FLAG_PROGRAM_POINT_SIZE"),
		std::make_pair(openGL::flag_texture2D, "FLAG_TEXTURE_2D"),
		std::make_pair(openGL::flag_alphaTest, "FLAG_ALPHA_TEST"),
		std::make_pair(openGL::flag_fog, "FLAG_FOG")
	};
	_os << "{";
	bool hasOne = false;
	for (auto &it : list) {
		if ((_obj & it.first) != 0) {
			if (hasOne==true) {
				_os << ",";
			}
			_os << it.second;
			hasOne = true;
		}
	}
	_os << "}";
	return _os;
}


std::vector<std::pair<enum gale::openGL::renderMode, std::string>>& getListRenderMode() {
	static std::vector<std::pair<enum gale::openGL::renderMode, std::string>> list = {
		std::make_pair(gale::openGL::render_point, "POINTS"),
		std::make_pair(gale::openGL::render_line, "LINES"),
		std::make_pair(gale::openGL::render_lineStrip, "LINES_STRIP"),
		std::make_pair(gale::openGL::render_lineLoop, "LINE_LOOP"),
		std::make_pair(gale::openGL::render_triangle, "TRIANGLE"),
		std::make_pair(gale::openGL::render_triangleStrip, "TRIANGLE_STRIP"),
		std::make_pair(gale::openGL::render_triangleFan, "TRIANGLE_FAN"),
		std::make_pair(gale::openGL::render_quad, "QUAD"),
		std::make_pair(gale::openGL::render_quadStrip, "QUAD_STRIP"),
		std::make_pair(gale::openGL::render_polygon, "POLYGON"),
	};
	return list;
}


namespace etk {
	template<> std::string to_string<gale::openGL::renderMode>(const gale::openGL::renderMode& _obj) {
		for (auto &it : getListRenderMode()) {
			if (it.first == _obj) {
				return it.second;
			}
		}
		GALE_ERROR("Can not convert : " << static_cast<int32_t>(_obj) << " return UNKNOW");
		return "UNKNOW";
	}
	template<> std::u32string to_u32string<gale::openGL::renderMode>(const gale::openGL::renderMode& _obj) {
		return etk::to_u32string(etk::to_string(_obj));
	}
	template<> bool from_string<gale::openGL::renderMode>(gale::openGL::renderMode& _variableRet, const std::string& _value) {
		for (auto &it : getListRenderMode()) {
			if (it.second == _value) {
				_variableRet = it.first;
				return true;
			}
		}
		GALE_WARNING("Can not parse : '" << _value << "' set Triangle default value");
		_variableRet = gale::openGL::render_triangle;
		return false;
	}
	template<> bool from_string<gale::openGL::renderMode>(gale::openGL::renderMode& _variableRet, const std::u32string& _value) {
		return from_string(_variableRet, etk::to_string(_value));
	}
};

std::ostream& gale::operator <<(std::ostream& _os, const enum openGL::renderMode& _obj) {
	_os << etk::to_string(_obj);
	return _os;
}


struct correspondenceTable {
	enum gale::openGL::flag curentFlag;
	GLenum OGlFlag;
};

static correspondenceTable_ts basicFlag[] = {
	{gale::openGL::flag_blend, GL_BLEND},
	#if 0
	!(defined(__TARGET_OS__Android) || defined(__TARGET_OS__MacOs))
	{gale::openGL::flag_clipDistanceI, GL_CLIP_DISTANCE0},
	{gale::openGL::flag_colorLogigOP, GL_COLOR_LOGIC_OP},
	#endif
	{(uint32_t)gale::openGL::flag_cullFace, GL_CULL_FACE},
	#if 0
		!(defined(__TARGET_OS__Android) || defined(__TARGET_OS__MacOs))
	{gale::openGL::flag_debugOutput, GL_DEBUG_OUTPUT},
	{gale::openGL::flag_debugOutputSynchronous, GL_DEBUG_OUTPUT_SYNCHRONOUS},
	{gale::openGL::flag_depthClamp, GL_DEPTH_CLAMP},
	#endif
	{gale::openGL::flag_depthTest, GL_DEPTH_TEST},
	{gale::openGL::flag_dither, GL_DITHER},
	#if 0
	!(defined(__TARGET_OS__Android) || defined(__TARGET_OS__MacOs))
	{gale::openGL::flag_framebufferSRGB, GL_FRAMEBUFFER_SRGB},
	{gale::openGL::flag_lineSmooth, GL_LINE_SMOOTH},
	{gale::openGL::flag_multisample, GL_MULTISAMPLE},
	#endif
	{gale::openGL::flag_polygonOffsetFill, GL_POLYGON_OFFSET_FILL},
	#if 0
	!(defined(__TARGET_OS__Android) || defined(__TARGET_OS__MacOs))
	{gale::openGL::flag_polygonOffsetLine, GL_POLYGON_OFFSET_LINE},
	{gale::openGL::flag_polygonOffsetPoint, GL_POLYGON_OFFSET_POINT},
	{gale::openGL::flag_polygonSmooth, GL_POLYGON_SMOOTH},
	{gale::openGL::flag_primitiveRestart, GL_PRIMITIVE_RESTART},
	{gale::openGL::flag_primitiveRestartFixedIndex, GL_PRIMITIVE_RESTART_FIXED_INDEX},
	#endif
	{gale::openGL::flag_sampleAlphaToCoverage, GL_SAMPLE_ALPHA_TO_COVERAGE},
	#if 0
	!(defined(__TARGET_OS__Android) || defined(__TARGET_OS__MacOs))
	{gale::openGL::flag_sampleAlphaToOne, GL_SAMPLE_ALPHA_TO_ONE},
	#endif
	{gale::openGL::flag_sampleCoverage, GL_SAMPLE_COVERAGE},
	#if 0
	!(defined(__TARGET_OS__Android) || defined(__TARGET_OS__MacOs))
	{gale::openGL::flag_sampleShading, GL_SAMPLE_SHADING},
	{gale::openGL::flag_sampleMask, GL_SAMPLE_MASK},
	#endif
	{gale::openGL::flag_scissorTest, GL_SCISSOR_TEST},
	{gale::openGL::flag_stencilTest, GL_STENCIL_TEST},
	#if 0
	!(defined(__TARGET_OS__Android) || defined(__TARGET_OS__MacOs))
	{gale::openGL::flag_programPointSize, GL_PROGRAM_POINT_SIZE},
	#endif
	{gale::openGL::flag_texture2D, GL_TEXTURE_2D},
	#if 0
	!(defined(__TARGET_OS__Android) || defined(__TARGET_OS__MacOs))
	{gale::openGL::flag_alphaTest, GL_ALPHA_TEST},
	{gale::openGL::flag_fog, GL_FOG}
	#endif
	//{(uint32_t)gale::openGL::FLAG_, GL_}
};
static int32_t basicFlagCount = sizeof(basicFlag) / sizeof(struct correspondenceTable);

void gale::openGL::reset() {
	#ifdef DIRECT_MODE
		GALE_TODO("...");
	#else
		l_flagsMustBeSet = 0;
		l_programId = -1;
		l_textureflags = 0;
		updateAllFlags();
	#endif
}

void gale::openGL::enable(enum gale::openGL::flag _flagID) {
	//GALE_INFO("Enable : " <GALE_ERROR< _flagID);
	#ifdef DIRECT_MODE
	for (int32_t iii=0; iii<basicFlagCount ; iii++) {
		if ( basicFlag[iii].curentFlag == (uint32_t)_flagID ) {
			glEnable(basicFlag[iii].OGlFlag);
		}
	}
	# else
		//GALE_DEBUG("Enable FLAGS = " << l_flagsMustBeSet);
		l_flagsMustBeSet |= (uint32_t)_flagID;
		//GALE_DEBUG("             == >" << l_flagsMustBeSet);
	#endif
}

void gale::openGL::disable(enum gale::openGL::flag _flagID) {
	//GALE_INFO("Disable : " << _flagID);
	#ifdef DIRECT_MODE
	for (int32_t iii=0; iii<basicFlagCount ; iii++) {
		if ( basicFlag[iii].curentFlag == (uint32_t)_flagID ) {
			glDisable(basicFlag[iii].OGlFlag);
		}
	}
	# else
		//GALE_DEBUG("Disable FLAGS = " << l_flagsMustBeSet);
		l_flagsMustBeSet &= ~((uint32_t)_flagID);
		//GALE_DEBUG("              == >" << l_flagsMustBeSet);
	#endif
}

void gale::openGL::updateAllFlags() {
	#ifdef DIRECT_MODE
		return;
	#endif
	// check if fhags has change :
	if (l_flagsMustBeSet == l_flagsCurrent ) {
		OPENGL_INFO("OGL: current flag : " << (enum openGL::flag)l_flagsMustBeSet);
		return;
	}
	OPENGL_INFO("OGL: set new flag : " << (enum openGL::flag)l_flagsMustBeSet);
	for (int32_t iii=0; iii<basicFlagCount ; iii++) {
		uint32_t CurrentFlag = basicFlag[iii].curentFlag;
		if ( (l_flagsMustBeSet&CurrentFlag)!=(l_flagsCurrent&CurrentFlag) ) {
			if ( (l_flagsMustBeSet&CurrentFlag) != 0) {
				glEnable(basicFlag[iii].OGlFlag);
				OPENGL_INFO("    enable : " << (enum openGL::flag)basicFlag[iii].curentFlag);
			} else {
				glDisable(basicFlag[iii].OGlFlag);
				OPENGL_INFO("    disable : " << (enum openGL::flag)basicFlag[iii].curentFlag);
			}
		}
	}
	l_flagsCurrent = l_flagsMustBeSet;
}


void gale::openGL::activeTexture(uint32_t _flagID) {
	if (l_programId >= 0) {
		glActiveTexture(_flagID);
	}
}

void gale::openGL::desActiveTexture(uint32_t _flagID) {
	if (l_programId >= 0) {
		
	}
}

const uint32_t convertRenderMode[] = {
	GL_POINTS,
	GL_LINES,
	GL_LINE_STRIP,
	GL_LINE_LOOP,
	GL_TRIANGLES,
	GL_TRIANGLE_STRIP,
	GL_TRIANGLE_FAN,
	#if (!defined(__TARGET_OS__IOs) && !defined(__TARGET_OS__Android))
		GL_QUADS,
		GL_QUAD_STRIP,
		GL_POLYGON
	#else
		0,
		0,
		0
	#endif
};

void gale::openGL::drawArrays(enum renderMode _mode, int32_t _first, int32_t _count) {
	if (l_programId >= 0) {
		updateAllFlags();
		glDrawArrays(convertRenderMode[_mode], _first, _count);
	}
}

void gale::openGL::drawElements(enum renderMode _mode, const std::vector<uint32_t>& _indices) {
	if (l_programId >= 0) {
		updateAllFlags();
		//GALE_DEBUG("Request draw of " << indices.size() << "elements");
		glDrawElements(convertRenderMode[_mode], _indices.size(), GL_UNSIGNED_INT, &_indices[0]);
	}
}

void gale::openGL::drawElements16(enum renderMode _mode, const std::vector<uint16_t>& _indices) {
	if (l_programId >= 0) {
		updateAllFlags();
		glDrawElements(convertRenderMode[_mode], _indices.size(), GL_UNSIGNED_SHORT, &_indices[0]);
	}
}

void gale::openGL::drawElements8(enum renderMode _mode, const std::vector<uint8_t>& _indices) {
	if (l_programId >= 0) {
		updateAllFlags();
		glDrawElements(convertRenderMode[_mode], _indices.size(), GL_UNSIGNED_BYTE, &_indices[0]);
	}
}

void gale::openGL::useProgram(int32_t _id) {
	//GALE_DEBUG("USE prog : " << id);
	#if 1
		// note : In normal openGL case, the system might call with the program ID and at the end with 0, 
		//        here, we wrap this use to prevent over call of glUseProgram  == > then we set -1 when the 
		//        user no more use this program, and just stop grnerating. (chen 0  == > this is an errored program ...
		if (-1 == _id) {
			// not used  == > because it is unneded
			return;
		}
		if (l_programId != _id) {
			l_programId = _id;
			glUseProgram(l_programId);
		}
	#else
		if (-1 == _id) {
			glUseProgram(0);
		} else {
			l_programId = _id;
			glUseProgram(_id);
		}
	#endif
}




bool gale::openGL::genBuffers(std::vector<uint32_t>& _buffers) {
	if (_buffers.size() == 0) {
		GALE_WARNING("try to generate vector buffer with size 0");
		return true;
	}
	OPENGL_INFO("Create N=" << _buffers.size() << " Buffer");
	glGenBuffers(_buffers.size(), &_buffers[0]);
	checkGlError("glGenBuffers", __LINE__);
	bool hasError = false;
	for (size_t iii=0; iii<_buffers.size(); iii++) {
		if (_buffers[iii] == 0) {
			GALE_ERROR("[" << iii << "] error to create a buffer id=" << _buffers[iii]);
			hasError = true;
		}
	}
	return hasError;
}

bool gale::openGL::deleteBuffers(std::vector<uint32_t>& _buffers) {
	if (_buffers.size() == 0) {
		GALE_WARNING("try to delete vector buffer with size 0");
		return true;
	}
	glDeleteBuffers(_buffers.size(), &_buffers[0]);
	checkGlError("glDeleteBuffers", __LINE__);
	for (auto &it : _buffers) {
		it = 0;
	}
	return true;
}

bool gale::openGL::bindBuffer(uint32_t _bufferId) {
	glBindBuffer(GL_ARRAY_BUFFER, _bufferId);
	checkGlError("glBindBuffer", __LINE__);
	return true;
}

 GL_STREAM_DRAW, GL_STATIC_DRAW, or GL_DYNAMIC_DRAW
 
bool gale::openGL::bufferData(size_t _size, const void* _data, GLenum _usage) {
	glBufferData(GL_ARRAY_BUFFER, _size, _data, _usage);
	checkGlError("glBufferData", __LINE__);
	return true;
}

bool gale::openGL::unbindBuffer() {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	checkGlError("glBindBuffer(0)", __LINE__);
	return true;
}

