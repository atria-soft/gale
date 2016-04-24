/** @file
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
#include <gale/renderer/openGL/openGL-include.h>
#include <ethread/tools.h>

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



#ifdef GALE_SIMULATION_OPENGL_AVAILLABLE
	bool s_simulationMode = false;
	/**
	 * @brief Start the simulation mode of Open GL (can be usebull when no HW is availlable for testing System like buildfarm (eg: travis-ci)
	 * This generate a non access on the openGL IO, and all function work corectly.
	 */
	void gale::openGL::startSimulationMode() {
		s_simulationMode = true;
	}
#endif

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

static std::vector<std::thread::id>& getContextList() {
	static std::vector<std::thread::id> g_val;
	return g_val;
}

bool gale::openGL::hasContext() {
	bool ret = false;
	mutexOpenGl().lock();
	auto it = std::find(getContextList().begin(), getContextList().end(), std::this_thread::get_id());
	if (it != getContextList().end()) {
		ret = true;
	}
	mutexOpenGl().unlock();
	return ret;
}

void gale::openGL::threadHasContext() {
	mutexOpenGl().lock();
	auto it = std::find(getContextList().begin(), getContextList().end(), std::this_thread::get_id());
	if (it != getContextList().end()) {
		GALE_ERROR("set openGL context associate with threadID a second time ... ");
	} else {
		getContextList().push_back(std::this_thread::get_id());
	}
	mutexOpenGl().unlock();
}

void gale::openGL::threadHasNoMoreContext() {
	mutexOpenGl().lock();
	auto it = std::find(getContextList().begin(), getContextList().end(), std::this_thread::get_id());
	if (it != getContextList().end()) {
		getContextList().erase(it);
	} else {
		GALE_ERROR("rm openGL context associate with threadID that is not registered.");
	}
	
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
	#ifdef GALE_SIMULATION_OPENGL_AVAILLABLE
		if (s_simulationMode == false) {
	#endif
	glFlush();
	#ifdef GALE_SIMULATION_OPENGL_AVAILLABLE
		}
	#endif
	OPENGL_INFO("========================" );
	OPENGL_INFO("==   FLUSH OPEN GL    ==" );
	OPENGL_INFO("========================");
}

void gale::openGL::swap() {
	
}

void gale::openGL::setViewPort(const ivec2& _start, const ivec2& _stop) {
	#ifdef GALE_SIMULATION_OPENGL_AVAILLABLE
		if (s_simulationMode == false) {
	#endif
	glViewport(_start.x(), _start.y(), _stop.x(), _stop.y());
	#ifdef GALE_SIMULATION_OPENGL_AVAILLABLE
		}
	#endif
}

void gale::openGL::setViewPort(const vec2& _start, const vec2& _stop) {
	#ifdef GALE_SIMULATION_OPENGL_AVAILLABLE
		if (s_simulationMode == false) {
	#endif
	glViewport(_start.x(), _start.y(), _stop.x(), _stop.y());
	#ifdef GALE_SIMULATION_OPENGL_AVAILLABLE
		}
	#endif
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
	#ifdef GALE_SIMULATION_OPENGL_AVAILLABLE
		if (s_simulationMode == false) {
	#endif
	glClearColor(_color.r(), _color.g(), _color.b(), _color.a());
	#ifdef GALE_SIMULATION_OPENGL_AVAILLABLE
		}
	#endif
}

void gale::openGL::clearDepth(float _value) {
	#ifdef GALE_SIMULATION_OPENGL_AVAILLABLE
		if (s_simulationMode == false) {
	#endif
	#if !((defined(__TARGET_OS__Android) || defined(__TARGET_OS__IOs)))
		glClearDepth(_value);
	#endif
	#if defined(__TARGET_OS__IOs)
		glClearDepthf(_value);
	#endif
	#ifdef GALE_SIMULATION_OPENGL_AVAILLABLE
		}
	#endif
}
void gale::openGL::clearStencil(int32_t _value) {
	#ifdef GALE_SIMULATION_OPENGL_AVAILLABLE
		if (s_simulationMode == false) {
	#endif
	glClearStencil(_value);
	#ifdef GALE_SIMULATION_OPENGL_AVAILLABLE
		}
	#endif
}

