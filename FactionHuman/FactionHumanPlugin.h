#pragma once

#include <DllGameMod.h>
#include <Array.h>

namespace FlagRTS
{
	class IKindSpecificDataSupplier;
	class IObjectSpecificDataSupplier;
	class IObjectStateSupplier;
	class IUnitCommandSupplier;
	class UnitClassSupplier;
	struct PluginInternal;

	class DllExclusive FactionHumanPlugin : public DllGameMod
	{
	private:
		PluginInternal* _pluginInternal;

	public:
		FactionHumanPlugin();
		~FactionHumanPlugin();

		void Init();
		void Deinit();
	};
}
