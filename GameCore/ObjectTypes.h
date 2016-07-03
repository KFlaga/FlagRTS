#pragma once

// UNUSED FOR NOW

namespace FlagRTS
{
	namespace ObjectTypes
	{
		namespace 
		{
			enum OT_Shift
			{
				SceneObject = 0,
				Weapon,
				Effect,
				Mover,
				Command,
				PhysicalObject,
				GameCamera,
				VisualEffect,
				Sound,
				MapDecorator,
				Unit,
			};
		}

		enum ObjectType : unsigned int
		{
			Unknown = 0,
			SceneObject = 1 << OT_Shift::SceneObject,
			PhysicalObject = 1 << OT_Shift::PhysicalObject | ObjectType::SceneObject,
			MapDecorator = 1 << OT_Shift::MapDecorator | ObjectType::PhysicalObject,
			Unit = 1 << OT_Shift::Unit | ObjectType::PhysicalObject,
			GameCamera = 1 << OT_Shift::GameCamera | ObjectType::SceneObject,
			VisualEffect = 1 << OT_Shift::VisualEffect | ObjectType::SceneObject,
			Effect = 1 << OT_Shift::Effect,
			Mover = 1 << OT_Shift::Mover,
			Weapon = 1 << OT_Shift::Weapon,
			Command = 1 << OT_Shift::Command,
		};
	}
}