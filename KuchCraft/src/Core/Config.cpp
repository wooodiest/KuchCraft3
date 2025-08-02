#include "kcpch.h"
#include "Core/Config.h"

namespace KuchCraft {

	void Config::Serialize(const std::filesystem::path& path) const
	{
		nlohmann::json configJson;

		configJson["Application"] = {
			{ "EnableImGui",    Application.EnableImGui },
			{ "CoreLogLevel",   ToString(Application.CoreLogLevel) },
			{ "GameLogLevel",   ToString(Application.GameLogLevel) }
		};

		configJson["Window"] = {
			{ "Title",         Window.Title },
			{ "IconPath",      Window.IconPath.string() },
			{ "FullScreen",    Window.FullScreen },
			{ "Borderless",    Window.Borderless },
			{ "Width",         Window.Width  },
			{ "Height",        Window.Height },
			{ "PositionX",     Window.PositionX },
			{ "PositionY",     Window.PositionY },
			{ "Resizable",     Window.Resizable },
			{ "VSync",         Window.VSync },
			{ "CursorMode",    ToString(Window.CursorMode) },
			{ "ContentScaleX", Window.ContentScaleX },
			{ "ContentScaleY", Window.ContentScaleY }
		};

		configJson["Renderer"] = {
			{ "ShadersPath",              Renderer.ShadersPath },
			{ "OpenGlMajorVersion",       Renderer.OpenGlMajorVersion },
			{ "OpenGlMinorVersion",       Renderer.OpenGlMinorVersion },
			{ "OpenGlLogging",            Renderer.OpenGlLogging },
			{ "MaxCombinedTextureSlots",  Renderer.MaxCombinedTextureSlots },
			{ "MaxTextureSlots",          Renderer.MaxTextureSlots },
			{ "MaxQuadsInBatch",          Renderer.MaxQuadsInBatch },
			{ "MaxPlanesInBatch",         Renderer.MaxPlanesInBatch },
			{ "RenderDistance",           Renderer.RenderDistance },
		};

		configJson["Game"] = {
			{ "WorldsDir",    Game.WorldsDir },
			{ "DataPacksDir", Game.DataPacksDir }
		};

		std::ofstream file(path);
		if (!file.is_open())
		{
			KC_CORE_ERROR("Failed to open : {}", path.string());
			return;
		}

		file << configJson.dump(4, ' ');
		file.close();
	}

	void Config::Deserialize(const std::filesystem::path& path)
	{
		if (!std::filesystem::exists(path))
		{
			KC_CORE_WARN("File does not exist: {}", path.string());
			return;
		}

		std::ifstream file(path);
		if (!file.is_open())
		{
			KC_CORE_ERROR("Failed to open file: {}", path.string());
			return;
		}

		nlohmann::json configJson;
		try
		{
			file >> configJson;
			file.close();
		}
		catch (std::exception& e)
		{
			KC_CORE_ERROR("Failed to parse JSON: {}", e.what());
			return;
		}

		if (configJson.contains("Application"))
		{
			nlohmann::json applicationJson = configJson["Application"];

			if (applicationJson.contains("EnableImGui"))
				Application.EnableImGui = applicationJson["EnableImGui"];
			if (applicationJson.contains("CoreLogLevel"))
			{
				const auto& coreLogLevel = FromString<LogLevel>(applicationJson["CoreLogLevel"].get<std::string>());
				if (coreLogLevel.has_value())
					Application.CoreLogLevel = *coreLogLevel;
			}
			if (applicationJson.contains("GameLogLevel"))
			{
				const auto& gameLogLevel = FromString<LogLevel>(applicationJson["GameLogLevel"].get<std::string>());
				if (gameLogLevel.has_value())
					Application.GameLogLevel = *gameLogLevel;
			}		
		}

		if (configJson.contains("Window"))
		{
			auto& windowJson = configJson["Window"];

			if (windowJson.contains("Title"))
				Window.Title = windowJson["Title"];
			if (windowJson.contains("IconPath"))
				Window.IconPath = windowJson["IconPath"].get<std::string>();
			if (windowJson.contains("FullScreen"))
				Window.FullScreen = windowJson["FullScreen"];
			if (windowJson.contains("Borderless"))
				Window.Borderless = windowJson["Borderless"];
			if (windowJson.contains("Width"))
				Window.Width = windowJson["Width"];
			if (windowJson.contains("Height"))
				Window.Height = windowJson["Height"];
			if (windowJson.contains("PositionX"))
				Window.PositionX = windowJson["PositionX"];
			if (windowJson.contains("PositionY"))
				Window.PositionY = windowJson["PositionY"];
			if (windowJson.contains("Resizable"))
				Window.Resizable = windowJson["Resizable"];
			if (windowJson.contains("VSync"))
				Window.VSync = windowJson["VSync"];
			if (windowJson.contains("CursorMode"))
			{
				auto val = FromString<WindowCursorMode>(windowJson["CursorMode"].get<std::string>());
				if (val.has_value())
					Window.CursorMode = *val;
			}
			if (windowJson.contains("ContentScaleX"))
				Window.ContentScaleX = windowJson["ContentScaleX"];
			if (windowJson.contains("ContentScaleY"))
				Window.ContentScaleY = windowJson["ContentScaleY"];
		}

		if (configJson.contains("Renderer"))
		{
			auto& rendererJson = configJson["Renderer"];

			if (rendererJson.contains("ShadersPath"))
				Renderer.ShadersPath = rendererJson["ShadersPath"];
			if (rendererJson.contains("OpenGlMajorVersion"))
				Renderer.OpenGlMajorVersion = rendererJson["OpenGlMajorVersion"];
			if (rendererJson.contains("OpenGlMinorVersion"))
				Renderer.OpenGlMinorVersion = rendererJson["OpenGlMinorVersion"];
			if (rendererJson.contains("OpenGlLogging"))
				Renderer.OpenGlLogging = rendererJson["OpenGlLogging"];
			if (rendererJson.contains("MaxCombinedTextureSlots"))
				Renderer.MaxCombinedTextureSlots = rendererJson["MaxCombinedTextureSlots"];
			if (rendererJson.contains("MaxTextureSlots"))
				Renderer.MaxTextureSlots = rendererJson["MaxTextureSlots"];
			if (rendererJson.contains("MaxQuadsInBatch"))
				Renderer.MaxQuadsInBatch = rendererJson["MaxQuadsInBatch"];
			if (rendererJson.contains("MaxPlanesInBatch"))
				Renderer.MaxPlanesInBatch = rendererJson["MaxPlanesInBatch"];
			if (rendererJson.contains("RenderDistance"))
				Renderer.RenderDistance = rendererJson["RenderDistance"];
		}

		if (configJson.contains("Game"))
		{
			auto& gameJson = configJson["Game"];

			if (gameJson.contains("WorldsDir"))
				Game.WorldsDir = gameJson["WorldsDir"];
			if (gameJson.contains("DataPacksDir"))
				Game.DataPacksDir = gameJson["DataPacksDir"];
		}

	}

}
