if(NOT LMMS_HAVE_VST)
	return()
endif()

include(BuildPlugin)
link_directories("${CMAKE_CURRENT_BINARY_DIR}/..")
if(LMMS_BUILD_LINUX)
	link_libraries(-Wl,--enable-new-dtags)
	set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
	set(CMAKE_INSTALL_RPATH "$ORIGIN")
else()
	set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${PLUGIN_DIR}")
endif()
build_plugin(vestige Vestige.cpp Vestige.h MOCFILES Vestige.h EMBEDDED_RESOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.png;*.svg")
target_link_libraries(vestige vstbase)
