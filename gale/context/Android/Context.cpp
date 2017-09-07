/** @file
 * @author Edouard DUPIN, Kevin BILLONNEAU
 * @copyright 2011, Edouard DUPIN, all right reserved
 * @license MPL v2.0 (see license file)
 */

#include <jni.h>
#include <sys/time.h>
#include <time.h>
#include <cstdint>
#include <pthread.h>
#include <ethread/Mutex.hpp>
#include <gale/debug.hpp>
#include <gale/context/Context.hpp>
#include <gale/Dimension.hpp>
/* include auto generated file */
#include <org_gale_GaleConstants.h>
#include <jvm-basics/jvm-basics.hpp>

// jni doc : /usr/lib/jvm/java-1.6.0-openjdk/include

ethread::Mutex g_interfaceMutex;


class AndroidContext : public gale::Context {
	public:
		enum application {
			appl_unknow,
			appl_application,
			appl_wallpaper
		};
	private:
		enum application m_javaApplicationType;
		// get a resources from the java environement : 
		JNIEnv* m_JavaVirtualMachinePointer; //!< the JVM
		jclass m_javaClassGale; //!< main activity class (android ...)
		jclass m_javaClassGaleCallback;
		jobject m_javaObjectGaleCallback;
		jmethodID m_javaMethodGaleCallbackStop; //!< Stop function identifier
		jmethodID m_javaMethodGaleCallbackEventNotifier; //!< basic methode to call ...
		jmethodID m_javaMethodGaleCallbackKeyboardUpdate; //!< basic methode to call ...
		jmethodID m_javaMethodGaleCallbackOrientationUpdate;
		jmethodID m_javaMethodGaleActivitySetTitle;
		jmethodID m_javaMethodGaleActivityOpenURI;
		jmethodID m_javaMethodGaleActivitySetClipBoardString;
		jmethodID m_javaMethodGaleActivityGetClipBoardString;
		jclass m_javaDefaultClassString; //!< default string class
		int32_t m_currentHeight;
		gale::key::Special m_guiKeyBoardSpecialKeyMode;//!< special key of the android system :
		bool m_clipBoardOwnerStd;
	private:
		bool safeInitMethodID(jmethodID& _mid, jclass& _cls, const char* _name, const char* _sign) {
			_mid = m_JavaVirtualMachinePointer->GetMethodID(_cls, _name, _sign);
			if(_mid == nullptr) {
				GALE_ERROR("C->java : Can't find the method " << _name);
				/* remove access on the virtual machine : */
				m_JavaVirtualMachinePointer = nullptr;
				return false;
			}
			return true;
		}
	public:
		AndroidContext(gale::Application* _application, JNIEnv* _env, jclass _classBase, jobject _objCallback, enum application _typeAPPL) :
		  gale::Context(_application),
		  m_javaApplicationType(_typeAPPL),
		  m_JavaVirtualMachinePointer(nullptr),
		  m_javaClassGale(0),
		  m_javaClassGaleCallback(0),
		  m_javaObjectGaleCallback(0),
		  m_javaMethodGaleCallbackStop(0),
		  m_javaMethodGaleCallbackEventNotifier(0),
		  m_javaMethodGaleCallbackKeyboardUpdate(0),
		  m_javaMethodGaleCallbackOrientationUpdate(0),
		  m_javaMethodGaleActivitySetTitle(0),
		  m_javaMethodGaleActivityOpenURI(0),
		  m_javaMethodGaleActivitySetClipBoardString(0),
		  m_javaMethodGaleActivityGetClipBoardString(0),
		  m_javaDefaultClassString(0),
		  m_currentHeight(0),
		  m_clipBoardOwnerStd(false) {
			GALE_DEBUG("*******************************************");
			if (m_javaApplicationType == appl_application) {
				GALE_DEBUG("** set JVM Pointer (application)         **");
			} else {
				GALE_DEBUG("** set JVM Pointer (LiveWallpaper)       **");
			}
			GALE_DEBUG("*******************************************");
			m_JavaVirtualMachinePointer = _env;
			// get default needed all time elements : 
			if (nullptr != m_JavaVirtualMachinePointer) {
				GALE_DEBUG("C->java : try load org/gale/Gale class");
				m_javaClassGale = m_JavaVirtualMachinePointer->FindClass("org/gale/Gale" );
				if (m_javaClassGale == 0) {
					GALE_ERROR("C->java : Can't find org/gale/Gale class");
					// remove access on the virtual machine : 
					m_JavaVirtualMachinePointer = nullptr;
					return;
				}
				/* The object field extends Activity and implement GaleCallback */
				m_javaClassGaleCallback = m_JavaVirtualMachinePointer->GetObjectClass(_objCallback);
				if(m_javaClassGaleCallback == nullptr) {
					GALE_ERROR("C->java : Can't find org/gale/GaleCallback class");
					// remove access on the virtual machine : 
					m_JavaVirtualMachinePointer = nullptr;
					return;
				}
				bool functionCallbackIsMissing = false;
				bool ret= false;
				ret = safeInitMethodID(m_javaMethodGaleActivitySetTitle,
				                       m_javaClassGaleCallback,
				                       "titleSet",
				                       "(Ljava/lang/String;)V");
				if (ret == false) {
					jvm_basics::checkExceptionJavaVM(_env);
					GALE_ERROR("system can not start without function : titleSet");
					functionCallbackIsMissing = true;
				}
				ret = safeInitMethodID(m_javaMethodGaleActivityOpenURI,
				                       m_javaClassGaleCallback,
				                       "openURI",
				                       "(Ljava/lang/String;)V");
				if (ret == false) {
					jvm_basics::checkExceptionJavaVM(_env);
					GALE_ERROR("system can not start without function : openURI");
					functionCallbackIsMissing = true;
				}
				
				ret = safeInitMethodID(m_javaMethodGaleCallbackStop,
				                       m_javaClassGaleCallback,
				                       "stop",
				                       "()V");
				if (ret == false) {
					jvm_basics::checkExceptionJavaVM(_env);
					GALE_ERROR("system can not start without function : stop");
					functionCallbackIsMissing = true;
				}
				
				ret = safeInitMethodID(m_javaMethodGaleCallbackEventNotifier,
				                       m_javaClassGaleCallback,
				                       "eventNotifier",
				                       "([Ljava/lang/String;)V");
				if (ret == false) {
					jvm_basics::checkExceptionJavaVM(_env);
					GALE_ERROR("system can not start without function : eventNotifier");
					functionCallbackIsMissing = true;
				}
				
				ret = safeInitMethodID(m_javaMethodGaleCallbackKeyboardUpdate,
				                       m_javaClassGaleCallback,
				                       "keyboardUpdate",
				                       "(Z)V");
				if (ret == false) {
					jvm_basics::checkExceptionJavaVM(_env);
					GALE_ERROR("system can not start without function : keyboardUpdate");
					functionCallbackIsMissing = true;
				}
				
				ret = safeInitMethodID(m_javaMethodGaleCallbackOrientationUpdate,
				                       m_javaClassGaleCallback,
				                       "orientationUpdate",
				                       "(I)V");
				if (ret == false) {
					jvm_basics::checkExceptionJavaVM(_env);
					GALE_ERROR("system can not start without function : orientationUpdate");
					functionCallbackIsMissing = true;
				}
				
				ret = safeInitMethodID(m_javaMethodGaleActivitySetClipBoardString,
				                       m_javaClassGaleCallback,
				                       "setClipBoardString",
				                       "(Ljava/lang/String;)V");
				if (ret == false) {
					jvm_basics::checkExceptionJavaVM(_env);
					GALE_ERROR("system can not start without function : setClipBoardString");
					functionCallbackIsMissing = true;
				}
				
				ret = safeInitMethodID(m_javaMethodGaleActivityGetClipBoardString,
				                       m_javaClassGaleCallback,
				                       "getClipBoardString",
				                       "()Ljava/lang/String;");
				if (ret == false) {
					jvm_basics::checkExceptionJavaVM(_env);
					GALE_ERROR("system can not start without function : getClipBoardString");
					functionCallbackIsMissing = true;
				}
				
				m_javaObjectGaleCallback = _env->NewGlobalRef(_objCallback);
				//javaObjectGaleCallbackAndActivity = objCallback;
				if (m_javaObjectGaleCallback == nullptr) {
					functionCallbackIsMissing = true;
				}
				
				m_javaDefaultClassString = m_JavaVirtualMachinePointer->FindClass("java/lang/String" );
				if (m_javaDefaultClassString == 0) {
					GALE_ERROR("C->java : Can't find java/lang/String" );
					// remove access on the virtual machine : 
					m_JavaVirtualMachinePointer = nullptr;
					functionCallbackIsMissing = true;
				}
				if (functionCallbackIsMissing == true) {
					GALE_CRITICAL(" mission one function ==> system can not work withut it...");
				}
			}
		}
		
