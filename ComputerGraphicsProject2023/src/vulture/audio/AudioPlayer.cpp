#include "AudioPlayer.h"
#include "AudioBackend.h"
#include "soloud_wav.h"
#undef ERROR
#include "vulture/core/Logger.h"

#include <unordered_map>
#include <array>
#include <filesystem>

namespace vulture {

static const std::array<String, 8> supportedExtensions = {
	".wav",
	".mp3"
};

struct AudioPlayerData
{
	SoLoud::handle handle = 0;
	SoLoud::Wav wav;
};

#define DATA reinterpret_cast<AudioPlayerData*>(m_Data.get())

AudioPlayer::AudioPlayer(const String& name) :
	m_Data(makeRef<AudioPlayerData>())
{
	String namePrefix = "res/sounds/" + name;

	String format;
	for (auto& ext : supportedExtensions)
	{
		if (std::filesystem::exists((namePrefix + ext).cString()))
		{
			format = ext;
			break;
		}
	}
	if (format.isEmpty())
	{
		VUERROR("Failed to load audio named %s!", name.cString());
		return;
	}

	String path = namePrefix + format;
	SoLoud::result res = DATA->wav.load(path.cString());
	if (res != SoLoud::SO_NO_ERROR)
	{
		VUERROR("Failed to load audio at %s!", path.cString());
	}
}

void AudioPlayer::play(f32 startTime)
{
	stop();
	DATA->handle = audioEngineData.engine.play(DATA->wav);
	audioEngineData.engine.seek(DATA->handle, startTime);
}

void AudioPlayer::setPause(bool pause)
{
	if (DATA->handle != 0)
		audioEngineData.engine.setPause(DATA->handle, pause);
}

void AudioPlayer::stop()
{
	if (DATA->handle != 0)
		audioEngineData.engine.stop(DATA->handle);
}

} // namespace vulture
