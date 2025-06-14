#include "kcpch.h"

#include "Core/Application.h"

#if defined(KC_PLATFORM_WINDOWS)
	#include <Windows.h>
#endif

#if defined(KC_PLATFORM_WINDOWS) && defined(KC_DIST)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	int    argc = __argc;
	char** argv = __argv;
#else
int main(int argc, char** argv)
{
#endif

	KuchCraft::InitializeCore();
	auto app = new KuchCraft::Application(argc, argv);
	app->Run();
	delete app;
	KuchCraft::ShutdownCore();

	return 0;
}