void gale::openGL::clear(uint32_t _flags) {
	GLbitfield field = 0;
	for (int32_t iii=0; iii<basicFlagClearCount ; iii++) {
		if ( (basicFlagClear[iii].curentFlag & _flags) != 0) {
			field |= basicFlagClear[iii].OGlFlag;
		}
	}
	#ifdef GALE_SIMULATION_OPENGL_AVAILLABLE
		if (s_simulationMode == false) {
	#endif
	glClear(field);
	#ifdef GALE_SIMULATION_OPENGL_AVAILLABLE
		}
	#endif
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

static struct correspondenceTable basicFlag[] = {
	{gale::openGL::flag_blend, GL_BLEND},
	#if 0
	!(defined(__TARGET_OS__Android) || defined(__TARGET_OS__MacOs))
	{gale::openGL::flag_clipDistanceI, GL_CLIP_DISTANCE0},
	{gale::openGL::flag_colorLogigOP, GL_COLOR_LOGIC_OP},
	#endif
	{gale::openGL::flag_cullFace, GL_CULL_FACE},
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
		#ifdef GALE_SIMULATION_OPENGL_AVAILLABLE
			if (s_simulationMode == false) {
		#endif
		for (int32_t iii=0; iii<basicFlagCount ; iii++) {
			if ( basicFlag[iii].curentFlag == (uint32_t)_flagID ) {
				glEnable(basicFlag[iii].OGlFlag);
			}
		}
		#ifdef GALE_SIMULATION_OPENGL_AVAILLABLE
			}
		#endif
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
				#ifdef GALE_SIMULATION_OPENGL_AVAILLABLE
					if (s_simulationMode == false) {
				#endif
				glDisable(basicFlag[iii].OGlFlag);
				#ifdef GALE_SIMULATION_OPENGL_AVAILLABLE
					}
				#endif
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
				#ifdef GALE_SIMULATION_OPENGL_AVAILLABLE
					if (s_simulationMode == false) {
				#endif
				glEnable(basicFlag[iii].OGlFlag);
				#ifdef GALE_SIMULATION_OPENGL_AVAILLABLE
					}
				#endif
				OPENGL_INFO("    enable : " << (enum openGL::flag)basicFlag[iii].curentFlag);
			} else {
				#ifdef GALE_SIMULATION_OPENGL_AVAILLABLE
					if (s_simulationMode == false) {
				#endif
				glDisable(basicFlag[iii].OGlFlag);
				#ifdef GALE_SIMULATION_OPENGL_AVAILLABLE
					}
				#endif
				OPENGL_INFO("    disable : " << (enum openGL::flag)basicFlag[iii].curentFlag);
			}
		}
	}
	l_flagsCurrent = l_flagsMustBeSet;
}


