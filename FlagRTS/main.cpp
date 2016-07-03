#include "AppManager.h"
#include <ObjectPoolAllocator.h>
#include <StdAllocator.h>
#include <Memory.h>
#include <Profiler.h>
#include <TypeId.h>

#define WIN32_LEAN_AND_MEAN
#include "windows.h"

INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT)
{
	PROFILE_REGISTER_CLOCK(mainClock, "Main");
	PROFILE_START(mainClock);
	FlagRTS::Malloc = new FlagRTS::StdAllocator();
	FlagRTS::TypeHelper::GlobalTypeHelper = new FlagRTS::TypeHelper();

	FlagRTS::AppManager::Initialize();
	auto app = FlagRTS::AppManager::Instance();

	app->Run();

	FlagRTS::AppManager::Release();

	delete FlagRTS::TypeHelper::GlobalTypeHelper;
	delete FlagRTS::Malloc;

	PROFILE_END(mainClock);
	PROFILE_LOG_TO_FILE("profiler.log");
	PROFILE_RELEASE();


	abort();
	return 0;
}