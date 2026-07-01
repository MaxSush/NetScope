#pragma once

#include <string>
#include <thread>
#include <atomic>
#include <functional>

namespace netscope
{
	class Thread
	{
	public:
		Thread(std::string name);
		~Thread();

		// function with declaration: func(Thread& t) 
		void Start(std::function<void(Thread&)> function);
		void RequestStop();
		bool ShouldStop() const;
		void Join();
		bool IsRunning() const;
		const std::string& GetName() const;

	private:
		std::string m_name;
		std::thread m_thread;
		std::atomic<bool> m_isRunning;
		std::atomic<bool> m_stopRequested;
	};
}