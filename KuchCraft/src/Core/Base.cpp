#include "kcpch.h"
#include "Core/Base.h"

namespace KuchCraft {


	void InitializeCore()
	{
		Log::Init();

		uint32_t threadsCount = std::thread::hardware_concurrency();
		std::vector<std::thread> threads;
		threads.reserve(threadsCount);

		for (uint32_t i = 0; i < threadsCount; i++)
			threads.emplace_back([]() { Random::Init(); });

		for (auto& t : threads)
			t.join();
	}

	void ShutdownCore()
	{
		Log::Shutdown();
	}

}