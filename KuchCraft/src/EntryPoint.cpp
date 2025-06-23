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

	bool restart = false;
	do {
		restart = false;

		auto app = new KuchCraft::Application(argc, argv);
		app->Run();

		if (app->ShouldRestart())
		{
			restart = true;
			argc = app->GetConfig().Application.argc;
			argv = app->GetConfig().Application.argv;
			KC_CORE_INFO("Restarting application...");
		}
		
		delete app;
	} while (restart);

	KuchCraft::ShutdownCore();

	return 0;
}