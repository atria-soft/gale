/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license APACHE v2.0 (see license file)
 */

#ifndef __GALE_CLIPBOARD_H__
#define __GALE_CLIPBOARD_H__

#include <gale/debug.h>

// TODO : Remove this ... ==> set it in the context ....
namespace gale {
	namespace context {
		namespace clipBoard {
			enum clipboardListe {
				clipboard0,         //!< internal clipboard 0
				clipboard1,         //!< internal clipboard 1
				clipboard2,         //!< internal clipboard 2
				clipboard3,         //!< internal clipboard 3
				clipboard4,         //!< internal clipboard 4
				clipboard5,         //!< internal clipboard 5
				clipboard6,         //!< internal clipboard 6
				clipboard7,         //!< internal clipboard 7
				clipboard8,         //!< internal clipboard 8
				clipboard9,         //!< internal clipboard 9
				clipboardStd,       //!< External clipboard represent the Copy/Cut/Past buffer
				clipboardSelection, //!< External or internal clipboard depending on the OS, represent the middle button
				clipboardCount,     //!< Total number of clipboard
			};
			
			/**
			 * @brief set the string data on a specific clipboard. The Gui system is notify that the clipboard "SELECTION" and "COPY" are change
			 * @param[in] _clipboardID Select the specific ID of the clipboard
			 * @param[in] _data The string that might be send to the clipboard
			 */
			void set(enum gale::context::clipBoard::clipboardListe _clipboardID, const std::string& _data);
			/**
			 * @brief Call system to request the current clipboard.
			 * @note Due to some system that manage the clipboard request asynchronous (like X11) and gale managing the system with only one thread,
			 *       we need the call the system to send us the buffer, this is really ambigous, but the widget (who has focus) receive the 
			 *       notification of the arrival of this buffer id
			 * @param[in] _clipboardID the needed clipboard ID
			 */
			void request(enum gale::context::clipBoard::clipboardListe _clipboardID);
			/**
			 * @brief set the gale internal buffer (no notification at the GUI). This fuction might be use by the 
			 *        Gui abstraction to set the buffer we receive. The end user must not use it.
			 * @param[in] _clipboardID selected clipboard ID
			 * @param[in] _data new buffer data
			 */
			void setSystem(enum gale::context::clipBoard::clipboardListe _clipboardID,const std::string& _data);
			/**
			 * @brief get the gale internal buffer of the curent clipboard. The end user can use it when he receive the event in 
			 *        the widget : @ref onEventClipboard  == > we can nothe this function is the only one which permit it.
			 * @note if we call this fuction withoutcallin @ref gale::context::clipBoard::Request, we only get the previous clipboard
			 * @param[in] _clipboardID selected clipboard ID
			 * @return the requested buffer
			 */
			const std::string& get(enum gale::context::clipBoard::clipboardListe _clipboardID);
			
			// internal section
			
			/**
			 * @brief initialize the clipboard system (done by gale)
			 */
			void init();
			/**
			 * @brief Un-Initialize the clipboard system (done by gale)
			 */
			void unInit();
		};
	};
	/**
	 * @brief Debug operator To display the curent element in a Human redeable information
	 */
	std::ostream& operator <<(std::ostream& _os, const enum gale::context::clipBoard::clipboardListe _obj);
};

#endif


