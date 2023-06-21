#pragma once

namespace vulture {

/**
 * @brief A fully static class to handle the audio
 */
class AudioEngine
{
public:
	static bool init();

	static void cleanup();
};

} // namespace vulture
