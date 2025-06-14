#pragma once

#include <spdlog/spdlog.h>

namespace KuchCraft {

	enum class LogType : uint8_t
	{
		Core = 0, Game = 1
	};

	enum class LogLevel : uint8_t
	{
		Trace = 0, Info, Warn, Error, Fatal
	};

	class Log
	{
	public:
		static void Init();
		static void Shutdown();

		template<typename... Args>
		static void PrintMessage(LogType type, LogLevel level, fmt::format_string<Args...> format, Args&&... args)
		{
			auto logger = (type == LogType::Core) ? s_CoreLogger : s_GameLogger;
			switch (level)
			{
			case LogLevel::Trace:
				logger->trace(format, std::forward<Args>(args)...);
				break;
			case LogLevel::Info:
				logger->info(format, std::forward<Args>(args)...);
				break;
			case LogLevel::Warn:
				logger->warn(format, std::forward<Args>(args)...);
				break;
			case LogLevel::Error:
				logger->error(format, std::forward<Args>(args)...);
				break;
			case LogLevel::Fatal:
				logger->critical(format, std::forward<Args>(args)...);
				break;
			}
		}

		static void SetLogLevel(LogType type, LogLevel level)
		{
			auto logger = (type == LogType::Core) ? s_CoreLogger : s_GameLogger;
			switch (level)
			{
			case LogLevel::Trace:
				logger->set_level(spdlog::level::trace);
				break;
			case LogLevel::Info:
				logger->set_level(spdlog::level::info);
				break;
			case LogLevel::Warn:
				logger->set_level(spdlog::level::warn);
				break;
			case LogLevel::Error:
				logger->set_level(spdlog::level::err);
				break;
			case LogLevel::Fatal:
				logger->set_level(spdlog::level::critical);
				break;
			}
		}

	private:
		static inline std::shared_ptr<spdlog::logger> s_CoreLogger;
		static inline std::shared_ptr<spdlog::logger> s_GameLogger;

	private:
		Log() = delete;
		~Log() = delete;
		Log(const Log&) = delete;
		Log& operator=(const Log&) = delete;
		Log(Log&&) = delete;
		Log& operator=(Log&&) = delete;
	};
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Core Logging
#define KZ_CORE_TRACE(...)  ::KuchCraft::Log::PrintMessage(::KuchCraft::LogType::Core, ::KuchCraft::LogLevel::Trace, __VA_ARGS__)
#define KZ_CORE_INFO(...)   ::KuchCraft::Log::PrintMessage(::KuchCraft::LogType::Core, ::KuchCraft::LogLevel::Info, __VA_ARGS__)
#define KZ_CORE_WARN(...)   ::KuchCraft::Log::PrintMessage(::KuchCraft::LogType::Core, ::KuchCraft::LogLevel::Warn, __VA_ARGS__)
#define KZ_CORE_ERROR(...)  ::KuchCraft::Log::PrintMessage(::KuchCraft::LogType::Core, ::KuchCraft::LogLevel::Error, __VA_ARGS__)
#define KZ_CORE_FATAL(...)  ::KuchCraft::Log::PrintMessage(::KuchCraft::LogType::Core, ::KuchCraft::LogLevel::Fatal, __VA_ARGS__)

// Game Logging
#define KC_TRACE(...)   ::KuchCraft::Log::PrintMessage(::KuchCraft::LogType::Game, ::KuchCraft::LogLevel::Trace, __VA_ARGS__)
#define KC_INFO(...)    ::KuchCraft::Log::PrintMessage(::KuchCraft::LogType::Game, ::KuchCraft::LogLevel::Info, __VA_ARGS__)
#define KC_WARN(...)    ::KuchCraft::Log::PrintMessage(::KuchCraft::LogType::Game, ::KuchCraft::LogLevel::Warn, __VA_ARGS__)
#define KC_ERROR(...)   ::KuchCraft::Log::PrintMessage(::KuchCraft::LogType::Game, ::KuchCraft::LogLevel::Error, __VA_ARGS__)
#define KC_FATAL(...)   ::KuchCraft::Log::PrintMessage(::KuchCraft::LogType::Game, ::KuchCraft::LogLevel::Fatal, __VA_ARGS__)
