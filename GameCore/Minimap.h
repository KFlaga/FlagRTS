#pragma once

#include "DataTypes.h"
#include <Array.h>
#include <ArrayMap.h>
#include "ModelTexture.h"
#include <Event.h>

namespace Ogre
{
	class ManualObject;
	class SceneManager;
	class Material;
}

namespace FlagRTS
{
	class Unit;
	class PhysicalObject;
	class SceneObject;
	class MinimapUnit;
	class GameCamera;
	class BillboardMarker;
	class MiniMapDbvh;
	class TerrainBase;
	
	namespace MinimapFlags
	{
		enum FlagsType : int
		{
			None = 0,
			NeverVisible = 1<<0,
			AlwaysVisibile = 1<<1,
			RevealedVisible = 1<<2,
			RadarVisible = 1<<3,
			RadarIconVisible = 1<<4,
			VisibilityFlags = NeverVisible | AlwaysVisibile | RevealedVisible | RadarVisible | RadarIconVisible,

			MinimapSelectable = 1<<5,
			MinimapTargetable = 1<<6, // For commands

			OwnedUnit = 1<<7,
			AllyUnit = 1<<8,
			EnemyUnit = 1<<9,
			NeutralUnit = 1<<10,
			EnvironmentUnit = 1<<11,
			OwnerFlags = OwnedUnit | AllyUnit | EnemyUnit | NeutralUnit | EnvironmentUnit,

			Selected = 1<<12,
			UseCustomIcon = 1<<13
		};

		FlagsType ParseMinimapFlags(const char* name);
	}
	typedef MinimapFlags::FlagsType MinimapFlag;

	// Minimap is quad with terrain texture
	// Creates own SceneManager for managing MinimapUnits ( which are represented by Billboards )
	// Contains BillboardSets with units of each player + other icons
	// All this is rendered to one texture
	class Minimap
	{
	private:
		Array<MinimapUnit*> _allUnits;
		MiniMapDbvh* _unitsDbvh; // Dbvh for hittesting ( only selectable units are added )
		Vector2 _internalWorldSize;
		Vector2 _gameWorldSize;

		typedef ArrayMap<size_t, BillboardMarker*> MarkersMap;
		MarkersMap _unitMarkers; // Billboards marker for each color/icon type
		BillboardMarker* _selectedMarker;
		Ogre::SceneManager* _ogreMgr;
		Ogre::TexturePtr _finalTexture;
		Ogre::Camera* _renderCamera;
		Ogre::SceneNode* _cameraNode;
		Ogre::SceneNode* _bgNode;
		Ogre::ManualObject* _bgPlane;
		Ogre::MaterialPtr _terrainMaterial;

		GameCamera* _gameCamera;
		Ogre::SceneNode* _gameCameraNode;
		Ogre::ManualObject* _cameraBounds;
		bool _drawCameraBounds;
		float _zoomInv;

	public:
		Minimap(const Vector2& gameWorldSize, const IntVector2& finalTextureSize, 
			const string& terrainImageName, const string& terrainImageResourceGroup);
		~Minimap();

		void Update(float ms);

		// Begins rendering map to texture
		void BeginRender();
		// Ends rendering map to texture
		void EndRender();

		// Accepts point in uv coords [0-1] and returns SceneObject if one was hit or 0
		SceneObject* HitTest(const Vector2& screenMapPos);

		// Returns world (x,z) coords
		Vector2 ConvertMinimapToWorldCoords(const Vector2& screenMapPos);
		// Returns relative position of point on render plane (texture)
		// ( where [0-1] is in range of render plane )
		Vector2 ConvertWorldToMinimapCoords(const Vector2& worldPos);

		// Adds object to be shown on minimap
		void AddObject(SceneObject* unit);
		// Removes object from minimap
		void RemoveObject(SceneObject* unit);

		// Sets background texture ( material ) for minimap to be terrain's one
		void SetUseTerrainTexture();

		// Sets black background, only minimap units are visible
		void SetUseBlackTexture();

		// Sets background showing impassable areas for ground units
		void SetUsePathingTexture();

		// Sets camera from drawing bounds on map
		void SetActiveCamera(GameCamera* camera);

		// Enables/disables drawing of camera bounds on map
		void SetDrawCameraBounds(bool value);
		bool GetDrawCameraBounds() const { return _drawCameraBounds; }

		const Ogre::TexturePtr& GetRenderTexture() const { return _finalTexture; }

	private:
		void CreateRenderCamera();
		void CreateTerrainPlane();
		void CreateGameCameraBounds();
		void CreateBillboardMarkers();
		void LoadTerrainTexture(const string& terrainImageName, const string& terrainImageResourceGroup);

		BillboardMarker* CreateBillboardMarker(const string& materialName);

		Vector2 GetInternalCornerOfGameCamera(GameCamera* camera, float x, float y);

		void ObjectMoved(SceneObject*);
		DEFINE_DELEGATE1(ObjectMovedDelegate, Minimap,
			ObjectMoved, SceneObject*);
		ObjectMovedDelegate _onObjectMoved;
		
		void ObjectIsSelectedChanged(Unit*, bool);
		DEFINE_DELEGATE2(ObjectIsSelectedChangedDelegate, Minimap,
			ObjectIsSelectedChanged, Unit*, bool);
		ObjectIsSelectedChangedDelegate _onObjectSelected;
		
		void GameCameraMoved(SceneObject*);
		DEFINE_DELEGATE1(GameCameraMovedDelegate, Minimap,
			GameCameraMoved, SceneObject*);
		GameCameraMovedDelegate _onGameCameraMoved;
		
		void GameCameraRotated(SceneObject*);
		DEFINE_DELEGATE1(GameCameraRotatedDelegate, Minimap,
			GameCameraRotated, SceneObject*);
		GameCameraRotatedDelegate _onGameCameraRotated;
	};
}