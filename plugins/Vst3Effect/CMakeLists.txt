INCLUDE(BuildPlugin)

BUILD_PLUGIN(vst3effect
	Vst3Effect.cpp Vst3Effect.h
	Vst3EffectControls.cpp Vst3EffectControls.h
	Vst3EffectControlDialog.cpp Vst3EffectControlDialog.h
	MOCFILES Vst3Effect.h Vst3EffectControls.h Vst3EffectControlDialog.h
	EMBEDDED_RESOURCES logo.png
)

SET_TARGET_PROPERTIES(vst3effect PROPERTIES CXX_STANDARD 17)
TARGET_LINK_LIBRARIES(vst3effect vst3base)
