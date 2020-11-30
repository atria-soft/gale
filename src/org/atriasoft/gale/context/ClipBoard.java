package org.atriasoft.gale.context;

public class ClipBoard {
	private ClipBoard() {}
	/**
	 * @brief set the string data on a specific clipboard. The Gui system is notify that the clipboard "SELECTION" and "COPY" are change
	 * @param _clipboardID Select the specific ID of the clipboard
	 * @param _data The string that might be send to the clipboard
	 */
	public static void set(ClipboardList clipboardID, String data) {
	}
	/**
	 * @brief Call system to request the current clipboard.
	 * @note Due to some system that manage the clipboard request asynchronous (like X11) and gale managing the system with only one thread,
	 *       we need the call the system to send us the buffer, this is really ambigous, but the widget (who has focus) receive the 
	 *       notification of the arrival of this buffer id
	 * @param _clipboardID the needed clipboard ID
	 */
	public static void request(ClipboardList clipboardID) {
	}
	/**
	 * @brief set the gale internal buffer (no notification at the GUI). This fuction might be use by the 
	 *        Gui abstraction to set the buffer we receive. The end user must not use it.
	 * @param _clipboardID selected clipboard ID
	 * @param _data new buffer data
	 */
	public static void setSystem(ClipboardList clipboardID, String data) {
	}
	/**
	 * @brief get the gale internal buffer of the curent clipboard. The end user can use it when he receive the event in 
	 *        the widget : @ref onEventClipboard  == > we can nothe this function is the only one which permit it.
	 * @note if we call this fuction withoutcallin @ref gale::context::clipBoard::Request, we only get the previous clipboard
	 * @param _clipboardID selected clipboard ID
	 * @return the requested buffer
	 */
	public static String get(ClipboardList clipboardID) {
		return null;
	}
	
}
