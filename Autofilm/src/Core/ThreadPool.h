#pragma once

#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>

namespace Autofilm
{
    class Thread
    {
    public:
    		Thread()
		{
			_worker = std::thread(&Thread::queueLoop, this);
		}

		~Thread()
		{
			if (_worker.joinable())
			{
				wait();
				_queueMutex.lock();
				_destroying = true;
				_condition.notify_one();
				_queueMutex.unlock();
				_worker.join();
			}
		}

		// Add a new job to the thread's queue
		void addJob(std::function<void()> function)
		{
			std::lock_guard<std::mutex> lock(_queueMutex);
			_jobQueue.push(std::move(function));
			_condition.notify_one();
		}

		// Wait until all work items have been finished
		void wait()
		{
			std::unique_lock<std::mutex> lock(_queueMutex);
			_condition.wait(lock, [this]() { return _jobQueue.empty(); });
		}

    private:
        bool _destroying = false;
        std::thread _worker;
		std::queue<std::function<void()>> _jobQueue;
		std::mutex _queueMutex;
		std::condition_variable _condition;

        void queueLoop()
        {
            while (true) {
                std::function<void()> job;
                {
                    std::unique_lock<std::mutex> lock(_queueMutex);
                    _condition.wait(lock, [this] { return !_jobQueue.empty() || _destroying; });
                    if (_destroying) {
                        break;
                    }
                    job = _jobQueue.front();
                }

                job();

                {
                    std::lock_guard<std::mutex> lock(_queueMutex);
                    _jobQueue.pop();
                    _condition.notify_one();
                }
            }
        }
    };

    class ThreadPool
    {
    public:
        std::vector<std::unique_ptr<Thread>> _threads;

        // Sets the number of threads to be allocated in this pool
        void setThreadCount(uint32_t count)
        {
            _threads.clear();
            for (uint32_t i = 0; i < count; i++) {
                pushThread();
            }
        }

        void pushThread()
        {
            _threads.push_back(std::make_unique<Thread>());
        }

        // Wait until all threads have finished their work items
        void wait()
        {
            for (auto& thread : _threads)
            {
                thread->wait();
            }
        }
    };
}