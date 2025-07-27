#pragma once

namespace KuchCraft {

	constexpr inline int min_window_width  = 640;
	constexpr inline int min_window_height = 480;
	constexpr inline int max_window_width  = 7680;
	constexpr inline int max_window_height = 4320;

	constexpr inline float max_delta_time = 1.0f / 60.0f;
	constexpr inline float tick_interval  = 1.0f / 20.0f;

	struct ApplicationConfig
	{
		int    argc = 0;
		char** argv = nullptr;

		bool EnableImGui = true;

		LogLevel CoreLogLevel = LogLevel::Info;
		LogLevel GameLogLevel = LogLevel::Info;
	};

	struct WindowConfig
	{
		std::string Title = "KuchCraft";

		std::filesystem::path IconPath;

		bool FullScreen = false;
		bool Borderless = false;

		int  Width  = 1280;
		int  Height = 720;
		
		int PositionX = 0;
		int PositionY = 0;

		bool Resizable = true;
		bool VSync     = true;

		WindowCursorMode CursorMode = WindowCursorMode::Normal;

		float ContentScaleX = 0.0f;
		float ContentScaleY = 0.0f;
	};

	struct RendererConfig
	{
		std::string ShadersPath = "assets/shaders/";

		int OpenGlMajorVersion = 4;
		int OpenGlMinorVersion = 6;

		bool OpenGlLogging = true;

		int MaxCombinedTextureSlots = 64;
		int MaxTextureSlots         = 32;

		uint32_t MaxQuadsInBatch  = 10'000;
		uint32_t MaxPlanesInBatch = 10'000;

		std::string GetOpenGlVersion() const { return std::to_string(OpenGlMajorVersion * 100 + OpenGlMinorVersion * 10) + " core"; }
	};

	struct GameConfig
	{
		std::string WorldsDir    = "data/worlds/";
		std::string DataPacksDir = "data/packs/";
	};

	struct Config
	{
		ApplicationConfig Application;
		WindowConfig      Window;
		RendererConfig    Renderer;
		GameConfig        Game;

		void Serialize(const std::filesystem::path& path) const;
		void Deserialize(const std::filesystem::path& path);
	};
}