		~AndroidContext() {
			// TODO ...
		}
		
		void unInit(JNIEnv* _env) {
			_env->DeleteGlobalRef(m_javaObjectGaleCallback);
			m_javaObjectGaleCallback = nullptr;
		}
		
		int32_t run() {
			// might never be called !!!
			return -1;
		}
		
		void stop() {
			GALE_DEBUG("C->java : send message to the java : STOP REQUESTED");
			int status;
			if(!java_attach_current_thread(&status)) {
				return;
			}
			//Call java ...
			m_JavaVirtualMachinePointer->CallVoidMethod(m_javaObjectGaleCallback, m_javaMethodGaleCallbackStop);
			// manage execption : 
			jvm_basics::checkExceptionJavaVM(m_JavaVirtualMachinePointer);
			java_detach_current_thread(status);
		}
		
		void clipBoardGet(enum gale::context::clipBoard::clipboardListe _clipboardID) {
			// this is to force the local system to think we have the buffer
			// TODO : remove this 2 line when code will be writen
			m_clipBoardOwnerStd = true;
			switch (_clipboardID) {
				case gale::context::clipBoard::clipboardSelection:
					// NOTE : Windows does not support the middle button the we do it internaly
					// just transmit an event , we have the data in the system
					OS_ClipBoardArrive(_clipboardID);
					break;
				case gale::context::clipBoard::clipboardStd:
					if (false == m_clipBoardOwnerStd) {
						// generate a request TO the OS
						// TODO : Send the message to the OS "We disire to receive the copy buffer ...
					} else {
						// just transmit an event , we have the data in the system
						OS_ClipBoardArrive(_clipboardID);
					}
					break;
				default:
					GALE_ERROR("Request an unknow ClipBoard ...");
					break;
			}
		}
		
