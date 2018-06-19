/** @file
 * @author Edouard DUPIN
 * @copyright 2010, Edouard DUPIN, all right reserved
 * @license GPL v3 (see license file)
 */
//! [gale_sample_all]


#include <etk/types.hpp>
#include <gale/gale.hpp>
#include <gale/context/commandLine.hpp>
#include <test-debug/debug.hpp>

#include <gale/Application.hpp>
#include <gale/context/Context.hpp>
#include <gale/resource/Program.hpp>


#define GALE_SAMPLE_VBO_VERTICES  (0)
#define GALE_SAMPLE_VBO_COLOR     (1)
namespace appl {
	class MainApplication : public gale::Application {
		private:
			ememory::SharedPtr<gale::resource::Program> m_GLprogram;
			int32_t m_GLPosition;
			int32_t m_GLMatrix;
			int32_t m_GLColor;
			float m_angle;
			ememory::SharedPtr<gale::resource::VirtualBufferObject> m_verticesVBO;
		public:
			void onCreate(gale::Context& _context) override {
				setSize(vec2(800, 600));
				m_angle = 0.0f;
				m_GLprogram = gale::resource::Program::create("DATA:basic.prog");
				if (m_GLprogram != null) {
					m_GLPosition = m_GLprogram->getAttribute("EW_coord3d");
					m_GLColor    = m_GLprogram->getAttribute("EW_color");
					m_GLMatrix   = m_GLprogram->getUniform("EW_MatrixTransformation");
				}
				// this is the properties of the buffer requested : "r"/"w" + "-" + buffer type "f"=flaot "i"=integer
				m_verticesVBO = gale::resource::VirtualBufferObject::create(5);
				if (m_verticesVBO == null) {
					TEST_ERROR("can not instanciate VBO ...");
					return;
				}
				// TO facilitate some debugs we add a name of the VBO:
				m_verticesVBO->setName("[VBO] of basic SAMPLE");
				m_verticesVBO->pushOnBuffer(GALE_SAMPLE_VBO_VERTICES, vec3(-0.5,-0.5,0));
				m_verticesVBO->pushOnBuffer(GALE_SAMPLE_VBO_VERTICES, vec3(0,0.5,0));
				m_verticesVBO->pushOnBuffer(GALE_SAMPLE_VBO_VERTICES, vec3(0.5,-0.5,0));
				m_verticesVBO->pushOnBuffer(GALE_SAMPLE_VBO_COLOR, etk::Color<float>(etk::color::red));
				m_verticesVBO->pushOnBuffer(GALE_SAMPLE_VBO_COLOR, etk::Color<float>(etk::color::green));
				m_verticesVBO->pushOnBuffer(GALE_SAMPLE_VBO_COLOR, etk::Color<float>(etk::color::blue));
				// update all the VBO elements ...
				m_verticesVBO->flush();
				TEST_INFO("==> Init APPL (END)");
			}
			void onDraw(gale::Context& _context) override {
				m_angle += 0.01;
				TEST_INFO("==> appl Draw ...");
				ivec2 size = getSize();
				// set the basic openGL view port: (position drawed in the windows)
				gale::openGL::setViewPort(ivec2(0,0),size);
				// Clear all the stacked matrix ...
				gale::openGL::setBasicMatrix(mat4());
				// clear background
				etk::Color<float,4> bgColor = etk::color::yellow;
				bgColor.setA(0.75);
				gale::openGL::clearColor(bgColor);
				// real clear request:
				gale::openGL::clear(gale::openGL::clearFlag_colorBuffer);
				// create a local matrix environnement.
				gale::openGL::push();
				
				mat4 tmpProjection = etk::matOrtho(-1, 1,
				                                   -1, 1,
				                                   -2, 2);
				// set internal matrix system:
				gale::openGL::setMatrix(tmpProjection);
				#if 0
					vec3 vertices[3]={ vec3(-0.5,-0.5,0),
					                   vec3(0,0.5,0),
					                   vec3(0.5,-0.5,0)
					                 };
					etk::Color<float> color[3] = { etk::color::red,
					                               etk::color::green,
					                               etk::color::blue
					                             };
				#endif
				if (m_GLprogram == null) {
					TEST_INFO("No shader ...");
					return;
				}
				//EWOL_DEBUG("    display " << m_coord.size() << " elements" );
				m_GLprogram->use();
				
				// set Matrix : translation/positionMatrix
				mat4 projMatrix = gale::openGL::getMatrix() * etk::matRotate(vec3(0,0,1),m_angle);
				mat4 camMatrix = gale::openGL::getCameraMatrix();
				mat4 tmpMatrix = projMatrix * camMatrix;
				
				m_GLprogram->uniformMatrix(m_GLMatrix, tmpMatrix);
				#if 1
					// position:
					m_GLprogram->sendAttributePointer(m_GLPosition, m_verticesVBO, GALE_SAMPLE_VBO_VERTICES);
					// color:
					m_GLprogram->sendAttributePointer(m_GLColor, m_verticesVBO, GALE_SAMPLE_VBO_COLOR);
				#else
					// position:
					m_GLprogram->sendAttribute(m_GLPosition, 3/*x,y,z,unused*/, vertices, 4*sizeof(float));
					// color:
					m_GLprogram->sendAttribute(m_GLColor, 4/*r,g,b,a*/, color, 4*sizeof(float));
				#endif
				
				// Request the draw od the elements:
				gale::openGL::drawArrays(gale::openGL::renderMode::triangle, 0, 3 /*number of points*/);
				m_GLprogram->unUse();
				// Restore context of matrix
				gale::openGL::pop();
			}
			void onPointer(enum gale::key::type _type,
			               int32_t _pointerID,
			               const vec2& _pos,
			               gale::key::status _state) override {
				/*
				TEST_INFO("input event: type=" << _type);
				TEST_INFO("               id=" << _pointerID);
				TEST_INFO("              pos=" << _pos);
				TEST_INFO("            state=" << _state);
				*/
			}
			void onKeyboard(const gale::key::Special& _special,
			                enum gale::key::keyboard _type,
			                char32_t _value,
			                gale::key::status _state) override {
				TEST_INFO("Keyboard event: special=" << _special);
				TEST_INFO("                   type=" << _type);
				TEST_INFO("                  value=" << uint32_t(_value));
				TEST_INFO("                  state=" << _state);
			}
	};
}
//! [gale_declare_main]
/**
 * @brief Main of the program (This can be set in every case, but it is not used in Andoid...).
 * @param std IO
 * @return std IO
 */
int main(int _argc, const char *_argv[]) {
	return gale::run(ETK_NEW(appl::MainApplication), _argc, _argv);
}
//! [gale_declare_main]



//! [gale_sample_all]
