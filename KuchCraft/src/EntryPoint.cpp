#include "kcpch.h"

#include "Core/Application.h"

#if defined(KC_PLATFORM_WINDOWS)
	#include <Windows.h>
#endif

int main(int argc, char** argv)
{
#if defined(KC_PLATFORM_WINDOWS) && !defined(KC_HAS_CONSOLE)
	::ShowWindow(::GetConsoleWindow(), SW_HIDE);
#endif

	KuchCraft::InitializeCore();
	auto app = new KuchCraft::Application(argc, argv);
	app->Run();
	delete app;
	KuchCraft::ShutdownCore();

	return 0;
}