/** @file
 * @author Edouard DUPIN
 * @copyright 2010, Edouard DUPIN, all right reserved
 * @license GPL v3 (see license file)
 */
//! [gale_sample_all]


#include <etk/types.h>
#include <gale/gale.h>
#include <gale/context/commandLine.h>
#include <test-debug/debug.h>

#include <gale/Application.h>
#include <gale/context/Context.h>
#include <gale/resource/Program.h>


class MainApplication : public gale::Application {
	private:
		ememory::SharedPtr<gale::resource::Program> m_GLprogram;
		int32_t m_GLPosition;
		int32_t m_GLMatrix;
		int32_t m_GLColor;
	public:
		void onCreate(gale::Context& _context) override {
			setSize(vec2(800, 600));
			m_GLprogram = gale::resource::Program::create("DATA:basic.prog");
			if (m_GLprogram != nullptr) {
				m_GLPosition = m_GLprogram->getAttribute("EW_coord3d");
				m_GLColor    = m_GLprogram->getAttribute("EW_color");
				m_GLMatrix   = m_GLprogram->getUniform("EW_MatrixTransformation");
			}
			TEST_INFO("==> Init APPL (END)");
		}
		void onDraw(gale::Context& _context) override {
			ivec2 size = getSize();
			// set the basic openGL view port: (position drawed in the windows)
			gale::openGL::setViewPort(ivec2(0,0),size);
			// Clear all the stacked matrix ...
			gale::openGL::setBasicMatrix(mat4());
			// clear background
			gale::openGL::clearColor(etk::color::yellow);
			// real clear request:
			gale::openGL::clear(gale::openGL::clearFlag_colorBuffer);
			// create a local matrix environnement.
			gale::openGL::push();
			
			mat4 tmpProjection = etk::matOrtho(-1, 1,
			                                   -1, 1,
			                                   -2, 2);
			// set internal matrix system:
			gale::openGL::setMatrix(tmpProjection);
			
			vec3 vertices[3]={ vec3(-0.5,-0.5,0),
			                   vec3(0,0.5,0),
			                   vec3(0.5,-0.5,0)
			                 };
			etk::Color<float> color[3] = { etk::color::red,
			                               etk::color::green,
			                               etk::color::blue
			                             };
			if (m_GLprogram == nullptr) {
				TEST_INFO("No shader ...");
				return;
			}
			//EWOL_DEBUG("    display " << m_coord.size() << " elements" );
			m_GLprogram->use();
			// set Matrix : translation/positionMatrix
			mat4 projMatrix = gale::openGL::getMatrix();
			mat4 camMatrix = gale::openGL::getCameraMatrix();
			mat4 tmpMatrix = projMatrix * camMatrix;
			m_GLprogram->uniformMatrix(m_GLMatrix, tmpMatrix);
			// position :
			m_GLprogram->sendAttribute(m_GLPosition, 3/*x,y,z,unused*/, vertices, 4*sizeof(float));
			// color :
			m_GLprogram->sendAttribute(m_GLColor, 4/*r,g,b,a*/, color, 4*sizeof(float));
			// Request the draw od the elements : 
			gale::openGL::drawArrays(gale::openGL::renderMode::triangle, 0, 3 /*number of points*/);
			m_GLprogram->unUse();
			// Restore context of matrix
			gale::openGL::pop();
		}
		void onPointer(enum gale::key::type _type,
		               int32_t _pointerID,
		               const vec2& _pos,
		               gale::key::status _state) override {
			TEST_INFO("input event: type=" << _type);
			TEST_INFO("               id=" << _pointerID);
			TEST_INFO("              pos=" << _pos);
			TEST_INFO("            state=" << _state);
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

//! [gale_declare_main]
/**
 * @brief Main of the program (This can be set in every case, but it is not used in Andoid...).
 * @param std IO
 * @return std IO
 */
int main(int _argc, const char *_argv[]) {
	return gale::run(new MainApplication(), _argc, _argv);
}
//! [gale_declare_main]



//! [gale_sample_all]
