#include "kcpch.h"

#include "Core/Log.h"

#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/basic_file_sink.h"

namespace KuchCraft {

    void Log::Init()
    {
        std::string logsDirectory = "logs";
        if (!std::filesystem::exists(logsDirectory))
            std::filesystem::create_directories(logsDirectory);

        std::vector<spdlog::sink_ptr> coreSinks =
        {
            std::make_shared<spdlog::sinks::basic_file_sink_mt>(logsDirectory + "/app.log", true),
#if KC_HAS_CONSOLE
            std::make_shared<spdlog::sinks::stdout_color_sink_mt>()
#endif
        };

        std::vector<spdlog::sink_ptr> gameSinks =
        {
			std::make_shared<spdlog::sinks::basic_file_sink_mt>(logsDirectory + "/kuchcraft.log", true),
#if KC_HAS_CONSOLE
			std::make_shared<spdlog::sinks::stdout_color_sink_mt>()
#endif
        };

        coreSinks[0]->set_pattern("[%T] [%l] %n: %v");
		gameSinks[0]->set_pattern("[%T] [%l] %n: %v");

#if KC_HAS_CONSOLE
        coreSinks[1]->set_pattern("%^[%T] %n: %v%$");
        gameSinks[1]->set_pattern("%^[%T] %n: %v%$");
#endif

		s_CoreLogger = std::make_shared<spdlog::logger>("Core", coreSinks.begin(), coreSinks.end());
		s_CoreLogger->set_level(spdlog::level::trace);

		s_GameLogger = std::make_shared<spdlog::logger>("Game", gameSinks.begin(), gameSinks.end());
		s_GameLogger->set_level(spdlog::level::trace);

    }

    void Log::Shutdown()
    {
		s_CoreLogger.reset();
		s_GameLogger.reset();
        spdlog::drop_all();
    }

}