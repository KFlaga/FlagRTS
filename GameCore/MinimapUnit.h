#pragma once

#include "DataTypes.h"

namespace Ogre
{
	class Billboard;
}

namespace FlagRTS
{
	class SceneObject;

	// Represents unit on minimap 
	// Show as filled rectangle with given color -> color represents unit aligment to owner,
	// unit type and if unit is selected
	// For each visible unit theres one minimap unit
	//
	class MinimapUnit
	{
		SceneObject* _object;
		Ogre::Billboard* _normalBillboard;
		Ogre::Billboard* _selectedBillboard;
		size_t _setHandle; // Current BillMarker id

	public:
		MinimapUnit() :
			_object(0),
			_normalBillboard(0),
			_selectedBillboard(0),
			_setHandle(0)
		{ }

		SceneObject* GetGameObject() const { return _object; }
		void SetGameObject(SceneObject* object) { _object = object; }

		Ogre::Billboard* GetBillboard() const { return _normalBillboard; }
		void SetBillboard(Ogre::Billboard* billboard) { _normalBillboard = billboard; }
		
		Ogre::Billboard* GetSelectedBillboard() const { return _selectedBillboard; }
		void SetSelectedBillboard(Ogre::Billboard* billboard) { _selectedBillboard = billboard; }

		size_t GetMarkerHandle() const { return _setHandle; }
		void SetMarkerHandle(size_t handle) { _setHandle = handle; }
	};
}