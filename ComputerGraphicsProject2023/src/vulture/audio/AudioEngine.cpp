#include "AudioEngine.h"
#define AUDIO_ENGINE_DATA_DEFINITION
#include "AudioBackend.h"

namespace vulture {

bool AudioEngine::init()
{
	SoLoud::result res = audioEngineData.engine.init();
	return res == SoLoud::SO_NO_ERROR;
}

void AudioEngine::cleanup()
{
	audioEngineData.engine.deinit();
}

} // namespace vulture
