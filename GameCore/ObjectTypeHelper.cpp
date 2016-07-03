#include "ObjectTypeHelper.h"
#include "SceneObject.h"
#include "PhysicalObject.h"
#include "GameCamera.h"
#include "SceneObjectDefinition.h"
#include "PhysicalObjectDefinition.h"
#include "GameCameraDefinition.h"
#include "MapDecorator.h"
#include "MapDecoratorDefinition.h"
#include "Unit.h"
#include "Command.h"
#include "HoverContext.h"
#include "Weapon.h"
#include "WeaponDefinition.h"
#include "CommandButton.h"
#include "CommandButtonDefinition.h"

namespace FlagRTS
{
	namespace ObjectTypeHelpers
	{
		const char* FindTypeFromId(TypeId type)
		{
			if(type == GetTypeId<SceneObject>())
				return "SceneObject";

			else if(type == GetTypeId<SceneObjectDefinition>())
				return "SceneObjectDefinition";

			else if(type == GetTypeId<PhysicalObject>())
				return "PhysicalObject";

			else if(type == GetTypeId<PhysicalObjectDefinition>())
				return "PhysicalObjectDefinition";

			else if(type == GetTypeId<GameCamera>())
				return "GameCamera";

			else if(type == GetTypeId<GameCameraDefinition>())
				return "GameCameraDefinition";

			else if(type == GetTypeId<MapDecorator>())
				return "MapDecorator";

			else if(type == GetTypeId<MapDecoratorDefinition>())
				return "MapDecoratorDefinition";
			
			else if(type == GetTypeId<Unit>())
				return "Unit";

			else if(type == GetTypeId<UnitDefinition>())
				return "UnitDefinition";
			
			else if(type == GetTypeId<Command>())
				return "Command";

			else if(type == GetTypeId<Weapon>())
				return "Weapon";
			
			else if(type == GetTypeId<HoverContext>())
				return "HoverContext";

			else if(type == GetTypeId<CommandButton>())
				return "CommandButton";

			else if(type == GetTypeId<CommandButtonDefinition>())
				return "CommandButtonDefinition";
			
			else if(type == GetTypeId<Weapon>())
				return "Weapon";

			else if(type == GetTypeId<WeaponDefinition>())
				return "WeaponDefinition";

			return 0;
		}

		TypeId FindTypeFromString(const string& type)
		{
			if(type == "SceneObject")
				return GetTypeId<SceneObject>();

			else if(type == "SceneObjectDefinition")
				return GetTypeId<SceneObjectDefinition>();

			else if(type == "PhysicalObject")
				return GetTypeId<PhysicalObject>();

			else if(type == "PhysicalObjectDefinition")
				return GetTypeId<PhysicalObjectDefinition>();

			else if(type == "GameCamera")
				return GetTypeId<GameCamera>();

			else if(type == "GameCameraDefinition")
				return GetTypeId<GameCameraDefinition>();

			else if(type == "MapDecorator")
				return GetTypeId<MapDecorator>();

			else if(type == "MapDecoratorDefinition")
				return GetTypeId<MapDecoratorDefinition>();

			else if(type == "Unit")
				return GetTypeId<Unit>();

			else if(type == "UnitDefinition")
				return GetTypeId<UnitDefinition>();
			
			else if(type == "Command")
				return GetTypeId<Command>();

			else if(type == "Weapon")
				return GetTypeId<Weapon>();
			
			else if(type == "HoverContext")
				return GetTypeId<HoverContext>();

			else if(type == "CommandButton")
				return GetTypeId<CommandButton>();

			else if(type == "CommandButtonDefinition")
				return GetTypeId<CommandButtonDefinition>();
			
			else if(type == "Weapon")
				return GetTypeId<Weapon>();

			else if(type == "WeaponDefinition")
				return GetTypeId<WeaponDefinition>();

			return GetInvalidType();
		}
	}
}