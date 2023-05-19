#include "Job.h"

#include "vulture/core/Logger.h"

#include <algorithm> // std::max
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <utility>

namespace vulture {

class Worker
{
public:
	Worker();

	void stop();

	int operator()(const std::stop_token&);
private:
	std::jthread m_Thread;
};

static std::vector<Worker> workers;

static std::mutex jobsQueueMutex;
static std::condition_variable jobsQueueConditionVariable;
static std::queue<Job> jobsQueue;

static std::mutex jobsCompletedMutex;
static std::vector<Job> jobsCompleted;

void Job::submit(std::function<bool(void*)> jobCallback, void* data, std::function<void(bool, void*)> cleanupCallback)
{
	{
		std::scoped_lock lock{ jobsQueueMutex };
		jobsQueue.push(Job(std::move(jobCallback), data, std::move(cleanupCallback)));
	}

	jobsQueueConditionVariable.notify_one();
}

bool Job::init()
{
	u32 workersCount = std::max(1, static_cast<i32>(std::jthread::hardware_concurrency()) - 1);
	workers.resize(workersCount);

	return true;
}

void Job::cleanup()
{
	for (auto& worker : workers)
	{
		worker.stop();
	}
	jobsQueueConditionVariable.notify_all();

	workers.clear();
}

void Job::process()
{
	std::scoped_lock lock{ jobsCompletedMutex };

	for (auto& job : jobsCompleted)
	{
		job.cleanupExecute();
	}

	jobsCompleted.clear();
}

Job::Job() :
	Job(nullptr, nullptr, nullptr)
{}

Job::Job(std::function<bool(void*)> workCallback, void* data, std::function<void(bool, void*)> cleanupCallback) :
	m_WorkCallback(std::move(workCallback)), m_CleanupCallback(std::move(cleanupCallback)), m_Data(data)
{}

void Job::execute()
{
	if (m_WorkCallback)
	{
		try
		{
			m_Result = m_WorkCallback(m_Data);
		}
		catch (const std::exception&)
		{
			m_Result = false;
		}
	}
	else
		m_Result = true;
}

void Job::cleanupExecute()
{
	if (m_CleanupCallback)
	{
		try
		{
			m_CleanupCallback(m_Result, m_Data);
		}
		catch (const std::exception& e)
		{
			VUERROR("Un exception was thrown during job processing.\nMessage: %s.", e.what());
		}
	}
}

Worker::Worker() :
	m_Thread(std::bind_front(&Worker::operator(), this))
{}

void Worker::stop()
{
	m_Thread.request_stop();
}

int Worker::operator()(const std::stop_token& stopToken)
{
	Job job;
	while (!stopToken.stop_requested())
	{
		{
			std::unique_lock lock{ jobsQueueMutex };

			jobsQueueConditionVariable.wait(lock, [stopToken] {
				return !jobsQueue.empty() || stopToken.stop_requested();
			});

			if (stopToken.stop_requested()) break;

			job = jobsQueue.front();
			jobsQueue.pop();
		}

		job.execute();

		{
			std::scoped_lock lock{ jobsCompletedMutex };
			jobsCompleted.push_back(job);
		}
	}
	return 0;
}

} // namespace vulture