		void clipBoardSet(enum gale::context::clipBoard::clipboardListe _clipboardID) {
			switch (_clipboardID) {
				case gale::context::clipBoard::clipboardSelection:
					// NOTE : nothing to do : Windows deas ot supported Middle button
					break;
				case gale::context::clipBoard::clipboardStd:
					
					// Request the clipBoard :
					GALE_DEBUG("C->java : set clipboard");
					if (m_javaApplicationType == appl_application) {
						int status;
						if(!java_attach_current_thread(&status)) {
							return;
						}
						//Call java ...
						jstring data = m_JavaVirtualMachinePointer->NewStringUTF(gale::context::clipBoard::get(_clipboardID).c_str());
						m_JavaVirtualMachinePointer->CallVoidMethod(m_javaObjectGaleCallback, m_javaMethodGaleActivityGetClipBoardString, data);
						m_JavaVirtualMachinePointer->DeleteLocalRef(data);
						// manage execption : 
						jvm_basics::checkExceptionJavaVM(m_JavaVirtualMachinePointer);
						java_detach_current_thread(status);
					} else {
						GALE_ERROR("C->java : can not set clipboard");
					}
					break;
				default:
					GALE_ERROR("Request an unknow ClipBoard ...");
					break;
			}
		}
	private:
		bool java_attach_current_thread(int *_rstatus) {
			GALE_DEBUG("C->java : call java");
			if (jvm_basics::getJavaVM() == nullptr) {
				GALE_ERROR("C->java : JVM not initialised");
				m_JavaVirtualMachinePointer = nullptr;
				return false;
			}
			*_rstatus = jvm_basics::getJavaVM()->GetEnv((void **) &m_JavaVirtualMachinePointer, JNI_VERSION_1_6);
			if (*_rstatus == JNI_EDETACHED) {
				JavaVMAttachArgs lJavaVMAttachArgs;
				lJavaVMAttachArgs.version = JNI_VERSION_1_6;
				lJavaVMAttachArgs.name = "GaleNativeThread";
				lJavaVMAttachArgs.group = nullptr; 
				int status = jvm_basics::getJavaVM()->AttachCurrentThread(&m_JavaVirtualMachinePointer, &lJavaVMAttachArgs);
				jvm_basics::checkExceptionJavaVM(m_JavaVirtualMachinePointer);
				if (status != JNI_OK) {
					GALE_ERROR("C->java : AttachCurrentThread failed : " << status);
					m_JavaVirtualMachinePointer = nullptr;
					return false;
				}
			}
			return true;
		}
		
		void java_detach_current_thread(int _status) {
			if(_status == JNI_EDETACHED) {
				jvm_basics::getJavaVM()->DetachCurrentThread();
				m_JavaVirtualMachinePointer = nullptr;
			}
		}
		
		void sendJavaKeyboardUpdate(jboolean _showIt) {
			int status;
			if(!java_attach_current_thread(&status)) {
				GALE_ERROR("Can not attach thread ...");
				return;
			}
			//Call java ...
			m_JavaVirtualMachinePointer->CallVoidMethod(m_javaObjectGaleCallback, m_javaMethodGaleCallbackKeyboardUpdate, _showIt);
			// manage execption : 
			jvm_basics::checkExceptionJavaVM(m_JavaVirtualMachinePointer);
			java_detach_current_thread(status);
		}
		void keyboardShow() {
			GALE_VERBOSE("Call Keyboard SHOW");
			sendJavaKeyboardUpdate(JNI_TRUE);
		};
		void keyboardHide() {
			GALE_VERBOSE("Call Keyboard HIDE");
			sendJavaKeyboardUpdate(JNI_FALSE);
		};
		
		// mode 0 : auto; 1 landscape, 2 portrait
		void forceOrientation(enum gale::orientation _orientation) {
			int status;
			if(!java_attach_current_thread(&status)) {
				return;
			}
			jint param = (jint)_orientation;
			
			//Call java ...
			m_JavaVirtualMachinePointer->CallVoidMethod(m_javaObjectGaleCallback, m_javaMethodGaleCallbackOrientationUpdate, param);
			
			// manage execption : 
			jvm_basics::checkExceptionJavaVM(m_JavaVirtualMachinePointer);
			java_detach_current_thread(status);
		}
		
		void setTitle(const etk::String& _title) {
			GALE_DEBUG("C->java : send message to the java : \"" << _title << "\"");
			if (m_javaApplicationType == appl_application) {
				int status;
				if(!java_attach_current_thread(&status)) {
					return;
				}
				//Call java ...
				jstring title = m_JavaVirtualMachinePointer->NewStringUTF(_title.c_str());
				m_JavaVirtualMachinePointer->CallVoidMethod(m_javaObjectGaleCallback, m_javaMethodGaleActivitySetTitle, title);
				m_JavaVirtualMachinePointer->DeleteLocalRef(title);
				// manage execption : 
				jvm_basics::checkExceptionJavaVM(m_JavaVirtualMachinePointer);
				java_detach_current_thread(status);
			} else {
				GALE_ERROR("C->java : can not set title on appliation that is not real application");
			}
		}
		
