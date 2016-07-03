#include "Command.h"
#include "TechRequirement.h"

namespace FlagRTS
{
	namespace OrderTypes
	{
		std::map<string, OrderType>& GetOrderNamesMap()
		{
			static std::map<string, OrderType> map;
			map["Stop"] = Stop;
			map["Gather"] = Gather;
			map["Hold"] = Hold;
			map["Move"] = Move;
			map["Rotate"] = Move;
			map["Attack"] = Attack;
			map["Patrol"] = Patrol;
			map["AttackMove"] = AttackMove;
			map["Ability"] = Ability;
			map["Follow"] = Follow;
			map["Repair"] = Repair;
			map["Other"] = Other;
			map["Internal"] = Internal;
			map["Submenu"] = UnitSubmenu;
			map["Load"] = LoadTarget;
			map["Unload"] = UnloadTarget;
			map["Train"] = Train;
			map["Train0"] = Train0;
			map["Train1"] = Train1;
			map["Train2"] = Train2;
			map["Train3"] = Train3;
			map["Train4"] = Train4;
			map["Train5"] = Train5;
			map["Train6"] = Train6;
			map["Train7"] = Train7;
			map["Train8"] = Train8;
			map["Train9"] = Train9;
			map["Train10"] = Train10;
			map["Train11"] = Train11;
			map["Ability0"] = Ability0;
			map["Ability1"] = Ability1;
			map["Ability2"] = Ability2;
			map["Ability3"] = Ability3;
			map["Ability4"] = Ability4;
			map["Ability5"] = Ability5;
			map["Ability6"] = Ability6;
			map["Ability7"] = Ability7;
			map["Ability8"] = Ability8;
			map["Ability9"] = Ability9;
			map["Ability10"] = Ability10;
			map["Ability11"] = Ability11;
			return map;
		}

		OrderType ParseOrderType(const string& order)
		{
			static std::map<string, OrderType>& map = GetOrderNamesMap();
			auto typeIt = map.find(order);
			if(typeIt != map.end())
			{
				return typeIt->second;
			}
			return InvalidCommand;
		}

		std::map<OrderType, const char*>& GetNamesByOrderMap()
		{
			static std::map<OrderType, const char*> map;
			map[Stop] = "Stop";
			map[Gather] = "Gather";
			map[Hold] = "Hold";
			map[Move] = "Move";
			map[Attack] = "Attack";
			map[Patrol] = "Patrol";
			map[AttackMove] = "AttackMove";
			map[Ability] = "Ability";
			map[Follow] = "Follow";
			map[Repair] = "Repair";
			map[Other] = "Other";
			map[Internal] = "Internal";
			map[UnitSubmenu] = "Submenu";
			map[LoadTarget] = "Load";
			map[UnloadTarget] = "Unload";
			map[Train] = "Train";
			map[Train0] = "Train0";
			map[Train1] = "Train1";
			map[Train2] = "Train2";
			map[Train3] = "Train3";
			map[Train4] = "Train4";
			map[Train5] = "Train5";
			map[Train6] = "Train6";
			map[Train7] = "Train7";
			map[Train8] = "Train8";
			map[Train9] = "Train9";
			map[Train10] = "Train10";
			map[Train11] = "Train11";
			map[Ability0] = "Ability0";
			map[Ability1] = "Ability1";
			map[Ability2] = "Ability2";
			map[Ability3] = "Ability3";
			map[Ability4] = "Ability4";
			map[Ability5] = "Ability5";
			map[Ability6] = "Ability6";
			map[Ability7] = "Ability7";
			map[Ability8] = "Ability8";
			map[Ability9] = "Ability9";
			map[Ability10] = "Ability10";
			map[Ability11] = "Ability11";
			return map;
		}

		const char* GetOrderName(OrderType order)
		{
			static std::map<OrderType, const char*>& map = GetNamesByOrderMap();
			auto typeIt = map.find(order);
			if(typeIt != map.end())
			{
				return typeIt->second;
			}
			return "Invalid";
		}
	}

	namespace CommandCastTypes
	{
		CommandCastType ParseCommandCastType(const string& castType)
		{
			if(castType.compare("Instant") == 0)
				return CommandCastType::Instant;
			if(castType.compare("Area") == 0)
				return CommandCastType::Area;
			if(castType.compare("Target") == 0)
				return CommandCastType::SingleTarget;
			if(castType.compare("None") == 0)
				return CommandCastType::None;
			return CommandCastType::None;
		}
	}

	namespace CommandApplianceTypes
	{
		CommandApplianceType ParseCommandApplianceType(const string& castType)
		{
			if(castType.compare("None") == 0)
				return CommandApplianceType::None;
			if(castType.compare("All") == 0)
				return CommandApplianceType::ToAll;
			if(castType.compare("Kind") == 0)
				return CommandApplianceType::ToKind;
			if(castType.compare("Single") == 0)
				return CommandApplianceType::ToSingle;
			return CommandApplianceType::None;
		}
	}

	Command::Command(Unit* owner, OrderType order, CommandCastType cast, CommandAligmentType aligment) :
		_executor(owner),
		_orderType(order),
		_castType(cast),
		_aligment(aligment),
		_appliance(CommandApplianceTypes::ToAll),
		_target(),
		_isFinished(false),
		_isSuspended(false),
		_availability(TechAvailability::Unchecked)
	{ }

	Command::Command(Unit* owner, Command* other) :
		_executor(owner),
		_orderType(other->_orderType),
		_castType(other->_castType),
		_aligment(other->_aligment),
		_appliance(other->_appliance),
		_target(),
		_isFinished(false),
		_isSuspended(false),
		_availability(TechAvailability::Unchecked)
	{ }

	Command::~Command()
	{
		// Delete all requirements
		for(unsigned int req = 0; req < _requirements.size(); ++req)
		{
			xDelete(_requirements[req]);
		}
	}

	void Command::CheckRequirements()
	{
		for(unsigned int req = 0; req < _requirements.size(); ++req)
		{
			_requirements[req]->CheckRequirementQuiet();
		}

		TechAvailability oldAv = _availability;
		TechAvailability newAv = GetFinalAvailability(_requirements);
		if( oldAv != newAv )
		{
			_availability = newAv;
			CommandAvailabilityChanged().Fire(this);
		}
	}
}