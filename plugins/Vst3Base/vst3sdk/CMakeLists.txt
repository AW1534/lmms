ADD_LIBRARY(pluginterfaces STATIC
    pluginterfaces/base/conststringtable.cpp
    pluginterfaces/base/coreiids.cpp
    pluginterfaces/base/funknown.cpp
    pluginterfaces/base/ustring.cpp
)
TARGET_INCLUDE_DIRECTORIES(pluginterfaces PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
