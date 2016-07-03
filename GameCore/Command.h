#pragma once

#include "DataTypes.h"
#include "CommandTarget.h"
#include "Technology.h"
#include <Event.h>

namespace FlagRTS
{
	class Unit;
	class CommandRequirement;

	// 
	namespace OrderTypes
	{
		enum OrderType : int16
		{
			InvalidCommand = -1, // To notify errors
			NoCommand = 0, // No command to be ordered
			DefaultRight = 256, // Default command bind to right click ( choosen based on supplied target )
			DefaultLeft, // Default command bind to left click ( choosen based on supplied target )
			Move,
			MoveGroup, // Group command -> unit moves in formation
			Rotate,
			RotateGroup,
			Follow, // Follows target
			FollowGroup,
			Patrol, // Moves between points, depending on unit attack/repairs etc on unit's way
			PatrolGroup,
			Stop, // Stops all commands
			Hold, // Prevents unit from moving
			Attack, // Attacks specified target
			AttackGroup,
			AttackMove, // Moves to target attacking anything on path
			AttackMoveGroup,
			Gather, // Worker command - gather resources (for both direct/via building gathering)
			ReturnResources,
			Repair, // Worker command - repair building/mechanical units
			Build, // For building ( objects placed on map )
			AbortBuild, // Aborts building construction ( destorys building )
			Train, // For units ( object that are created inside buildings and spawns created )
			AbortTrain, // Aborts unit training
			UnitSubmenu, // Only changes available commands in current context ( like ability level-up or set of available buildings )
			LoadTarget, // For use by units that can garnizon
			LoadSelf, // For use by units that are to be garnizoned
			UnloadTarget,
			UnloadSelf,
			RallyPoint,
			Ability, // Use all kind of abilities ( all other than specified above )
			Internal, // Unit kind specific commands, unavailable to be ordered by user,
			Other,
			Train0, // Train commands -> to allow having multiple trainable units binded to CButtons
			Train1,
			Train2,
			Train3,
			Train4,
			Train5,
			Train6,
			Train7,
			Train8,
			Train9,
			Train10,
			Train11,
			Ability0, // Ability commands -> allows custom abilities to be binded to CButtons
			Ability1,
			Ability2,
			Ability3,
			Ability4,
			Ability5,
			Ability6,
			Ability7,
			Ability8,
			Ability9,
			Ability10,
			Ability11,
		};
		OrderType ParseOrderType(const string& order);

		const char* GetOrderName(OrderType order);
	}
	typedef OrderTypes::OrderType OrderType;

	namespace CommandCastTypes
	{
		enum CommandCastType : uint8
		{
			Instant = 1,
			SingleTarget = 2, // Command need target, based on type context is choosen
			Area = 4, // AreaCommand context is used
			None = 8, // Command cannot be casted by user ( for Internal orders only )
		};
		CommandCastType ParseCommandCastType(const string& castType);
	}
	typedef CommandCastTypes::CommandCastType CommandCastType;

	namespace CommandAligments
	{
		enum CommnandAligmentType : uint8
		{
			Friendly = 1, // abilities/commands that benefits units
			Neutral = 2, // abilities/commands that are neutral to units ( movement, building )
			Hostile = 4, // abilities/commands that involve hurting / debuffing units
			None = 8, // Command cannot be casted by user ( for Internal orders only )
		};
		CommnandAligmentType ParseCommandAligmentType(const string& aligment);
	}
	typedef CommandAligments::CommnandAligmentType CommandAligmentType;

	namespace CommandApplianceTypes
	{
		enum CommandApplianceType : uint8
		{
			None = 0, // Command unorderable
			ToAll, // Order this command to whole selection ( like move/attack )
			ToKind, // Order command to whole selected kind
			ToSingle, // Order command to one unit from selection
		};
		CommandApplianceType ParseCommandApplianceType(const string& aligment);
	}
	typedef CommandApplianceTypes::CommandApplianceType CommandApplianceType;

	class Command
	{
	protected:
		OrderType _orderType;
		CommandCastType _castType;
		CommandAligmentType _aligment;
		CommandApplianceType _appliance;
		TechAvailability _availability;

		Array<TechRequirement*> _requirements; // All requirements are owned by Command and deleted in destructor
		Event<Command*> _availabilityChanged;

		CommandTarget _target;
		Unit* _executor;
		bool _isFinished;
		bool _isSuspended;

	public:
		Command(Unit* owner, OrderType order, CommandCastType cast, CommandAligmentType aligment);

		Command(Unit* owner, Command* other);

		virtual ~Command();

		// Updates command execution
		virtual void Update(float ms) = 0;
		// Starts command execution
		virtual void Execute() = 0;
		// Ends command execution
		virtual void Abort() 
		{ 
			_isSuspended = false;
			_isFinished = true; 
		}
		// Allocates and returns copy of this command
		// TODO : change to CommandDefinition
		virtual Command* GetCopy() = 0;

		bool IsFinished() const { return _isFinished; }
		bool IsSuspended() const { return _isSuspended; }

		void SetTarget(const CommandTarget& target) { _target = target; }
		const CommandTarget& GetTarget() const { return _target; }
		CommandTarget* GetTargetPtr() { return &_target; }

		Unit* GetExecutor() const { return _executor; }
		void SetExecutor(Unit* exe) { _executor = exe; }

		OrderType GetOrderType() const { return _orderType; }
		CommandCastType GetCastType() const { return _castType; }
		CommandAligmentType GetAligmentType() const { return _aligment; }

		CommandApplianceType GetApplianceType() const { return _appliance; }
		void SetApplianceType(CommandApplianceType app) { _appliance = app; }

		virtual const char* GetName()
		{
			return OrderTypes::GetOrderName(_orderType);
		}

		// Checks if given comand target type is valid for this command
		virtual bool IsTargetValid(CommandTarget* target) const { return true; }

		// Temoprary suspends command's updates ( i.e. when priority command
		// was ordered ). In standard version just Aborts command
		virtual void Suspend() { Abort(); }
		// Resumes suspended command updates
		// In standard version executes it again
		virtual void Resume() { Execute(); }

		TechAvailability GetAvailability() { return _availability; }
		void SetAvailability(TechAvailability av) { _availability = av; }

		void CheckRequirements();
		Array<TechRequirement*>& GetRequirements() { return _requirements; }
		const Array<TechRequirement*>& GetRequirements() const { return _requirements; }

		Event<Command*>& CommandAvailabilityChanged() { return _availabilityChanged; }
	};
}