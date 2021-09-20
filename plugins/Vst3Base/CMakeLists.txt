ADD_SUBDIRECTORY(vst3sdk)

INCLUDE(BuildPlugin)

BUILD_PLUGIN(vst3base
	impl/AttributeList.cpp    impl/AttributeList.h
	impl/BStream.cpp          impl/BStream.h
	impl/EventList.cpp        impl/EventList.h
	impl/HostApplication.cpp  impl/HostApplication.h
	impl/Message.cpp          impl/Message.h
	impl/ParamValueQueue.cpp  impl/ParamValueQueue.h
	impl/ParameterChanges.cpp impl/ParameterChanges.h
	Vst3Base.cpp
	Vst3Core.cpp              Vst3Core.h
	Vst3Manager.cpp           Vst3Manager.h
	Vst3Plugin.cpp            Vst3Plugin.h
	Vst3PluginControls.cpp    Vst3PluginControls.h
	Vst3PluginControlView.cpp Vst3PluginControlView.h
	Vst3PluginView.cpp        Vst3PluginView.h
	Vst3SubPluginFeatures.cpp Vst3SubPluginFeatures.h
	MOCFILES Vst3PluginView.h
	EXPORT_BASE_NAME vst3base
	LINK SHARED
)

SET_TARGET_PROPERTIES(vst3base PROPERTIES CXX_STANDARD 17)
TARGET_INCLUDE_DIRECTORIES(vst3base PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
TARGET_LINK_LIBRARIES(vst3base pluginterfaces)

IF(LMMS_BUILD_LINUX)
	TARGET_LINK_LIBRARIES(vst3base qx11embedcontainer ${CMAKE_DL_LIBS})
ENDIF(LMMS_BUILD_LINUX)
