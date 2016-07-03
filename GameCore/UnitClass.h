#pragma once

namespace FlagRTS
{
	namespace UnitClasses
	{
		enum SubClasses : uint8
		{
			// Behavior dependent part
			Aggresive = 0, // Unit that attacks by default ( attack on sight )
			Passive, // Unit that don't attack by default ( only return fire )
			Retreat, // Unit that don't retaliate, but retreat when attacked
			Worker,
			TransportResources, // Transports resources from mines to stores
			GatherResources, // Gathers resources from Resources directly
			TrainUnits, // Produces units
			ResourceFactory, // Produces resources (mines) that need to be transported to storage
			ResourceStorage, // Accepts resources from ResourceFactories
			ResourceGenerator, // Produces resources, that are gained instantly ( like power generators )
			Resource, // Unit is a resource
			ResearchFactory, // Can conduct researches
			UpgradeFactory, // Can produce unit upgrades
			ConstructionSite,
			// Unit move-space dependent part
			Ground,
			Air,
			Naval,
			Structure,
			Mobile,
			Immobile,
			// Attack capability part ( no mixed, use attack all )
			AttackGround,
			AttackAir,
			AttackNaval,
			AttackBuilding, // Ground and building attack is divided, as there can be units that can attack only buildings ( rams? )
			AttackRanged,
			AttackMelee,
			
			Healer, // Unit have heal abilities ( not repair )
			Repairer, // Unit can repair
			Converter, // Unit can converse other units

			TransportsUnits, // Unit capable of transorting other units

			// Unit purpose ( for formation position / AI )
			Melee, // Unit have short range, stands on front line
			Ranged, // Unit have some range, stands behind melee
			Support, // Units that benefits others, stands between other units (melee/ranged)
			Altillery, // Very long range units, stands on backlines
			NonAggressive, // Not-fighting units (ie workers), stands on backlines

			CustomClasssStart,
		};

		inline bool IsUnitOfSubClass(const UnitClass uclass, const uint8 checkClass)
		{
			return (uclass & (1ull<<checkClass)) != 0;
		}

		inline bool IsNotUnitOfSubClass(const UnitClass uclass, const uint8 checkClass)
		{
			return (uclass & (1ull<<checkClass)) == 0;
		}

		inline UnitClass AddClass(const UnitClass uclass, const uint8 addClass)
		{
			return uclass | (1ull<<addClass);
		}

		inline UnitClass RemoveClass(const UnitClass uclass, const uint8 remClass)
		{
			return uclass & ~(1ull<<remClass);
		}

		inline bool IsUnitOfComplexClass(const UnitClass uclass, const UnitClass checkClass)
		{
			return (uclass & checkClass) == checkClass;
		}

		inline bool IsNotUnitOfComplexClass(const UnitClass uclass, const UnitClass checkClass)
		{
			return (uclass & checkClass) != checkClass;
		}

		inline UnitClass CreateClass(const uint8 subClass)
		{
			return (1ull<<subClass);
		}

		inline UnitClass CreateClass(const uint8 sub1, const uint8 sub2)
		{
			return (1ull<<sub2) | CreateClass(sub1);
		}

		inline UnitClass CreateClass(const uint8 sub1, const uint8 sub2, const uint8 sub3)
		{
			return (1ull<<sub3) | CreateClass(sub1, sub2);
		}

		inline UnitClass CreateClass(const uint8 sub1, const uint8 sub2, const uint8 sub3, const uint8 sub4)
		{
			return (1ull<<sub4) | CreateClass(sub1, sub2, sub3);
		}

		// Some additional classes
		enum ComplexClasses : UnitClass
		{
			None = 0ll,
			Building = (1ull<<Ground) | (1ull<<Immobile) | (1ull<<Structure),
			DefenseTurret = Building | (1ull<<Aggresive),
			ResourceMine = Building | (1ull<<ResourceFactory),
			Laboratory = Building | (1ull<<ResearchFactory),
			Armory = Building | (1ull<<UpgradeFactory),
			MobileEngineer = (1ull<<Mobile) | (1ull<<Worker) | (1ull<<Repairer),
			MobileGround = (1ull<<Mobile) | (1ull<<Ground), 
			ImmobileGround = (1ull<<Immobile) | (1ull<<Ground), 
			MobileAir = (1ull<<Mobile) | (1ull<<Air), 
			ImmobileAir = (1ull<<Immobile) | (1ull<<Air),
		};
	}
}