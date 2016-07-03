#pragma once

#include "PathFindingUtils.h"

namespace PathFinding
{
	template<typename VectorT>
	class IPathUnit;

	// Interface for class that pairs units when one is within others look-up area
	template<typename VectorT>
	class IUnitUnitPairer
	{
	public:
		typedef IPathUnit<VectorT> UnitT;
		typedef PFArray<std::pair<UnitT*, PFArray<UnitT*>>> UnitPairsMap;

	protected:
		PFArray<UnitT*>* _units;
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

		PFArray<UnitT*>* GetUnitSource()
		{
			return _units;
		}

		UnitPairsMap& GetUnitPairsMap() { return _uuMap; }
		virtual void FindUnitPairs() = 0;
	};
}