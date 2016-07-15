/** @file
 * @author Edouard DUPIN
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license APACHE v2.0 (see license file)
 */
#pragma once

#include <list>
#include <vector>
#include <etk/types.h>
#include <gale/debug.h>
#include <gale/resource/Resource.h>

namespace gale {
	namespace resource {
		class Manager{
			private:
				std::list<ememory::WeakPtr<gale::Resource>> m_resourceList;
				std::vector<ememory::SharedPtr<gale::Resource>> m_resourceListToUpdate;
				bool m_contextHasBeenRemoved;
				std::recursive_mutex m_mutex;
			public:
				/**
				 * @brief initialize the internal variable
				 */
				Manager();
				/**
				 * @brief Uninitiamize the resource manager, free all resources previously requested
				 * @note when not free  == > generate warning, because the segfault can appear after...
				 */
				virtual ~Manager();
				/**
				 * @brief remove all resources (un-init) out of the destructor (due to the system implementation)
				 */
				void unInit();
				/**
				 * @brief display in the log all the resources loaded ...
				 */
				void display();
				/**
				 * @brief Reload all resources from files, and send there in openGL card if needed.
				 * @note If file is reference at THEME:XXX:filename if the Theme change the file will reload the newOne
				 */
				void reLoadResources();
				/**
				 * @brief Call by the system to send all the needed data on the graphic card chen they change ...
				 * @param[in] _object The resources that might be updated
				 */
				void update(const ememory::SharedPtr<gale::Resource>& _object);
				/**
				 * @brief Call by the system chen the openGL Context has been unexpectially removed  == > This reload all the texture, VBO and other ....
				 */
				void updateContext();
				/**
				 * @brief This is to inform the resources manager that we have no more openGl context ...
				 */
				void contextHasBeenDestroyed();
			public:
				// internal API to extent eResources in extern Soft
				ememory::SharedPtr<gale::Resource> localKeep(const std::string& _filename);
				void localAdd(const ememory::SharedPtr<gale::Resource>& _object);
				virtual void cleanInternalRemoved();
		};
	}
}

