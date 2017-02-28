#pragma once

#include "PathingDefs.h"
#include <Array.h>

namespace FlagRTS
{
	// Interface for class that pairs units when one is within others look-up area
	template<typename UnitT>
	class IUnitUnitPairer
	{
	public:
		typedef Array<std::pair<UnitT*, Array<UnitT*>>> UnitPairsMap;

	protected:
		Array<UnitT*>* _units;
		UnitPairsMap _uuMap;

	public:
		IUnitUnitPairer() :
			_units(0)
		{ }
		virtual ~IUnitUnitPairer() { }

		void SetUnitSource(PFArray<UnitT*>* units)
		{
			_units = units;
		}

		Array<UnitT*>* GetUnitSource()
		{
			return _units;
		}

		UnitPairsMap& GetUnitPairsMap() { return _uuMap; }
		virtual void FindUnitPairs() = 0;
	};
}