#include "DefaultCommandsSuppliers.h"
#include "Unit.h"
#include "DefaultCommands.h"
#include "HoverTargetTypes.h"
#include "GameWorld.h"
#include "TechnologyManager.h"

namespace FlagRTS
{
	void AttackGroundCommandSupplier::AddCommandsToUnit(Unit* unit)
	{
		auto commands = unit->GetCommands();
		BasicAttackCommand* attackCommand(xNew1(BasicAttackCommand, unit));
		commands->AddCommand(OrderTypes::Attack, attackCommand, 0);
		commands->AddRightCommand(HoverTargetTypes::EnemyUnit, attackCommand);
		commands->AddRightCommand(HoverTargetTypes::HostileUnit, attackCommand);
	}

	void MoveGroundCommandSupplier::AddCommandsToUnit(Unit* unit)
	{
		auto commands = unit->GetCommands();
		BasicMoveGroundCommand* moveCommand(xNew1(BasicMoveGroundCommand, unit));
		commands->AddCommand(OrderTypes::Move, moveCommand, 0);
		commands->AddRightCommand(HoverTargetTypes::ObstacleAir, moveCommand);
		commands->AddRightCommand(HoverTargetTypes::ObstacleNaval, moveCommand);
		commands->AddRightCommand(HoverTargetTypes::PassableGround, moveCommand);
		commands->AddRightCommand(HoverTargetTypes::Passable, moveCommand);
		commands->AddRightCommand(HoverTargetTypes::AllyUnit, moveCommand);
		commands->AddRightCommand(HoverTargetTypes::NeutralUnit, moveCommand);
		commands->AddRightCommand(HoverTargetTypes::OwnUnit, moveCommand);
		BasicRotateCommand* rotateCommand(xNew1(BasicRotateCommand, unit));
		commands->AddCommand(OrderTypes::Rotate, rotateCommand, 0);
	}

	void RotateCommandSupplier::AddCommandsToUnit(Unit* unit)
	{
		auto commands = unit->GetCommands();
		BasicRotateCommand* rotateCommand(xNew1(BasicRotateCommand, unit));
		commands->AddCommand(OrderTypes::Rotate, rotateCommand, 0);
	}

	void BaseCommandSupplier::AddCommandsToUnit(Unit* unit)
	{
		auto commands = unit->GetCommands();
		DoNothingCommand* noCommand(xNew1(DoNothingCommand, unit));
		commands->AddCommand(OrderTypes::NoCommand, noCommand, 0);
		commands->AddRightCommand(HoverTargetTypes::DefaultTarget, noCommand);
		commands->AddLeftCommand(HoverTargetTypes::DefaultTarget, noCommand);
	}

	void TrainUnitCommandSupplier::AddCommandsToUnit(Unit* unit)
	{
		auto commands = unit->GetCommands();
		auto& trainableUnits = unit->GetUnitDefinition()->GetTrainableUnitsArray();
		for(unsigned int i = 0; i < trainableUnits.size(); ++i)
		{
			auto trainCommand = xNew3(TrainUnitCommand,unit, trainableUnits[i]->DefinitionName, trainableUnits[i]->Number);
			// Add command for training unit in correct slot
			commands->AddCommand(trainCommand->GetOrderType(), trainCommand , 0);
			// Add all reqs to command
			auto playerTechs = GameWorld::GlobalWorld->GetTechnologyManager()->GetPlayerTechs(unit->GetOwner());
			for(unsigned int req = 0; req < trainableUnits[i]->RequirementNames.size(); ++req)
			{
				auto unitReq = playerTechs->FindRequirement(trainableUnits[i]->RequirementNames[req]);
				if(unitReq != 0)
				{
					trainCommand->GetRequirements().push_back(unitReq);
				}
			}
		}
	}
	
}