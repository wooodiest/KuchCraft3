#include "kcpch.h"
#include "Base.h"

#include "Core/Log.h"

void KuchCraft::InitializeCore()
{
	Log::Init();
	KZ_CORE_TRACE("Core Initializing...");
}

void KuchCraft::ShutdownCore()
{
	KZ_CORE_TRACE("Shutting down...");
	Log::Shutdown();
}
