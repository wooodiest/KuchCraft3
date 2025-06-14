#pragma once

#include <filesystem>

namespace KuchCraft {

	struct ApplicationConfig
	{
		int    argc = 0;
		char** argv = nullptr;

		bool EnableImGui = true;
	};

	struct WindowConfig
	{
		const char* Title = "KuchCraft";

		std::filesystem::path IconPath;

		bool StartMaximized = true;
		bool FullScreen     = false;

		int  Width  = 1280;
		int  Height = 720;
		
		int PositionX = 0;
		int PositionY = 0;

		bool Resizable = true;
		bool VSync     = true;

		bool ShowCursor = true;
	};

	struct RendererConfig
	{
		std::string ShaderVersion = "460 core";
	};

	struct Config
	{
		ApplicationConfig Application;
		WindowConfig      Window;
		RendererConfig    Renderer;

		void Serialize(const std::filesystem::path& path) const;
		void Deserialize(const std::filesystem::path& path);
	};
}