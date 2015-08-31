/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license APACHE v2.0 (see license file)
 */

#ifndef __GALE_CONTEXT_LOOP_ACTION_H__
#define __GALE_CONTEXT_LOOP_ACTION_H__

#include <etk/os/Fifo.h>
#include <gale/debug.h>
#include <gale/gale.h>
#include <gale/key/key.h>
#include <memory>
#define MAX_MANAGE_INPUT (15)

namespace gale {
	namespace context {
		
		class LoopAction : public std::enable_shared_from_this<LoopAction> {
			protected:
				int64_t m_timestamp; //!< time of the signal is emit (used for simulation)
			public:
				LoopAction();
				virtual ~LoopAction();
				virtual void doAction(gale::Context& _context) = 0;
				virtual std::string createString() = 0;
		};
		/**
		 * @brief Create action from the simulation string line
		 * @param[in] _lineToParse Simulation string line
		 * @return Created action.
		 */
		std::shared_ptr<LoopAction> createAction(const std::string& _lineToParse);
		/**
		 * @brief Add a factory to create an event from a specific type
		 * @param[in] _type Type of the action
		 * @param[in] _func function to call to create
		 * @return Created action.
		 */
		void addFactory(const std::string& _type, const std::function<std::shared_ptr<LoopAction>(const std::string&)>& _func);
		
		class LoopActionInit : public LoopAction {
			public:
				LoopActionInit() {};
				virtual void doAction(gale::Context& _context);
				virtual std::string createString();
		};
		
		class LoopActionResize : public LoopAction {
			protected:
				vec2 m_size;
			public:
				LoopActionResize(const vec2& _size);
				virtual void doAction(gale::Context& _context);
				virtual std::string createString();
		};
		
		class LoopActionView : public LoopAction {
			protected:
				bool m_show;
			public:
				LoopActionView(bool _show);
				virtual void doAction(gale::Context& _context);
				virtual std::string createString();
		};
		
		class LoopActionRecalculateSize : public LoopAction {
			public:
				LoopActionRecalculateSize() {};
				virtual void doAction(gale::Context& _context);
				virtual std::string createString();
		};
		
		class LoopActionInput : public LoopAction {
			private:
				enum gale::key::type m_type;
				enum gale::key::status m_status;
				int32_t m_pointerID;
				const vec2& m_pos;
			public:
				LoopActionInput(enum gale::key::type _type, enum gale::key::status _status, int32_t _pointerID, const vec2& _pos);
				virtual void doAction(gale::Context& _context);
				virtual std::string createString();
		};
		
		class LoopActionKeyboard : public LoopAction {
			private:
				gale::key::Special m_special;
				enum gale::key::keyboard m_type;
				enum gale::key::status m_state;
				char32_t m_char;
			public:
				LoopActionKeyboard(const gale::key::Special& _special, enum gale::key::keyboard _type, enum gale::key::status _state, char32_t _char=u32char::Null);
				virtual void doAction(gale::Context& _context);
				virtual std::string createString();
		};
		
		class LoopActionClipboardArrive : public LoopAction {
			private:
				enum gale::context::clipBoard::clipboardListe m_id;
			public:
				LoopActionClipboardArrive(enum gale::context::clipBoard::clipboardListe _id);
				virtual void doAction(gale::Context& _context);
				virtual std::string createString();
		};
	}
}

#endif