void gale::openGL::activeTexture(uint32_t _flagID) {
	if (l_programId >= 0) {
		#ifdef GALE_SIMULATION_OPENGL_AVAILLABLE
			if (s_simulationMode == false) {
		#endif
		glActiveTexture(_flagID);
		#ifdef GALE_SIMULATION_OPENGL_AVAILLABLE
			}
		#endif
	} else {
		#if DEBUG
			GALE_ERROR("try to bind texture with no program set");
		#endif
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

void gale::openGL::drawArrays(enum gale::openGL::renderMode _mode, int32_t _first, int32_t _count) {
	if (l_programId >= 0) {
		updateAllFlags();
		#ifdef GALE_SIMULATION_OPENGL_AVAILLABLE
			if (s_simulationMode == false) {
		#endif
		glDrawArrays(convertRenderMode[_mode], _first, _count);
		#ifdef GALE_SIMULATION_OPENGL_AVAILLABLE
			}
		#endif
	}
}

void gale::openGL::drawElements(enum renderMode _mode, const std::vector<uint32_t>& _indices) {
	if (l_programId >= 0) {
		updateAllFlags();
		#ifdef GALE_SIMULATION_OPENGL_AVAILLABLE
			if (s_simulationMode == false) {
		#endif
		//GALE_DEBUG("Request draw of " << indices.size() << "elements");
		glDrawElements(convertRenderMode[_mode], _indices.size(), GL_UNSIGNED_INT, &_indices[0]);
		#ifdef GALE_SIMULATION_OPENGL_AVAILLABLE
			}
		#endif
	}
}

void gale::openGL::drawElements16(enum renderMode _mode, const std::vector<uint16_t>& _indices) {
	if (l_programId >= 0) {
		updateAllFlags();
		#ifdef GALE_SIMULATION_OPENGL_AVAILLABLE
			if (s_simulationMode == false) {
		#endif
		glDrawElements(convertRenderMode[_mode], _indices.size(), GL_UNSIGNED_SHORT, &_indices[0]);
		#ifdef GALE_SIMULATION_OPENGL_AVAILLABLE
			}
		#endif
	}
}

void gale::openGL::drawElements8(enum renderMode _mode, const std::vector<uint8_t>& _indices) {
	if (l_programId >= 0) {
		updateAllFlags();
		#ifdef GALE_SIMULATION_OPENGL_AVAILLABLE
			if (s_simulationMode == false) {
		#endif
		glDrawElements(convertRenderMode[_mode], _indices.size(), GL_UNSIGNED_BYTE, &_indices[0]);
		#ifdef GALE_SIMULATION_OPENGL_AVAILLABLE
			}
		#endif
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
			#ifdef GALE_SIMULATION_OPENGL_AVAILLABLE
				if (s_simulationMode == false) {
			#endif
			glUseProgram(l_programId);
			#ifdef GALE_SIMULATION_OPENGL_AVAILLABLE
				}
			#endif
		}
	#else
		if (-1 == _id) {
			#ifdef GALE_SIMULATION_OPENGL_AVAILLABLE
				if (s_simulationMode == false) {
			#endif
			glUseProgram(0);
			#ifdef GALE_SIMULATION_OPENGL_AVAILLABLE
				}
			#endif
		} else {
			l_programId = _id;
			#ifdef GALE_SIMULATION_OPENGL_AVAILLABLE
				if (s_simulationMode == false) {
			#endif
			glUseProgram(_id);
			#ifdef GALE_SIMULATION_OPENGL_AVAILLABLE
				}
			#endif
		}
	#endif
}




bool gale::openGL::genBuffers(std::vector<uint32_t>& _buffers) {
	if (_buffers.size() == 0) {
		GALE_WARNING("try to generate vector buffer with size 0");
		return true;
	}
	OPENGL_INFO("Create N=" << _buffers.size() << " Buffer");
	#ifdef GALE_SIMULATION_OPENGL_AVAILLABLE
		if (s_simulationMode == false) {
	#endif
	glGenBuffers(_buffers.size(), &_buffers[0]);
	checkGlError("glGenBuffers", __LINE__);
	#ifdef GALE_SIMULATION_OPENGL_AVAILLABLE
		}
	#endif
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
	#ifdef GALE_SIMULATION_OPENGL_AVAILLABLE
		if (s_simulationMode == false) {
	#endif
	glDeleteBuffers(_buffers.size(), &_buffers[0]);
	checkGlError("glDeleteBuffers", __LINE__);
	#ifdef GALE_SIMULATION_OPENGL_AVAILLABLE
		}
	#endif
	for (auto &it : _buffers) {
		it = 0;
	}
	return true;
}

bool gale::openGL::bindBuffer(uint32_t _bufferId) {
	#ifdef GALE_SIMULATION_OPENGL_AVAILLABLE
		if (s_simulationMode == false) {
	#endif
	glBindBuffer(GL_ARRAY_BUFFER, _bufferId);
	checkGlError("glBindBuffer", __LINE__);
	#ifdef GALE_SIMULATION_OPENGL_AVAILLABLE
		}
	#endif
	return true;
}

static GLenum convertUsage[] = {
	GL_STREAM_DRAW,
	GL_STATIC_DRAW,
	GL_DYNAMIC_DRAW
};

bool gale::openGL::bufferData(size_t _size, const void* _data, enum gale::openGL::usage _usage) {
	#ifdef GALE_SIMULATION_OPENGL_AVAILLABLE
		if (s_simulationMode == false) {
	#endif
	glBufferData(GL_ARRAY_BUFFER, _size, _data, convertUsage[_usage]);
	checkGlError("glBufferData", __LINE__);
	#ifdef GALE_SIMULATION_OPENGL_AVAILLABLE
		}
	#endif
	return true;
}

bool gale::openGL::unbindBuffer() {
	#ifdef GALE_SIMULATION_OPENGL_AVAILLABLE
		if (s_simulationMode == false) {
	#endif
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	checkGlError("glBindBuffer(0)", __LINE__);
	#ifdef GALE_SIMULATION_OPENGL_AVAILLABLE
		}
	#endif
	return true;
}

static void checkGlError(const char* _op) {
	for (GLint error = glGetError(); error; error = glGetError()) {
		GALE_ERROR("after " << _op << "() glError (" << error << ")");
	}
}
#define LOG_OGL_INTERNAL_BUFFER_LEN    (8192)
static char l_bufferDisplayError[LOG_OGL_INTERNAL_BUFFER_LEN] = "";

int64_t gale::openGL::shader::create(enum gale::openGL::shader::type _type) {
	GLuint shader = 0;
	if (_type == gale::openGL::shader::type_vertex) {
		#ifdef GALE_SIMULATION_OPENGL_AVAILLABLE
			if (s_simulationMode == false) {
		#endif
		shader = glCreateShader(GL_VERTEX_SHADER);
		#ifdef GALE_SIMULATION_OPENGL_AVAILLABLE
			} else {
				static int32_t count = 0;
				shader = count++;
			}
		#endif
	} else if (_type == gale::openGL::shader::type_fragment) {
		#ifdef GALE_SIMULATION_OPENGL_AVAILLABLE
			if (s_simulationMode == false) {
		#endif
		shader = glCreateShader(GL_FRAGMENT_SHADER);
		#ifdef GALE_SIMULATION_OPENGL_AVAILLABLE
			} else {
				static int32_t count = 0;
				shader = count++;
			}
		#endif
	} else {
		GALE_ERROR("create shader with wrong type ...");
		return -1;
	}
	if (shader == 0) {
		GALE_ERROR("glCreateShader return error ...");
		#ifdef GALE_SIMULATION_OPENGL_AVAILLABLE
			if (s_simulationMode == false) {
		#endif
		checkGlError("glCreateShader");
		#ifdef GALE_SIMULATION_OPENGL_AVAILLABLE
			}
		#endif
		return -1;
	}
	return int64_t(shader);
}

void gale::openGL::shader::remove(int64_t& _shader) {
	if (_shader < 0) {
		return;
	}
	#ifdef GALE_SIMULATION_OPENGL_AVAILLABLE
		if (s_simulationMode == false) {
	#endif
	glDeleteShader(GLuint(_shader));
	#ifdef GALE_SIMULATION_OPENGL_AVAILLABLE
		}
	#endif
	_shader = -1;
}

bool gale::openGL::shader::compile(int64_t _shader, const std::string& _data) {
	const char* data = &_data[0];
	#ifdef GALE_SIMULATION_OPENGL_AVAILLABLE
		if (s_simulationMode == false) {
	#endif
	glShaderSource(GLuint(_shader), 1, (const char**)&data, nullptr);
	glCompileShader(GLuint(_shader));
	GLint compiled = 0;
	glGetShaderiv(GLuint(_shader), GL_COMPILE_STATUS, &compiled);
	if (!compiled) {
		GLint infoLen = 0;
		l_bufferDisplayError[0] = '\0';
		glGetShaderInfoLog(GLuint(_shader), LOG_OGL_INTERNAL_BUFFER_LEN, &infoLen, l_bufferDisplayError);
		GALE_ERROR("Error " << l_bufferDisplayError);
		std::vector<std::string> lines = etk::split(_data, '\n');
		for (size_t iii=0; iii<lines.size(); iii++) {
			GALE_ERROR("file " << (iii+1) << "|" << lines[iii]);
		}
		return false;
	}
	#ifdef GALE_SIMULATION_OPENGL_AVAILLABLE
		}
	#endif
	return true;
}


int64_t gale::openGL::program::create() {
	GLuint program = 0;
	#ifdef GALE_SIMULATION_OPENGL_AVAILLABLE
		if (s_simulationMode == false) {
	#endif
	program = glCreateProgram();
	if (program == 0) {
		GALE_ERROR("program creation return error ...");
		checkGlError("glCreateProgram", __LINE__);
		return -1;
	}
	#ifdef GALE_SIMULATION_OPENGL_AVAILLABLE
		} else {
			static uint32_t iii=0;
			program = iii++;
		}
	#endif
	GALE_DEBUG("Create program with oglID=" << program);
	return int64_t(program);
}

void gale::openGL::program::remove(int64_t& _prog) {
	if (_prog < 0) {
		return;
	}
	#ifdef GALE_SIMULATION_OPENGL_AVAILLABLE
		if (s_simulationMode == false) {
	#endif
	glDeleteProgram(GLuint(_prog));
	checkGlError("glDeleteProgram", __LINE__);
	#ifdef GALE_SIMULATION_OPENGL_AVAILLABLE
		}
	#endif
	_prog = -1;
}

bool gale::openGL::program::attach(int64_t _prog, int64_t _shader) {
	if (_prog < 0) {
		GALE_ERROR("wrong program ID");
		return false;
	}
	if (_shader < 0) {
		GALE_ERROR("wrong shader ID");
		return false;
	}
	#ifdef GALE_SIMULATION_OPENGL_AVAILLABLE
		if (s_simulationMode == false) {
	#endif
	glAttachShader(GLuint(_prog), GLuint(_shader));
	checkGlError("glAttachShader", __LINE__);
	#ifdef GALE_SIMULATION_OPENGL_AVAILLABLE
		}
	#endif
	return true;
}
bool gale::openGL::program::compile(int64_t _prog) {
	if (_prog < 0) {
		GALE_ERROR("wrong program ID");
		return false;
	}
	#ifdef GALE_SIMULATION_OPENGL_AVAILLABLE
		if (s_simulationMode == false) {
	#endif
	glLinkProgram(GLuint(_prog));
	checkGlError("glLinkProgram", __LINE__);
	GLint linkStatus = GL_FALSE;
	glGetProgramiv(GLuint(_prog), GL_LINK_STATUS, &linkStatus);
	checkGlError("glGetProgramiv", __LINE__);
	if (linkStatus != GL_TRUE) {
		GLint bufLength = 0;
		l_bufferDisplayError[0] = '\0';
		glGetProgramInfoLog(GLuint(_prog), LOG_OGL_INTERNAL_BUFFER_LEN, &bufLength, l_bufferDisplayError);
		char tmpLog[256];
		int32_t idOut=0;
		GALE_ERROR("Could not compile 'PROGRAM':");
		for (size_t iii=0; iii<LOG_OGL_INTERNAL_BUFFER_LEN ; iii++) {
			tmpLog[idOut] = l_bufferDisplayError[iii];
			if (    tmpLog[idOut] == '\n'
			     || tmpLog[idOut] == '\0'
			     || idOut >= 256) {
				tmpLog[idOut] = '\0';
				GALE_ERROR("     == > " << tmpLog);
				idOut=0;
			} else {
				idOut++;
			}
			if (l_bufferDisplayError[iii] == '\0') {
				break;
			}
		}
		if (idOut != 0) {
			tmpLog[idOut] = '\0';
			GALE_ERROR("     == > " << tmpLog);
		}
		return false;
	}
	#ifdef GALE_SIMULATION_OPENGL_AVAILLABLE
		}
	#endif
	return true;
}

int32_t gale::openGL::program::getAttributeLocation(int64_t _prog, const std::string& _name) {
	if (_prog < 0) {
		GALE_ERROR("wrong program ID");
		return -1;
	}
	if (_name.size() == 0) {
		GALE_ERROR("wrong name of attribure");
		return -1;
	}
	GLint val = 0;
	#ifdef GALE_SIMULATION_OPENGL_AVAILLABLE
		if (s_simulationMode == false) {
	#endif
	val = glGetAttribLocation(GLuint(_prog), _name.c_str());
	if (val < 0) {
		checkGlError("glGetAttribLocation", __LINE__);
		GALE_WARNING("glGetAttribLocation(\"" << _name << "\") = " << val);
	}
	#ifdef GALE_SIMULATION_OPENGL_AVAILLABLE
		}
	#endif
	return val;
}

int32_t gale::openGL::program::getUniformLocation(int64_t _prog, const std::string& _name) {
	if (_prog < 0) {
		GALE_ERROR("wrong program ID");
		return -1;
	}
	if (_name.size() == 0) {
		GALE_ERROR("wrong name of uniform");
		return -1;
	}
	GLint val = 0;
	#ifdef GALE_SIMULATION_OPENGL_AVAILLABLE
		if (s_simulationMode == false) {
	#endif
	val = glGetUniformLocation(GLuint(_prog), _name.c_str());
	if (val < 0) {
		checkGlError("glGetUniformLocation", __LINE__);
		GALE_WARNING("glGetUniformLocation(\"" << _name << "\") = " << val);
	}
	#ifdef GALE_SIMULATION_OPENGL_AVAILLABLE
		}
	#endif
	return val;
}

