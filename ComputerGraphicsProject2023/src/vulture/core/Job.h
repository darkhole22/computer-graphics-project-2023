#pragma once

#include "vulture/core/Core.h"

#include <functional>

namespace vulture {

class Job
{
public:
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