		void openURL(const etk::String& _url) {
			GALE_DEBUG("C->java : send message to the java : open URL'" << _url << "'");
			int status;
			if(!java_attach_current_thread(&status)) {
				return;
			}
			//Call java ...
			jstring url = m_JavaVirtualMachinePointer->NewStringUTF(_url.c_str());
			m_JavaVirtualMachinePointer->CallVoidMethod(m_javaObjectGaleCallback, m_javaMethodGaleActivityOpenURI, url);
			m_JavaVirtualMachinePointer->DeleteLocalRef(url);
			// manage execption :
			jvm_basics::checkExceptionJavaVM(m_JavaVirtualMachinePointer);
			java_detach_current_thread(status);
		}
		
		void sendSystemMessage(const char* _dataString) {
			GALE_DEBUG("C->java : send message to the java : \"" << _dataString << "\"");
			int status;
			if(!java_attach_current_thread(&status)) {
				return;
			}
			GALE_DEBUG("C->java : 222");
			if (nullptr == _dataString) {
				GALE_ERROR("C->java : No data to send ...");
				return;
			}
			GALE_DEBUG("C->java : 333");
			// create the string to the java
			jstring jstr = m_JavaVirtualMachinePointer->NewStringUTF(_dataString);
			if (jstr == 0) {
				GALE_ERROR("C->java : Out of memory" );
				return;
			}
			GALE_DEBUG("C->java : 444");
			// create argument list
			jobjectArray args = m_JavaVirtualMachinePointer->NewObjectArray(1, m_javaDefaultClassString, jstr);
			if (args == 0) {
				GALE_ERROR("C->java : Out of memory" );
				return;
			}
			GALE_DEBUG("C->java : 555");
			//Call java ...
			m_JavaVirtualMachinePointer->CallVoidMethod(m_javaObjectGaleCallback, m_javaMethodGaleCallbackEventNotifier, args);
			
			GALE_DEBUG("C->java : 666");
			jvm_basics::checkExceptionJavaVM(m_JavaVirtualMachinePointer);
			java_detach_current_thread(status);
		}
	public:
		void OS_SetInput(enum gale::key::type _type,
		                 enum gale::key::status _status,
		                 int32_t _pointerID,
		                 const vec2& _pos) {
			GALE_VERBOSE("OS_SetInput [BEGIN]");
			gale::Context::OS_SetInput(_type, _status, _pointerID, vec2(_pos.x(),m_currentHeight-_pos.y()));
			GALE_VERBOSE("OS_SetInput [END]");
		}
		
		void ANDROID_SetKeyboard(char32_t _myChar, bool _isDown, bool _isARepeateKey=false) {
			GALE_VERBOSE("ANDROID_SetKeyboard [BEGIN]");
			OS_setKeyboard(m_guiKeyBoardSpecialKeyMode,
			               gale::key::keyboard::character,
			               (_isDown==true?gale::key::status::down:gale::key::status::up),
			               _isARepeateKey,
			               _myChar);
			GALE_VERBOSE("ANDROID_SetKeyboard [END]");
		}
		
		bool ANDROID_systemKeyboradEvent(enum gale::key::keyboard _key, bool _isDown) {
			GALE_VERBOSE("ANDROID_systemKeyboradEvent [BEGIN]");
			OS_setKeyboard(m_guiKeyBoardSpecialKeyMode,
			               _key,
			               (_isDown==true?gale::key::status::down:gale::key::status::up));
			GALE_VERBOSE("ANDROID_systemKeyboradEvent [END]");
			return false;
		}
		
		void ANDROID_SetKeyboardMove(int _move, bool _isDown, bool _isARepeateKey=false) {
			GALE_VERBOSE("ANDROID_SetKeyboardMove [BEGIN]");
			// direct wrapping :
			enum gale::key::keyboard move = (enum gale::key::keyboard)_move;
			m_guiKeyBoardSpecialKeyMode.update(move, _isDown);
			OS_setKeyboard(m_guiKeyBoardSpecialKeyMode,
			               move,
			               (_isDown==true?gale::key::status::down:gale::key::status::up),
			               _isARepeateKey);
			GALE_VERBOSE("ANDROID_SetKeyboardMove [END]");
		}
		
		void OS_Resize(const vec2& _size) {
			GALE_INFO("OS_Resize [BEGIN]");
			m_currentHeight = _size.y();
			gale::Context::OS_Resize(_size);
			GALE_INFO("OS_Resize [END]");
		}
		void ANDROID_start() {
			GALE_INFO("ANDROID_start [BEGIN]");
			
			GALE_INFO("ANDROID_start [END]");
		}
		void ANDROID_reStart() {
			GALE_INFO("ANDROID_reStart [BEGIN]");
			
			GALE_INFO("ANDROID_reStart [END]");
		}
		void ANDROID_create() {
			GALE_INFO("ANDROID_create [BEGIN]");
			start2ndThreadProcessing();
			GALE_INFO("ANDROID_create [END]");
		}
		void ANDROID_destroy() {
			GALE_INFO("ANDROID_destroy [BEGIN]");
			
			GALE_INFO("ANDROID_destroy [END]");
		}
};

static etk::Vector<AndroidContext*> s_listInstance;
gale::Application* s_applicationInit = NULL;

