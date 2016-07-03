#pragma once

#include <Unit.h>
#include <UnitGroup.h>

namespace Ogre
{
	class SceneManager;
	class RegionSceneQuery;
}

namespace FlagRTS
{
	class CommandTarget;
	class CameraController;

	class SelectionBox;
	class SceneQuery_Select;
	class SceneQuery_AddToSelection;
	class SceneQuery_Deselect;

	class SelectionController
	{
		friend SceneQuery_Select;
		friend SceneQuery_AddToSelection;
		friend SceneQuery_Deselect;

	private:
		UnitGroup* _selectedUnits;
		Ogre::SceneManager* _ogreMgr;
		CameraController* _camController;
		SelectionBox* _sbox;
		SceneQuery_Select* _querySelect;
		SceneQuery_AddToSelection* _queryAddToSelection;
		SceneQuery_Deselect* _queryDeselect;
		int _selectionRange;
		bool _isNotOwnedUnitSelected;

		UnitGroup::UnitMap::iterator _currentSubgroupIt;
		Array<Unit*>* _currentSubgroup;
		size_t _currentSubgroupType;
		
		// Fired when some units are added/removed from selections ( in argument selected group )
		Event<UnitGroup*> _selectionChanged;
		// Fried when active selection subgroup ( with command context ) changes
		Event<Array<Unit*>*> _subgroupChanged;

		int _nextSelectionSound;

	public:
		SelectionController(CameraController* camCtrl, Ogre::SceneManager* ogre);
		~SelectionController();

		void Update(float ms);

		// Starts selection box
		void StartSelection(const Vector2& mousePos);

		void UpdateSelection(const Vector2& mousePos);
		// Ends selection box and selects units from it. If deselectOld = true, first deselects all units, 
		// otherwise adds to selection
		void EndSelection(bool deselectOld = true);
		// Ends selection box but do not change selected units
		void AbortSelection();
		// Selects hovered unit ( if valid unit )
		void SelectHovered(CommandTarget* hoverTarget, bool deselectOld = true);

		// Orders command to selected units
		void OrderSelected(int commandType, CommandTarget* target, bool queue)
		{
			_selectedUnits->OrderAll(commandType, target, queue);
		}

		// Orders command to selected units ( takes into account command appliance flags)
		void OrderSelected(Command* command, CommandTarget* target, bool queue);

		// Orders command to all selected units of given type
		void OrderSelectedOfType(size_t type, int commandType, CommandTarget* target, bool queue)
		{
			_selectedUnits->OrderAllOfType(type, commandType, target, queue);
		}

		// Orders command to one selected unit of given type ( command should have system
		// to determine which unit of group should be ordered to do so )
		void OrderOneUnitOfType(size_t type, int commandType, CommandTarget* target, bool queue);

		// Deselects all previously selected units and selects new one
		void SelectUnit(Unit* unit);

		// Selects all units whose middle-points are contained within selection box
		// Deselects all previously selected units
		void SelectAllFromBox(SelectionBox* box);

		// Selects all units whose middle-points are contained within selection box
		// Adds units to current selection
		void AddAllFromBox(SelectionBox* box);

		// Adds unit to selection
		void AddToSelection(Unit* unit);

		// Deselectd given unit if selected
		void DeselectUnit(Unit* unit);

		// Deselectes all units
		// Changes command context to default one
		void DeselectAll();

		// Select all owned units in selection range ( from camera center )
		void SelectAllInRange();

		// Select all owned units in selection range of given type 
		void SelectAllInRangeOfType(size_t type);

		// Removes all units of given type from seleted group
		void RemoveUnitTypeFromSelection(size_t type);
		
		// Removes all but of given type units from selection
		void LeaveOnlyUnitTypeInSelection(size_t type);

		// Selects all units from group
		void SelectGroup(UnitGroup* group)
		{
			_selectedUnits->CopyOther(group);
			_selectionChanged.Fire(_selectedUnits);
		}

		// Adds to selection all units from group
		void AddFromGroup(UnitGroup* group)
		{
			_selectedUnits->AddFromGroup(group);
			_selectionChanged.Fire(_selectedUnits);
		}

		UnitGroup* GetSelectedUnitGroup() 
		{
			return _selectedUnits;
		}

		unsigned int GetSelectedUnitCount()
		{
			return _selectedUnits->GroupCount();
		}

		UnitClass GetSelectedGroupClass() const
		{
			return _selectedUnits->GetGroupClass();
		}

		// Returns unit that represents group ( that is unit with highest priority
		// and with highest count in this priotity )
		Unit* GetGroupRepresentant()
		{
			_selectedUnits->GetGroupRepresentant();
		}

		// Returns list of selected units of given type or 0 if none selected
		Array<Unit*>* GetSubgroupOfType(size_t type)
		{
			auto subgroupIt = _selectedUnits->GetUnitsOfTypeIterator(type);
			if(subgroupIt != _selectedUnits->GetUnitGroupEnd())
				return &subgroupIt->second;
			return 0;
		}

		// Changes current context subgroup to one of given type
		// ( do not change if there's no units of given type selected )
		void SetContextSubgroupToType(size_t type);

		// Returns current context subgroup ( 0 if theres no units )
		Array<Unit*>* GetCurrentContextSubgroup()
		{
			return _currentSubgroup;
		}

		// Checks if current subgroup becomes empty and
		// if yes changes it to previous one or zero subgroup if all units are deselected
		void UpdateContextSubgroup();

		// Sets next context subgroup from unit group map
		void NextContextSubgroup();

		// Returns command set of first unit in context subgroup ( or 0 if none selected )
		UnitCommandSet* GetContextSubgroupCommands()
		{
			if(_currentSubgroup != 0)
				return (*_currentSubgroup->begin())->GetCommands();
			return 0;
		}

		// Returns true if selection consist of not-owned unit
		bool IsNotOwnedUnitSelected() const { return _isNotOwnedUnitSelected; }

		Event<UnitGroup*>& SelectionChanged() { return _selectionChanged; }
		Event<Array<Unit*>*>& SubgroupChanged() { return _subgroupChanged; }
		SelectionBox* GetSelectionBox() { return _sbox; }

	private:
		Ogre::RegionSceneQuery* CreateSceneQuery(SelectionBox* sbox);

		// Deselectes all units, but do not fire selection changed event
		// nor updates subgroup
		void DeselectAllSilient();

		void DeselectSilient(Unit* unit);
		void SelectSilient(Unit* unit);
		void AddToSelectionSilient(Unit* unit);

		bool CheckIfUnitShouldBeSelected(Unit* unit);

		void PlaySelectionSound(Unit* unit);

		void SelectedUnitDied(UnitGroup*, Unit*);
		DEFINE_DELEGATE2(SelectedUnitDiedDelegate, 
			SelectionController, SelectedUnitDied,
			UnitGroup*, Unit*);
		SelectedUnitDiedDelegate _onSelectedUnitDied;
	};
}