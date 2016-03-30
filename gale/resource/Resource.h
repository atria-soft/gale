/** @file
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license APACHE v2.0 (see license file)
 */
#pragma once

#include <mutex>
#include <memory>
#include <etk/types.h>
#include <gale/debug.h>

#define MAX_RESOURCE_LEVEL (5)

#define DECLARE_RESOURCE_FACTORY(className) \
	template<typename ... T> static std::shared_ptr<className> create( T&& ... all ) { \
		std::shared_ptr<className> resource(new className()); \
		if (resource == nullptr) { \
			GALE_ERROR("Factory resource error"); \
			return nullptr; \
		} \
		resource->init(std::forward<T>(all)... ); \
		if (resource->resourceHasBeenCorectlyInit() == false) { \
			GALE_CRITICAL("resource Is not correctly init : " << #className ); \
		} \
		getManager().localAdd(resource); \
		return resource; \
	}

#define DECLARE_RESOURCE_NAMED_FACTORY(className) \
	template<typename ... T> static std::shared_ptr<className> create(const std::string& _name, T&& ... all ) { \
		std::shared_ptr<className> resource; \
		std::shared_ptr<gale::Resource> resource2; \
		if (_name != "" && _name != "---") { \
			resource2 = getManager().localKeep(_name); \
		} \
		if (resource2 != nullptr) { \
			resource = std::dynamic_pointer_cast<className>(resource2); \
			if (resource == nullptr) { \
				GALE_CRITICAL("Request resource file : '" << _name << "' With the wrong type (dynamic cast error)"); \
				return nullptr; \
			} \
		} \
		if (resource != nullptr) { \
			return resource; \
		} \
		resource = std::shared_ptr<className>(new className()); \
		if (resource == nullptr) { \
			GALE_ERROR("allocation error of a resource : " << _name); \
			return nullptr; \
		} \
		resource->init(_name, std::forward<T>(all)... ); \
		if (resource->resourceHasBeenCorectlyInit() == false) { \
			GALE_CRITICAL("resource Is not correctly init : " << #className ); \
		} \
		getManager().localAdd(resource); \
		return resource; \
	}

#define DECLARE_RESOURCE_SINGLE_FACTORY(className,uniqueName) \
	template<typename ... T> static std::shared_ptr<className> create(T&& ... all ) { \
		std::shared_ptr<className> resource; \
		std::shared_ptr<gale::Resource> resource2 = getManager().localKeep(uniqueName); \
		if (resource2 != nullptr) { \
			resource = std::dynamic_pointer_cast<className>(resource2); \
			if (resource == nullptr) { \
				GALE_CRITICAL("Request resource file : '" << uniqueName << "' With the wrong type (dynamic cast error)"); \
				return nullptr; \
			} \
		} \
		if (resource != nullptr) { \
			return resource; \
		} \
		resource = std::shared_ptr<className>(new className()); \
		if (resource == nullptr) { \
			GALE_ERROR("allocation error of a resource : " << uniqueName); \
			return nullptr; \
		} \
		resource->init(uniqueName, std::forward<T>(all)... ); \
		if (resource->resourceHasBeenCorectlyInit() == false) { \
			GALE_CRITICAL("resource Is not correctly init : " << #className ); \
		} \
		getManager().localAdd(resource); \
		return resource; \
	}

namespace gale {
	namespace resource {
		class Manager;
	};
	/**
	 * @brief A Resource is a generic interface to have an instance that have things that can be used by many people, ad have some hardware dependency.
	 * For example of resources :
	 * :** Shaders: openGL display interface.
	 * :** Texture: openGL imega interface.
	 * :** Font: Single file interface to store many glyphe ==> reduce the number of parallele loaded files.
	 * :** ConfigFile: simple widget configuration files
	 * :** ...
	 */
	class Resource : public std::enable_shared_from_this<gale::Resource> {
		protected:
			std::recursive_mutex m_mutex;
		protected:
			/**
			 * @brief generic protected contructor (use factory to create this class)
			 */
			Resource();
			/**
			 * @brief Initialisation of the class and previous classes.
			 * @param[in] _name Name of the resource.
			 */
			void init();
			//! @previous
			void init(const std::string& _name);
		public:
			//! geenric destructor
			virtual ~Resource() {
				
			};
		private:
			size_t m_id; //!< unique ID definition
		public:
			size_t getId() {
				return m_id;
			}
		private:
			bool m_resourceHasBeenInit; //!< Know if the init function has bben called
		public:
			bool resourceHasBeenCorectlyInit() {
				return m_resourceHasBeenInit;
			}
		private:
			std::vector<const char*> m_listType;
		public:
			/**
			 * @brief get the current type of the Resource
			 * @return the last type name of the element
			 */
			const char* const getType();
			/**
			 * @brief Get the herarchic of the Resource type.
			 * @return descriptive string.
			 */
			std::string getTypeDescription();
			/**
			 * @brief check  if the element herited from a specific type
			 * @param[in] _type Type to check.
			 * @return true if the element is compatible.
			 */
			bool isTypeCompatible(const std::string& _type);
		protected:
			/**
			 * @brief Add a type of the list of Object.
			 * @param[in] _type new type to add.
			 */
			void addResourceType(const char* _type);
		protected:
			std::string m_name; //!< name of the resource ...
		public:
			/**
			 * @brief get the resource name
			 * @return The requested name
			 */
			const std::string& getName() const {
				return m_name;
			};
			/**
			 * @brief get the resource name
			 * @param[in] _name The new name
			 */
			void setName(const std::string& _name) {
				m_name = _name;
			};
		protected:
			uint8_t m_resourceLevel; //!< Level of the resource ==> for update priority [0..5] 0 must be update first.
		public:
			/**
			 * @brief Get the current resource level;
			 * @return value in [0..5]
			 */
			uint8_t getResourceLevel() {
				return m_resourceLevel;
			};
			/**
			 * @brief Call when need to send data on the harware (openGL)
			 * @note This is done asynchronously with the create of the Resource.
			 * @return true The context is updated
			 * @return false The context is not updated
			 */
			virtual bool updateContext();
			/**
			 * @brief The current OpenGl context is removing ==> remove yout own system data
			 */
			virtual void removeContext();
			/**
			 * @brief The notification of the Context removing is too late, we have no more acces on the OpenGl context (thank you Android).
			 * Juste update your internal state
			 */
			virtual void removeContextToLate();
			/**
			 * @brief User request the reload of all resources (usefull when the file depend on DATA:GUI:xxx ...
			 */
			virtual void reload();
		protected:
			/**
			 * @brief Get the current resource Manager
			 */
			static gale::resource::Manager& getManager();
	};
}

#include <gale/resource/Manager.h>

