/**
 * @author Edouard DUPIN
 * 
 * @copyright 2010, Edouard DUPIN, all right reserved
 * 
 * @license GPL v3 (see license file)
 */


#include <etk/types.h>
#include <gale/gale.h>
#include <gale/context/commandLine.h>

#include <gale/Application.h>
#include <gale/context/Context.h>


class MainApplication : public gale::Application {
	public:
		void onCreate(gale::Context& _context) {
			setSize(vec2(800, 600));
			std::cout << "==> Init APPL (END)" << std::endl;
		}
		void onDraw(gale::Context& _context) {
			std::cout << "Draw (start)" << std::endl;
			
			std::cout << "Draw (end)" << std::endl;
		}
};

/**
 * @brief Main of the program (This can be set in every case, but it is not used in Andoid...).
 * @param std IO
 * @return std IO
 */
int main(int _argc, const char *_argv[]) {
	return gale::run(new MainApplication(), _argc, _argv);
}


