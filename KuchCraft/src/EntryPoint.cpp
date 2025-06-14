#include "kcpch.h"

#include "Core/Base.h"

#if defined(KC_PLATFORM_WINDOWS)
	#include <Windows.h>
#endif

int main(int argc, char** argv)
{
#if defined(KC_PLATFORM_WINDOWS) && !defined(KC_HAS_CONSOLE)
	::ShowWindow(::GetConsoleWindow(), SW_HIDE);
#endif

	KuchCraft::InitializeCore();
	std::cout << "Hello, KuchCraft!" << std::endl;
	KuchCraft::ShutdownCore();
	return 0;
}