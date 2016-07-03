#pragma once

#include <DataTypes.h>
#include <OgreRay.h>
#include <unordered_map>
#include <Event.h>

namespace FlagRTS
{
	class HoverContext;
	class Command;
	class CommandTarget;
	class Unit;
	class SceneObject;

	class HoverController
	{
	private:
		Ogre::SceneManager* _ogreMgr;
		HoverContext* _selectionContext;
		HoverContext* _noSelectionContext;
		HoverContext* _disabledContext;
		std::unordered_map<int, HoverContext*> _commandContextes;
		HoverContext* _currentContext;
		bool _targetChanged;
		SceneObject* _hoveredObject;
		string _lastCursor;

	public:
		HoverController(Ogre::SceneManager* ogreMgr);
		~HoverController();
		void Update(const Ogre::Ray& cameraRay, CommandTarget* target);

		bool CheckIfTargetChanged() const { return _targetChanged; }
		SceneObject* GetHoveredObject() const { return _hoveredObject; }
		Unit* GetHoveredUnit() const;
		HoverContext* GetCurrentHoverContext() const { return _currentContext; }
		
		// Sets hover context to one used when hover targets are disabled
		// ( i.e. over gui, when selection box is shown )
		void ChangeHoverContextToHold();
		// Sets hover context to one used when nothing is selected
		void ChangeHoverContextToIdle();
		// Sets hover context based on supplied selected units group class
		void ChangeHoverContextToUnitGroup(UnitClass groupClass);
		// Sets hover context to command's one
		void ChangeHoverContextToCommand(Command* command, UnitClass groupClass);

		bool CustomLeftAction() const { return false; }

	private:
		SceneObject* CheckHover(const Ogre::Ray& cameraRay);

		void OnHoverTargetDespawned(SceneObject* obj)
		{
			_hoveredObject = 0;
		}
		DEFINE_DELEGATE1(HoverTargetDespawned, HoverController,
			OnHoverTargetDespawned, SceneObject*);
		HoverTargetDespawned _onHoverTargetDespawned;
	};
}