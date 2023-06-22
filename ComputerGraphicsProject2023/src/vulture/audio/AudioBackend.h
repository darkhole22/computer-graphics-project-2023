#pragma once

#include "soloud.h"

namespace vulture {

struct AudioEngineData
{
	SoLoud::Soloud engine;
};

#ifdef AUDIO_ENGINE_DATA_DEFINITION
AudioEngineData audioEngineData = {};
#else
extern AudioEngineData audioEngineData;
#endif

} // namespace vulture
