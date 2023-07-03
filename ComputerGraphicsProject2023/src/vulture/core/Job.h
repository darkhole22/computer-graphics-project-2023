#pragma once

#include "vulture/core/Core.h"

#include <functional>
#include <thread>

namespace vulture {

/**
* @brief This class allows to submit jobs to the job system.
*
* A job has two components.
* The first is executed on a different thread asynchronously and returns a boolean.
* The second is executed on the main thread after the work is completed and should be
* used to cleanup all the resources used by the job.
*/
class Job
{
public:
	/**
	* @brief Enqueues a new job.
	*
	* @param jobCallback: the work to do asynchronously.
	* @param data: the data produced by the job.
	* @param cleanupCallback: the callback used to finish the job. This is executed on the main thread.
	*/
	static void submit(std::function<bool(void*)> jobCallback, void* data, std::function<void(bool, void*)> cleanupCallback);

	friend class Application;
	friend class Worker;
private:
	Job();
	Job(std::function<bool(void*)> jobCallback, void* data, std::function<void(bool, void*)> cleanupCallback);

	std::function<bool(void*)> m_WorkCallback;
	std::function<void(bool, void*)> m_CleanupCallback;
	void* m_Data;
	bool m_Result = false;

	void execute();
	void cleanupExecute();

	static bool init();
	static void cleanup();
	static void process();
};

} // namespace vulture
