#pragma once
#include "vulture/core/Core.h"

namespace vulture {

class AudioPlayer
{
public:
	AudioPlayer(const String& name);

	void play(f32 startTime = 0.0f);

	void setPause(bool pause = true);

	void stop();

	~AudioPlayer() = default;
private:
	Ref<void> m_Data;
};

} // namespace vulture
