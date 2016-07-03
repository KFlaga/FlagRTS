#pragma once

namespace Ogre
{
	class SceneManager;
	class Mesh;
}

namespace PathFinding
{
	class UniformGridPathingMap;
}

namespace FlagRTS
{
	class Unit;
	class UnitDefinition;

	// Creates construction sites ( probably by calling GameWorld::CreateSceneObject with
	//	predefined UnitDefinition )
	class ConstructionSiteFactory
	{
	protected:
		Ogre::SceneManager* _ogreMgr;
		PathFinding::UniformGridPathingMap* _obstacleMap;

	public:
		ConstructionSiteFactory(Ogre::SceneManager* ogreMgr, PathFinding::UniformGridPathingMap* obstMap);

		virtual Unit* CreateSite(
			Unit* buildingUnderConstruction, int owner);

		virtual void DestroySite(Unit* cSite);

		virtual Unit* GetConstructedBuilding(Unit* site);

	protected:
		Ogre::Mesh* GenerateCSiteMesh(Unit* building, const char* name);
	};
}