extern "C" {
	/* Call to initialize the graphics state */
	void Java_org_gale_Gale_EWparamSetArchiveDir(JNIEnv* _env,
	                                             jclass _cls,
	                                             jint _id,
	                                             jint _mode,
	                                             jstring _myString,
	                                             jstring _applicationName) {
		std::unique_lock<ethread::Mutex> lock(g_interfaceMutex);
		if(    _id >= (int32_t)s_listInstance.size()
		    || _id<0
		    || nullptr == s_listInstance[_id] ) {
			GALE_ERROR("Call C With an incorrect instance _id=" << (int32_t)_id);
			// TODO : generate error in java to stop the current instance
			return;
		}
		//GALE_CRITICAL(" call with ID : " << _id);
		// direct setting of the date in the string system ...
		jboolean isCopy;
		const char* str = _env->GetStringUTFChars(_myString, &isCopy);
		jboolean isCopy2;
		const char* str2 = _env->GetStringUTFChars(_applicationName, &isCopy2);
		s_listInstance[_id]->setArchiveDir(_mode, str, str2);
		if (isCopy == JNI_TRUE) {
			// from here str is reset ...
			_env->ReleaseStringUTFChars(_myString, str);
			str = nullptr;
		}
		if (isCopy2 == JNI_TRUE) {
			// from here str is reset ...
			_env->ReleaseStringUTFChars(_applicationName, str2);
			str2 = nullptr;
		}
	}
	
	// declare main application instance like an application:
	int main(int argc, char**argv);
	jint Java_org_gale_Gale_EWsetJavaVirtualMachineStart(JNIEnv* _env,
	                                                     jclass _classBase,
	                                                     jobject _objCallback,
	                                                     int _typeApplication) {
		std::unique_lock<ethread::Mutex> lock(g_interfaceMutex);
		GALE_DEBUG("*******************************************");
		GALE_DEBUG("** Creating GALE context                 **");
		GALE_DEBUG("*******************************************");
		AndroidContext* tmpContext = nullptr;
		s_applicationInit = NULL;
		gale::Application* localApplication = NULL;
		// call the basic init of all application (that call us ...)
		main(0,NULL);
		localApplication = s_applicationInit;
		s_applicationInit = NULL;
		if (org_gale_GaleConstants_GALE_APPL_TYPE_ACTIVITY == _typeApplication) {
			tmpContext = new AndroidContext(localApplication, _env, _classBase, _objCallback, AndroidContext::appl_application);
		} else if (org_gale_GaleConstants_GALE_APPL_TYPE_WALLPAPER == _typeApplication) {
			tmpContext = new AndroidContext(localApplication, _env, _classBase, _objCallback, AndroidContext::appl_wallpaper);
		} else {
			GALE_CRITICAL(" try to create an instance with no apply type: " << _typeApplication);
			return -1;
		}
		if (nullptr == tmpContext) {
			GALE_ERROR("Can not allocate the main context instance _id=" << (s_listInstance.size()-1));
			return -1;
		}
		// for future case : all time this ...
		s_listInstance.pushBack(tmpContext);
		int32_t newID = s_listInstance.size()-1;
		return newID;
	}
	
	void Java_org_gale_Gale_EWsetJavaVirtualMachineStop(JNIEnv* _env, jclass _cls, jint _id) {
		std::unique_lock<ethread::Mutex> lock(g_interfaceMutex);
		GALE_DEBUG("*******************************************");
		GALE_DEBUG("** remove JVM Pointer                    **");
		GALE_DEBUG("*******************************************");
		if(    _id >= (int32_t)s_listInstance.size()
		    || _id<0) {
			GALE_ERROR("Call C With an incorrect instance _id=" << (int32_t)_id);
			return;
		}
		if (nullptr == s_listInstance[_id]) {
			GALE_ERROR("the requested instance _id=" << (int32_t)_id << " is already removed ...");
			return;
		}
		s_listInstance[_id]->unInit(_env);
		delete(s_listInstance[_id]);
		s_listInstance[_id]=nullptr;
	}
	void Java_org_gale_Gale_EWtouchEvent(JNIEnv* _env, jobject _thiz, jint _id) {
		std::unique_lock<ethread::Mutex> lock(g_interfaceMutex);
		GALE_DEBUG("  == > Touch Event");
		if(    _id >= (int32_t)s_listInstance.size()
		    || _id<0
		    || nullptr == s_listInstance[_id] ) {
			GALE_ERROR("Call C With an incorrect instance _id=" << (int32_t)_id);
			// TODO : generate error in java to stop the current instance
			return;
		}
		jvm_basics::checkExceptionJavaVM(_env);
	}
	
	void Java_org_gale_Gale_EWonCreate(JNIEnv* _env, jobject _thiz, jint _id) {
		std::unique_lock<ethread::Mutex> lock(g_interfaceMutex);
		GALE_DEBUG("*******************************************");
		GALE_DEBUG("** Activity on Create                    **");
		GALE_DEBUG("*******************************************");
		if(    _id >= (int32_t)s_listInstance.size()
		    || _id<0
		    || nullptr == s_listInstance[_id] ) {
			GALE_ERROR("Call C With an incorrect instance _id=" << (int32_t)_id);
			// TODO : generate error in java to stop the current instance
			return;
		}
		s_listInstance[_id]->ANDROID_create();
		//s_listInstance[_id]->init();
	}
	
	void Java_org_gale_Gale_EWonStart(JNIEnv* _env, jobject _thiz, jint _id) {
		std::unique_lock<ethread::Mutex> lock(g_interfaceMutex);
		GALE_DEBUG("*******************************************");
		GALE_DEBUG("** Activity on Start                     **");
		GALE_DEBUG("*******************************************");
		if(    _id >= (int32_t)s_listInstance.size()
		    || _id<0
		    || nullptr == s_listInstance[_id] ) {
			GALE_ERROR("Call C With an incorrect instance _id=" << (int32_t)_id);
			// TODO : generate error in java to stop the current instance
			return;
		}
		s_listInstance[_id]->ANDROID_start();
		//SendSystemMessage(" testmessages ... ");
	}
	void Java_org_gale_Gale_EWonReStart(JNIEnv* _env, jobject _thiz, jint _id) {
		std::unique_lock<ethread::Mutex> lock(g_interfaceMutex);
		GALE_DEBUG("*******************************************");
		GALE_DEBUG("** Activity on Re-Start                  **");
		GALE_DEBUG("*******************************************");
		if(    _id >= (int32_t)s_listInstance.size()
		    || _id<0
		    || nullptr == s_listInstance[_id] ) {
			GALE_ERROR("Call C With an incorrect instance _id=" << (int32_t)_id);
			// TODO : generate error in java to stop the current instance
			return;
		}
		s_listInstance[_id]->ANDROID_reStart();
	}
	void Java_org_gale_Gale_EWonResume(JNIEnv* _env, jobject _thiz, jint _id) {
		std::unique_lock<ethread::Mutex> lock(g_interfaceMutex);
		GALE_DEBUG("*******************************************");
		GALE_DEBUG("** Activity on resume                    **");
		GALE_DEBUG("*******************************************");
		if(    _id >= (int32_t)s_listInstance.size()
		    || _id<0
		    || nullptr == s_listInstance[_id] ) {
			GALE_ERROR("Call C With an incorrect instance _id=" << (int32_t)_id);
			// TODO : generate error in java to stop the current instance
			return;
		}
		s_listInstance[_id]->OS_Resume();
	}
	void Java_org_gale_Gale_EWonPause(JNIEnv* _env, jobject _thiz, jint _id) {
		std::unique_lock<ethread::Mutex> lock(g_interfaceMutex);
		GALE_DEBUG("*******************************************");
		GALE_DEBUG("** Activity on pause                     **");
		GALE_DEBUG("*******************************************");
		if(    _id >= (int32_t)s_listInstance.size()
		    || _id<0
		    || nullptr == s_listInstance[_id] ) {
			GALE_ERROR("Call C With an incorrect instance _id=" << (int32_t)_id);
			// TODO : generate error in java to stop the current instance
			return;
		}
		// All the openGl has been destroyed ...
		s_listInstance[_id]->getResourcesManager().contextHasBeenDestroyed();
		s_listInstance[_id]->OS_Suspend();
	}
	void Java_org_gale_Gale_EWonStop(JNIEnv* _env, jobject _thiz, jint _id) {
		std::unique_lock<ethread::Mutex> lock(g_interfaceMutex);
		GALE_DEBUG("*******************************************");
		GALE_DEBUG("** Activity on Stop                      **");
		GALE_DEBUG("*******************************************");
		if(    _id >= (int32_t)s_listInstance.size()
		    || _id<0
		    || nullptr == s_listInstance[_id] ) {
			GALE_ERROR("Call C With an incorrect instance _id=" << (int32_t)_id);
			// TODO : generate error in java to stop the current instance
			return;
		}
		s_listInstance[_id]->OS_Stop();
	}
	void Java_org_gale_Gale_EWonDestroy(JNIEnv* _env, jobject _thiz, jint _id) {
		std::unique_lock<ethread::Mutex> lock(g_interfaceMutex);
		GALE_DEBUG("*******************************************");
		GALE_DEBUG("** Activity on Destroy                   **");
		GALE_DEBUG("*******************************************");
		if(    _id >= (int32_t)s_listInstance.size()
		    || _id<0
		    || nullptr == s_listInstance[_id] ) {
			GALE_ERROR("Call C With an incorrect instance _id=" << (int32_t)_id);
			// TODO : generate error in java to stop the current instance
			return;
		}
		s_listInstance[_id]->ANDROID_destroy();
		//s_listInstance[_id]->UnInit();
	}
	
	
	
	/* **********************************************************************************************
	 * ** IO section :
	 * ********************************************************************************************** */
	void Java_org_gale_Gale_EWinputEventMotion(JNIEnv* _env,
	                                           jobject _thiz,
	                                           jint _id,
	                                           jint _pointerID,
	                                           jfloat _x,
	                                           jfloat _y) {
		std::unique_lock<ethread::Mutex> lock(g_interfaceMutex);
		if(    _id >= (int32_t)s_listInstance.size()
		    || _id<0
		    || nullptr == s_listInstance[_id] ) {
			GALE_ERROR("Call C With an incorrect instance _id=" << (int32_t)_id);
			// TODO : generate error in java to stop the current instance
			return;
		}
		s_listInstance[_id]->OS_SetInput(gale::key::type::finger,
		                                 gale::key::status::move,
		                                 _pointerID+1,
		                                 vec2(_x,_y));
	}
	
	void Java_org_gale_Gale_EWinputEventState(JNIEnv* _env,
	                                          jobject _thiz,
	                                          jint _id,
	                                          jint _pointerID,
	                                          jboolean _isDown,
	                                          jfloat _x,
	                                          jfloat _y) {
		std::unique_lock<ethread::Mutex> lock(g_interfaceMutex);
		if(    _id >= (int32_t)s_listInstance.size()
		    || _id < 0
		    || s_listInstance[_id] == nullptr ) {
			GALE_ERROR("Call C With an incorrect instance _id=" << (int32_t)_id);
			// TODO : generate error in java to stop the current instance
			return;
		}
		s_listInstance[_id]->OS_SetInput(gale::key::type::finger,
		                                 (_isDown==true?gale::key::status::down:gale::key::status::up),
		                                 _pointerID+1,
		                                 vec2(_x,_y));
	}
	
	void Java_org_gale_Gale_EWmouseEventMotion(JNIEnv* _env,
	                                           jobject _thiz,
	                                           jint _id,
	                                           jint _pointerID,
	                                           jfloat _x,
	                                           jfloat _y) {
		std::unique_lock<ethread::Mutex> lock(g_interfaceMutex);
		if(    _id >= (int32_t)s_listInstance.size()
		    || _id<0
		    || nullptr == s_listInstance[_id] ) {
			GALE_ERROR("Call C With an incorrect instance _id=" << (int32_t)_id);
			// TODO : generate error in java to stop the current instance
			return;
		}
		s_listInstance[_id]->OS_SetInput(gale::key::type::mouse,
		                                 gale::key::status::move,
		                                 _pointerID+1,
		                                 vec2(_x,_y));
	}
	
	void Java_org_gale_Gale_EWmouseEventState(JNIEnv* _env,
	                                          jobject _thiz,
	                                          jint _id,
	                                          jint _pointerID,
	                                          jboolean _isDown,
	                                          jfloat _x,
	                                          jfloat _y) {
		std::unique_lock<ethread::Mutex> lock(g_interfaceMutex);
		if(    _id >= (int32_t)s_listInstance.size()
		    || _id<0
		    || nullptr == s_listInstance[_id] ) {
			GALE_ERROR("Call C With an incorrect instance _id=" << (int32_t)_id);
			// TODO : generate error in java to stop the current instance
			return;
		}
		s_listInstance[_id]->OS_SetInput(gale::key::type::mouse,
		                                 (_isDown==true?gale::key::status::down:gale::key::status::up),
		                                 _pointerID+1,
		                                 vec2(_x,_y));
	}
	
	void Java_org_gale_Gale_EWunknowEvent(JNIEnv* _env,
	                                      jobject _thiz,
	                                      jint _id,
	                                      jint _pointerID) {
		std::unique_lock<ethread::Mutex> lock(g_interfaceMutex);
		if(    _id >= (int32_t)s_listInstance.size()
		    || _id<0
		    || nullptr == s_listInstance[_id] ) {
			GALE_ERROR("Call C With an incorrect instance _id=" << (int32_t)_id);
			// TODO : generate error in java to stop the current instance
			return;
		}
		GALE_DEBUG("Unknown IO event : " << _pointerID << " ???");
	}
	
	void Java_org_gale_Gale_EWkeyboardEventMove(JNIEnv* _env,
	                                            jobject _thiz,
	                                            jint _id,
	                                            jint _type,
	                                            jboolean _isdown) {
		std::unique_lock<ethread::Mutex> lock(g_interfaceMutex);
		if(    _id >= (int32_t)s_listInstance.size()
		    || _id<0
		    || nullptr == s_listInstance[_id] ) {
			GALE_ERROR("Call C With an incorrect instance _id=" << (int32_t)_id);
			// TODO : generate error in java to stop the current instance
			return;
		}
		GALE_DEBUG("IO keyboard Move event : \"" << _type << "\" is down=" << _isdown);
		s_listInstance[_id]->ANDROID_SetKeyboardMove(_type, _isdown);
	}
	
	void Java_org_gale_Gale_EWkeyboardEventKey(JNIEnv* _env,
	                                           jobject _thiz,
	                                           jint _id,
	                                           jint _uniChar,
	                                           jboolean _isdown) {
		std::unique_lock<ethread::Mutex> lock(g_interfaceMutex);
		if(    _id >= (int32_t)s_listInstance.size()
		    || _id<0
		    || nullptr == s_listInstance[_id] ) {
			GALE_ERROR("Call C With an incorrect instance _id=" << (int32_t)_id);
			// TODO : generate error in java to stop the current instance
			return;
		}
		GALE_DEBUG("IO keyboard Key event : \"" << _uniChar << "\" is down=" << _isdown);
		s_listInstance[_id]->ANDROID_SetKeyboard(_uniChar, _isdown);
	}
	
	void Java_org_gale_Gale_EWdisplayPropertyMetrics(JNIEnv* _env,
	                                                 jobject _thiz,
	                                                 jint _id,
	                                                 jfloat _ratioX,
	                                                 jfloat _ratioY) {
		std::unique_lock<ethread::Mutex> lock(g_interfaceMutex);
		if(    _id >= (int32_t)s_listInstance.size()
		    || _id<0
		    || nullptr == s_listInstance[_id] ) {
			GALE_ERROR("Call C With an incorrect instance _id=" << (int32_t)_id);
			// TODO : generate error in java to stop the current instance
			return;
		}
		// set the internal system ratio properties ...
		gale::Dimension::setPixelRatio(vec2(_ratioX,_ratioY), gale::distance::inch);
	}
	
	// TODO : set a return true or false if we want to grep this event ...
	bool Java_org_gale_Gale_EWkeyboardEventKeySystem(JNIEnv* _env,
	                                                 jobject _thiz,
	                                                 jint _id,
	                                                 jint _keyVal,
	                                                 jboolean _isdown) {
		std::unique_lock<ethread::Mutex> lock(g_interfaceMutex);
		if(    _id >= (int32_t)s_listInstance.size()
		    || _id<0
		    || nullptr == s_listInstance[_id] ) {
			GALE_ERROR("Call C With an incorrect instance _id=" << (int32_t)_id);
			// TODO : generate error in java to stop the current instance
			return false;
		}
		switch (_keyVal) {
			case org_gale_GaleConstants_GALE_SYSTEM_KEY_VOLUME_UP:
				GALE_VERBOSE("IO keyboard Key system \"VOLUME_UP\" is down=" << _isdown);
				return s_listInstance[_id]->ANDROID_systemKeyboradEvent(gale::key::keyboard::volumeUp, _isdown);
			case org_gale_GaleConstants_GALE_SYSTEM_KEY_VOLUME_DOWN:
				GALE_DEBUG("IO keyboard Key system \"VOLUME_DOWN\" is down=" << _isdown);
				return s_listInstance[_id]->ANDROID_systemKeyboradEvent(gale::key::keyboard::volumeDown, _isdown);
			case org_gale_GaleConstants_GALE_SYSTEM_KEY_MENU:
				GALE_DEBUG("IO keyboard Key system \"MENU\" is down=" << _isdown);
				return s_listInstance[_id]->ANDROID_systemKeyboradEvent(gale::key::keyboard::menu, _isdown);
			case org_gale_GaleConstants_GALE_SYSTEM_KEY_CAMERA:
				GALE_DEBUG("IO keyboard Key system \"CAMERA\" is down=" << _isdown);
				return s_listInstance[_id]->ANDROID_systemKeyboradEvent(gale::key::keyboard::camera, _isdown);
			case org_gale_GaleConstants_GALE_SYSTEM_KEY_HOME:
				GALE_DEBUG("IO keyboard Key system \"HOME\" is down=" << _isdown);
				return s_listInstance[_id]->ANDROID_systemKeyboradEvent(gale::key::keyboard::home, _isdown);
			case org_gale_GaleConstants_GALE_SYSTEM_KEY_POWER:
				GALE_DEBUG("IO keyboard Key system \"POWER\" is down=" << _isdown);
				return s_listInstance[_id]->ANDROID_systemKeyboradEvent(gale::key::keyboard::power, _isdown);
			case org_gale_GaleConstants_GALE_SYSTEM_KEY_BACK:
				GALE_DEBUG("IO keyboard Key system \"BACK\" is down=" << _isdown);
				return s_listInstance[_id]->ANDROID_systemKeyboradEvent(gale::key::keyboard::back, _isdown);
			default:
				GALE_ERROR("IO keyboard Key system event : \"" << _keyVal << "\" is down=" << _isdown);
				break;
		}
		return false;
	}
	
	
	/* **********************************************************************************************
	 * **	Renderer section :
	 * ********************************************************************************************** */
	void Java_org_gale_Gale_EWrenderInit(JNIEnv* _env,
	                                     jobject _thiz,
	                                     jint _id) {
		std::unique_lock<ethread::Mutex> lock(g_interfaceMutex);
		GALE_VERBOSE("Java_org_gale_Gale_EWrenderInit [BEGIN]");
		if(    _id >= (int32_t)s_listInstance.size()
		    || _id<0
		    || nullptr == s_listInstance[_id] ) {
			GALE_ERROR("Call C With an incorrect instance _id=" << (int32_t)_id);
			// TODO : generate error in java to stop the current instance
			return;
		}
		GALE_VERBOSE("Java_org_gale_Gale_EWrenderInit [END]");
	}
	
	void Java_org_gale_Gale_EWrenderResize(JNIEnv* _env,
	                                       jobject _thiz,
	                                       jint _id,
	                                       jint _w,
	                                       jint _h) {
		std::unique_lock<ethread::Mutex> lock(g_interfaceMutex);
		GALE_VERBOSE("Java_org_gale_Gale_EWrenderResize [BEGIN]");
		if(    _id >= (int32_t)s_listInstance.size()
		    || _id<0
		    || nullptr == s_listInstance[_id] ) {
			GALE_ERROR("Call C With an incorrect instance _id=" << (int32_t)_id);
			// TODO : generate error in java to stop the current instance
			return;
		}
		s_listInstance[_id]->OS_Resize(vec2(_w, _h));
		GALE_VERBOSE("Java_org_gale_Gale_EWrenderResize [END]");
	}
	
	// TODO : Return true or false to not redraw when the under draw has not be done (processing gain of time)
	void Java_org_gale_Gale_EWrenderDraw(JNIEnv* _env,
	                                     jobject _thiz,
	                                     jint _id) {
		std::unique_lock<ethread::Mutex> lock(g_interfaceMutex);
		GALE_VERBOSE("Java_org_gale_Gale_EWrenderDraw [BEGIN]");
		if(    _id >= (int32_t)s_listInstance.size()
		    || _id<0
		    || nullptr == s_listInstance[_id] ) {
			GALE_ERROR("Call C With an incorrect instance _id=" << (int32_t)_id);
			// TODO : generate error in java to stop the current instance
			return;
		}
		s_listInstance[_id]->OS_Draw(true);
		GALE_VERBOSE("Java_org_gale_Gale_EWrenderDraw [END]");
	}
};


int gale::run(gale::Application* _application, int _argc, const char *_argv[]) {
	s_applicationInit = _application;
	return 0;
}
