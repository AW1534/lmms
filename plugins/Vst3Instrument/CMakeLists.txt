INCLUDE(BuildPlugin)

BUILD_PLUGIN(vst3instrument
    Vst3Instrument.cpp Vst3Instrument.h
    MOCFILES Vst3Instrument.h
    EMBEDDED_RESOURCES logo.png
)

SET_TARGET_PROPERTIES(vst3instrument PROPERTIES CXX_STANDARD 17)
TARGET_LINK_LIBRARIES(vst3instrument vst3base)
