#pragma once
#include "vulture/core/Core.h"

namespace vulture {

/**
 * @brief A class to handle a single sound loaded from file.
 */
class AudioPlayer
{
public:
	/**
	 * @brief Construct an AudioPlayer with a provided audio file.
	 *
	 * @param name The name of the file to load (with no extension).
	 */
	explicit AudioPlayer(const String& name);

	/**
	 * @brief Plays the audio. If the audio is already playing it is stopped before restarting.
	 * @param startTime the starting time of the audio in seconds
	 */
	void play(f32 startTime = 0.0f);

	/**
	 * @brief Pauses or un-pauses the audio.
	 * @param pause true to pause, false to un-pause.
	 */
	void setPause(bool pause = true);

	/**
	 * @brief Stops the audio.
	 */
	void stop();

	~AudioPlayer() = default;
private:
	Ref<void> m_Data;
};

} // namespace vulture
