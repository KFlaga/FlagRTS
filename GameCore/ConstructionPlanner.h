#pragma once

#include "DataTypes.h"

namespace PathFinding
{
	class UniformGridPathingMap;
};

namespace FlagRTS
{
	class ObjectOutline;
	class UnitDefinition;
	class CellColorer;

	struct PlanningResult
	{
		bool IsLocationValid;
		Vector3 PlannedPosition;
		UnitDefinition* UnitDef;
	};

	// Helps to construct buildings :
	// When builing is requested ( passed as UnitDefinition ), it creates outline for it
	// and spawns it under cursor position ( mouse position or any other point is updated extrnally )
	// It snaps building to fit the cells and check if all needed cells are free
	// If they are, it changes outline to valid, otherwise invalid
	// When cursor is pressed ( which should passed here as well ), it returns if builing can be placed, 
	// its position ( or if there obstacle option is to return some near valid position (like 1 cell difference))
	// and definition of planned building, so it can be placed afterwards
	class ConstructionPlanner
	{
	protected:
		Ogre::SceneManager* _ogreMgr;
		PathFinding::UniformGridPathingMap* _obstacleMap;
		ObjectOutline* _currentOutline;
		CellColorer* _validColorer;
		CellColorer* _invalidColorer;
		PlanningResult _result;
		IntVector2 _topLeftCell;

	public:
		ConstructionPlanner(Ogre::SceneManager* ogreMgr, PathFinding::UniformGridPathingMap* obstacleMap);
		virtual ~ConstructionPlanner();

		virtual void Update(float ms);

		virtual void SetPlannedObjectPosition(const Vector3& terrainPosition);
		const PlanningResult& AcceptPlan();
		virtual void AbandonPlan();
		virtual void StartNewPlan(UnitDefinition* plannedBuildingDef, const Vector3& startPosition);

		bool IsPlaning() const { return _currentOutline != 0; }

		const PlanningResult& GetLastPlanningResult() const { return _result; }

		virtual bool CheckPlanIsValid(UnitDefinition* plannedBuildingDef, 
			const Vector3& terrain, uint8 player);
	};
}