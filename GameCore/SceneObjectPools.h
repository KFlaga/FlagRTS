#pragma once

#include "SceneObjectDefaultPool.h"
#include "MapDecorator.h"
#include "GameCamera.h"
#include "Unit.h"
#include "Missle.h"
#include "IUnitCommandSupplier.h"
#include "UnitClassSupplier.h"

namespace FlagRTS
{
	class MapDecoratorPool : public SceneObjectDefaultPool<MapDecorator, MapDecoratorDefinition>
	{
	public:
		MapDecoratorPool();
		~MapDecoratorPool();
		
		MapDecorator* CreateCast(MapDecoratorDefinition* objDef, int owner);

		void DestroyCast(MapDecorator* object);
	};

	class GameCameraPool : public SceneObjectDefaultPool<GameCamera, GameCameraDefinition>
	{
	public:
		GameCameraPool();
		~GameCameraPool();

		GameCamera* CreateCast(GameCameraDefinition* objDef, int owner);

		void DestroyCast(GameCamera* object);
	};

	class UnitPool : public SceneObjectDefaultPool<Unit, UnitDefinition>
	{
	private:
		ArrayMaps<IUnitCommandSupplier*>::KeyCCString _commandSuppliers;
		ArrayMaps<UnitClassSupplier*>::KeyCCString _classSuppliers;

	public:
		UnitPool();
		~UnitPool();

		Unit* CreateCast(UnitDefinition* objDef, int owner);

		// Prefered method to create units -> no longer
		//Unit* CreateUnit(UnitDefinition* objDef, uint8 owner);

		void DestroyCast(Unit* object);

		void RegisterCommandSupplier(const char* className, IUnitCommandSupplier* supplier)
		{
			auto suppIt = _commandSuppliers.find(className);
			if(suppIt != _commandSuppliers.end())
				xDelete(suppIt->Value);

			_commandSuppliers[className] = supplier;
		}

		void UnregisterCommandSupplier(const char* className)
		{
			_commandSuppliers.erase(className);
		}

		void RegisterUnitClassSupplier(const char* className, UnitClassSupplier* supplier)
		{
			auto suppIt = _classSuppliers.find(className);
			if(suppIt != _classSuppliers.end())
				xDelete(suppIt->Value);

			_classSuppliers[className] = supplier;
		}

		void UnregisterUnitClassSupplier(const char* className)
		{
			_classSuppliers.erase(className);
		}
	};

	class MisslePool : public SceneObjectDefaultPool<Missle, MissleDefinition>
	{
	public:
		MisslePool();
		~MisslePool();
		
		Missle* CreateCast(MissleDefinition* objDef, int owner);

		void DestroyCast(Missle* object);
	};
}