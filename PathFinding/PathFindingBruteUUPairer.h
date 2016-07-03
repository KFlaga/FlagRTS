#pragma once

#include "PathFindingIUnitUnitPairer.h"
#include "PathFindingIPathUnit.h"

namespace PathFinding
{
	class BruteUUPairer : public IUnitUnitPairer<PFVector2>
	{
	public:
		BruteUUPairer(PFArray<UnitT*>* unitsSource) :
			IUnitUnitPairer(unitsSource)
		{ }

		void FindUnitForcePairs()
		{
			_uuMap.clear();
			_uuMap.reserve( _units->size() );
			UnitT* sourceUnit;
			UnitT* subjectUnit;
			for(unsigned int subject = 0; subject < _units->size(); ++subject)
			{
				subjectUnit = (*_units)[subject];
				_uuMap.push_back(std::make_pair( subjectUnit, PFArray<UnitT*>() ));
				for(unsigned int source = 0; source < _units->size(); ++source)
				{
					if(subject == source)
						continue;

					// Check if force source is within force subject lookup radius
					sourceUnit = (*_units)[source];
					// If source unit blocks subject it will repulse subject
					if((sourceUnit->GetCollsionFilter().Blocking & subjectUnit->GetCollsionFilter().Groups) != 0 &&
						 subjectUnit->IsWithinSight(sourceUnit))
					{
						_uuMap[subject].second.push_back(sourceUnit);
					}
				}
			}
		}
	};
}