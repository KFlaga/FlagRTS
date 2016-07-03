//#pragma once
//
//#include "Unit.h"
//#include "UnitGroup.h"
//
//namespace FlagRTS
//{
//	class IFormation
//	{
//	protected:
//		UnitGroup _units;
//		IntVector2 _maxUnitSize;
//		size_t _pathingHandle;
//
//	public:
//		IFormation();
//
//	};
//
//	class SubFormationGroup;
//
//	class FormationSlot
//	{
//	public:
//		Unit* _unit;
//		SubFormationGroup* _group;
//		Vector2 _position;
//	};
//
//	class FormationJoint
//	{
//	public:
//		FormationSlot* _slot1;
//		FormationSlot* _slot2;
//
//		float _desiredLength;
//		float _desiredAngle;
//		float _length;
//		float _angle;
//	};
//
//	// Part of formation units from one group ( specified UnitClass )
//	// There may be more than one subfromation for same classes
//	class SubFormationGroup
//	{
//	public:
//		enum BalancingPolicy
//		{
//			FillLineFirst,
//			BalancedLines
//		};
//
//		enum SpacingPolicy
//		{
//			MinSpread,
//			MaxSpread,
//		};
//
//	public:
//		Array<Unit*> _units;
//
//		Array<FormationSlot*> _slots;
//		Array<FormationJoint*> _joints;
//		Array<FormationJoint*> _bonds; // Joints to other groups
//
//		float _lineMax; // Max units per line (Lmax)
//		float _lineMin; // Min units per line (Lmin)
//		float _lineRatio; // Ratio width/height of slots (r)
//		float _slotSpread; // Distance between slots in line (ss)
//		float _lineSpread; // Distance between lines (ls)
//		BalancingPolicy _balancing; // How units are distibuted among lines (bp)
//		SpacingPolicy _spacing; // How units are distrubuted among slots in line (sp)
//
//		int _estimatedLinesCount;
//		int _linesCount;
//		int _estimatedLineSize;
//		int _lineSize;
//
//		// Estimates number of lines / slots / joints in group, ignoring other groups
//		void EstimateSize(Array<Unit*>& units)
//		{
//			// 1) Compute number of lines, considering Lmax, Lmin, r, bp
//
//			if( _balancing == BalancedLines )
//			{
//
//			}
//			else
//			{
//
//			}
//
//
//		}
//	};
//
//	class JointsBasedFormation : public IFormation
//	{
//		Array<SubFormationGroup*> _subFormations;
//
//		Array<FormationSlot*>* _slotsStorage;
//		Array<FormationJoint*>* _jointsStorage;
//
//	protected:
//
//		// Between group params
//		float _spacingBetweenGroups;
//		float _maxBondsPerUnit;
//
//		float _angleDiffCost; // Cost of difference between actual/desired angle for joint 
//		float _distDiffCost; // Cost of difference between actual/desired length for slot 
//	};
//}