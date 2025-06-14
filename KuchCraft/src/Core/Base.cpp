#include "kcpch.h"
#include "Base.h"

#include "Core/Log.h"

void KuchCraft::InitializeCore()
{
	Log::Init();
	KC_CORE_TRACE("Core Initializing...");
}

void KuchCraft::ShutdownCore()
{
	KC_CORE_TRACE("Shutting down...");
	Log::Shutdown();
}
