#include "Minimap.h"
#include "MinimapUnit.h"
#include "Unit.h"
#include <OgreSceneManager.h>
#include <OgreTextureManager.h>
#include <OgreHardwarePixelBuffer.h>
#include <OgreManualObject.h>
#include <RenderSystemManager.h>
#include <PathFindingDbvh.h>
#include "BillboardMarker.h"
#include "GameWorld.h"
#include "Map.h"
#include "MapTerrain.h"
#include "GameCamera.h"
#include "MainGameObjectPool.h"

namespace FlagRTS
{
	inline size_t GetMinimapUnitHandle(MinimapUnit* unit)
	{
		return reinterpret_cast<size_t>(unit);
	}

	inline MinimapUnit* GetMinimapUnit(size_t handle)
	{
		return reinterpret_cast<MinimapUnit*>(handle);
	}

	inline PathFinding::Box GetMinimapShape(SceneObject* object, const Vector3& internalCenter)
	{
		return PathFinding::Box(
			Vector2(internalCenter.x - object->GetMinimapSize().x*0.5f, 
			internalCenter.z - object->GetMinimapSize().y*0.5f),
			Vector2(internalCenter.x + object->GetMinimapSize().x*0.5f, 
			internalCenter.z + object->GetMinimapSize().y*0.5f));
	}

	struct MinimapHitTest
	{
		Vector2 TestPoint;
		
		MinimapHitTest(const Vector2& point) : 
			TestPoint(point)
		{ }

		inline bool Intersect(const PathFinding::Box& box, Vector2& point) const
		{
			// Check few unit wide box with center below test point, as its quite hard to 
			// hit box with mouse cursor precisely
			point = TestPoint;
			return !(point.x < box.TopLeft.x ||
				point.x+7.f > box.BotRight.x ||
				point.y < box.TopLeft.y ||
				point.y+7.f > box.BotRight.y);
		}
	};

	class MiniMapDbvh : public PathFinding::Dbvh<PathFinding::Box>
	{
	public:
		MiniMapDbvh(float minMergeBen, float minRotBen) :
			PathFinding::Dbvh<PathFinding::Box>(minMergeBen, minRotBen)
		{ }
	};

	Minimap::Minimap(const Vector2& gameWorldSize, const IntVector2& finalTextureSize, 
		const string& terrainImageName, const string& terrainImageResourceGroup) :
	_ogreMgr(RenderSystemManager::Instance()->GetOgreRoot()->
		createSceneManager(Ogre::ST_GENERIC)),
		_gameCamera(0),
		_drawCameraBounds(false),
		_gameWorldSize(gameWorldSize),
		_zoomInv(1.f),
		_onObjectMoved(this),
		_onObjectSelected(this),
		_onGameCameraMoved(this),
		_onGameCameraRotated(this)
	{
		_ogreMgr->setAmbientLight(Ogre::ColourValue(1.f, 1.f, 1.f));

		_finalTexture = Ogre::TextureManager::getSingleton().createManual(
			"MinimapTex", 
			"Base", 
			Ogre::TEX_TYPE_2D, 
			finalTextureSize.X, finalTextureSize.Y, 0,
			Ogre::PF_R8G8B8, 
			Ogre::TU_RENDERTARGET);

		CreateRenderCamera();
		CreateTerrainPlane();
		CreateGameCameraBounds();
		CreateBillboardMarkers();
		LoadTerrainTexture(terrainImageName, terrainImageResourceGroup);

		_unitsDbvh = xNew2(MiniMapDbvh,0.5f,0.75f);
	}

	Minimap::~Minimap()
	{
		// TODO clean-up
	}

	void Minimap::CreateRenderCamera()
	{
		// Camera points directly top->down, placed on center of internal world
		_renderCamera = _ogreMgr->createCamera("MinimapCamera");
		_renderCamera->setDirection(Vector3::NEGATIVE_UNIT_Z);
		_renderCamera->setFarClipDistance(500.f);
		_renderCamera->setNearClipDistance(100.f);
		_renderCamera->setProjectionType(Ogre::ProjectionType::PT_ORTHOGRAPHIC);
		_renderCamera->setFrustumExtents(-_gameWorldSize.x*0.5f, 
			_gameWorldSize.x*0.5f, _gameWorldSize.y*0.5f, -_gameWorldSize.y*0.5f);

		_cameraNode = _ogreMgr->getRootSceneNode()->createChildSceneNode(
			"MinimapCameraNode", Vector3(_gameWorldSize.x*0.5f,300.f,_gameWorldSize.y*0.5f), 
			Quaternion(Radian(Degree(-90.f)), Vector3(1.f,0.f,0.f)));
		_cameraNode->attachObject(_renderCamera);
	}

	void Minimap::CreateTerrainPlane()
	{
		_bgNode = _ogreMgr->getRootSceneNode()->createChildSceneNode(
			"MinimapBgNode", Vector3(_gameWorldSize.x*0.5f,0.f,_gameWorldSize.y*0.5f));

		_bgPlane = _ogreMgr->createManualObject();
		_bgPlane->estimateVertexCount(4);
		_bgPlane->estimateIndexCount(6);

		_bgPlane->begin("MinimapBlackMat");
		_bgPlane->position(-_gameWorldSize.x*0.5f, 0.f,-_gameWorldSize.y*0.5f);
		_bgPlane->textureCoord(0.f,0.f);
		_bgPlane->position(-_gameWorldSize.x*0.5f, 0.f,_gameWorldSize.y*0.5f);
		_bgPlane->textureCoord(0.f,1.f);
		_bgPlane->position(_gameWorldSize.x*0.5f, 0.f,_gameWorldSize.y*0.5f);
		_bgPlane->textureCoord(1.f,1.f);
		_bgPlane->position(_gameWorldSize.x*0.5f, 0.f,-_gameWorldSize.y*0.5f);
		_bgPlane->textureCoord(1.f,0.f);
		_bgPlane->quad(0,1,2,3);
		_bgPlane->end();

		_bgNode->attachObject(_bgPlane);
	}

	void Minimap::CreateGameCameraBounds()
	{
		_cameraBounds = _ogreMgr->createManualObject();
		_cameraBounds->estimateVertexCount(8);
		_cameraBounds->estimateIndexCount(24);

		_cameraBounds->begin("MinimapWhiteMat", Ogre::RenderOperation::OT_TRIANGLE_LIST);
		_cameraBounds->position(0.f, 0.f, 0.f);
		_cameraBounds->position(0.f, 0.f, 4.f);
		_cameraBounds->position(4.f, 0.f, 4.f);
		_cameraBounds->position(4.f, 0.f, 0.f);
		_cameraBounds->position(1.f, 0.f, 1.f);
		_cameraBounds->position(3.f, 0.f, 1.f);
		_cameraBounds->position(3.f, 0.f, 3.f);
		_cameraBounds->position(3.f, 0.f, 1.f);
		_cameraBounds->quad(0,1,5,4);
		_cameraBounds->quad(1,2,6,5);
		_cameraBounds->quad(2,3,7,6);
		_cameraBounds->quad(3,0,4,7);
		_cameraBounds->end();

		_gameCameraNode = _ogreMgr->getRootSceneNode()->createChildSceneNode(
			"MinimapCameraBoundsNode", Vector3(0.f,15.f,0.f));
	}

	void Minimap::CreateBillboardMarkers()
	{
		// Create billboards markers for standard units
		auto marker = CreateBillboardMarker("MinimapOwnedMat");
		_unitMarkers[MinimapFlags::OwnedUnit] = marker;

		marker = CreateBillboardMarker("MinimapAllyMat");
		_unitMarkers[MinimapFlags::AllyUnit] = marker;

		marker = CreateBillboardMarker("MinimapAllyMat");
		_unitMarkers[MinimapFlags::NeutralUnit] = marker;

		marker = CreateBillboardMarker("MinimapNeutralMat");
		_unitMarkers[MinimapFlags::EnvironmentUnit] = marker;

		marker = CreateBillboardMarker("MinimapSelectedMat");
		_unitMarkers[MinimapFlags::Selected] = marker;
		_selectedMarker = marker;

		marker = CreateBillboardMarker("MinimapEnemyMat");
		_unitMarkers[MinimapFlags::EnemyUnit] = marker;
	}

	BillboardMarker* Minimap::CreateBillboardMarker(const string& materialName)
	{
		// Get global minimap billboard marker definition and change material used
		// Dont create it via GameWorld, as different scene manager is used for ogre loading
		BillboardMarker* marker = static_cast<BillboardMarker*>(
			GameWorld::GlobalWorld->GetObjectPool()->Create(
			GameWorld::GlobalWorld->GetSceneObjectDefinition("BillboardMarker", "MinimapMarker"),
			NEUTRAL_PLAYERNUM));
		marker->LoadResources(_ogreMgr);

		marker->GetBillboardSet()->setMaterialName(materialName, "Base");

		// Create scene node for billboard
		SceneNode* node = _ogreMgr->getRootSceneNode()->createChildSceneNode();
		marker->SetSceneNode(node);
		marker->Spawn();

		return marker;
	}

	void Minimap::LoadTerrainTexture(const string& terrainImageName, const string& terrainImageResourceGroup)
	{
		Ogre::Image img;

		img.load(terrainImageName, 
			terrainImageResourceGroup);

		auto texture = Ogre::TextureManager::getSingleton().createManual(
			"MinimapTerrainTex", "Base", 
			Ogre::TEX_TYPE_2D, 
			img.getWidth(), img.getHeight(), 
			0, img.getFormat(),
			Ogre::TU_DEFAULT);

		texture->loadImage(img);

		auto material = Ogre::MaterialManager::getSingleton().
			create("MinimapTerrainMat", "Base");

		material->setReceiveShadows(false);
		Ogre::TextureUnitState *texState = material->getTechnique(0)->getPass(0)->
			createTextureUnitState("MinimapTerrainTex");
		material->getTechnique(0)->setLightingEnabled(false);

		material->load();
	}

	void Minimap::Update(float ms)
	{
		static float updateTimer = 200.f;
		if( updateTimer <= 0.f )
		{
			_unitsDbvh->Optimize();
			updateTimer = 200.f;
		}
		else
			updateTimer -= ms;
	}

	void Minimap::BeginRender()
	{
		// Create render viewport for final texture
		Ogre::RenderTexture* renderTexture = _finalTexture->getBuffer()->getRenderTarget();
		renderTexture->addViewport(_renderCamera);
		renderTexture->getViewport(0)->setClearEveryFrame(true);
		renderTexture->getViewport(0)->setBackgroundColour(Ogre::ColourValue::Black);
		renderTexture->getViewport(0)->setOverlaysEnabled(false);
		renderTexture->getViewport(0)->setShadowsEnabled(false);
		renderTexture->getViewport(0)->setSkiesEnabled(false);
	}

	void Minimap::EndRender()
	{

		Ogre::RenderTexture* renderTexture = _finalTexture->getBuffer()->getRenderTarget();
		renderTexture->removeAllViewports();
	}

	SceneObject* Minimap::HitTest(const Vector2& screenMapPos)
	{
		Vector2 worldCoords(ConvertMinimapToWorldCoords(screenMapPos));

		Array<std::pair<SceneObject*, Vector2>> hitObjects;
		_unitsDbvh->TestRay(MinimapHitTest(worldCoords), hitObjects);

		if( hitObjects.size() == 0 )
			return 0;
		if( hitObjects.size() == 1 )
			return hitObjects[0].first;
		
		SceneObject* hit = 0;
		float minDist = 999999.f;
		// For multiple hits find closest one
		for(unsigned int i = 0; i < hitObjects.size(); ++i)
		{
			Vector2 pos(hitObjects[i].first->GetPositionAbsolute().x, 
				hitObjects[i].first->GetPositionAbsolute().z);
			float dist = pos.squaredDistance(worldCoords);
			if( pos.squaredDistance(worldCoords) < minDist )
			{
				hit = hitObjects[i].first;
				minDist = dist;
			}
		}

		return hit;
	}

	void Minimap::AddObject(SceneObject* object)
	{
		int mmapFlags = object->GetMinimapFlags();

		MinimapUnit* mmapUnit = xNew0(MinimapUnit);
		mmapUnit->SetGameObject(object);
		object->SetMinimapHandle(GetMinimapUnitHandle(mmapUnit));
		object->Moved() += &_onObjectMoved;

		_allUnits.push_back(mmapUnit);

		// Find / create billboard set for object
		MarkersMap::Iterator markerIt = _unitMarkers.end();
		if( (mmapFlags & MinimapFlags::UseCustomIcon) != 0 )
		{
			// Object kind uses custom icon -> get its handle
			size_t iconHandle = object->GetMinimapIconHandle();

			markerIt = _unitMarkers.find(iconHandle);
			if( markerIt == _unitMarkers.end() )
			{
				// If marker is not created yet -> create it
				BillboardMarker* marker = CreateBillboardMarker( object->GetMinimapIconMaterialName() );
				_unitMarkers.insert(iconHandle, marker);
				markerIt = _unitMarkers.find(iconHandle);
			}
		}
		else
			markerIt = _unitMarkers.find(mmapFlags & MinimapFlags::OwnerFlags);

		// Create billboard for object
		const Vector3& pos(object->GetPositionAbsolute());
		auto bill = markerIt->Value->AddBillboard( Vector3(pos.x, 10.f, pos.z), 
			object->GetOrientationAbsolute().getYaw() );
		bill->setDimensions( object->GetMinimapSize().x, object->GetMinimapSize().y );
		mmapUnit->SetBillboard(bill);
		mmapUnit->SetMarkerHandle(markerIt->Key);

		if( (mmapFlags & MinimapFlags::MinimapSelectable) != 0 )
		{
			// Create additional selected billboard for object
			auto sbill = _selectedMarker->AddBillboard(Vector3(pos.x, 10.f, pos.z),
				object->GetOrientationAbsolute().getYaw());
			sbill->setDimensions( 0.f, 0.f );
			mmapUnit->SetSelectedBillboard(sbill);

			// If unit is selectable add it to Dbvh for ray tests
			if( _allUnits.size() == 2 )
			{
				// Dbvh can br created with at least 2 objects
				PFArray<std::pair<SceneObject*, PathFinding::Box>> objects;
				objects.push_back(std::make_pair(_allUnits[0]->GetGameObject(), 
					GetMinimapShape(_allUnits[0]->GetGameObject(), _allUnits[0]->GetBillboard()->getPosition())));
				objects.push_back(std::make_pair(_allUnits[1]->GetGameObject(), 
					GetMinimapShape(_allUnits[1]->GetGameObject(), _allUnits[1]->GetBillboard()->getPosition())));
				_unitsDbvh->CreateInitialBvh(objects);
				static_cast<Unit*>(_allUnits[0]->GetGameObject())->IsSelectedChanged() += &_onObjectSelected;
				static_cast<Unit*>(_allUnits[1]->GetGameObject())->IsSelectedChanged() += &_onObjectSelected;
			}
			else if( _allUnits.size() > 2 )
			{
				_unitsDbvh->AddObject( object, GetMinimapShape(object, object->GetPositionAbsolute()));
				static_cast<Unit*>(object)->IsSelectedChanged() += &_onObjectSelected;
			}
		}
	}

	void Minimap::RemoveObject(SceneObject* object)
	{
		MinimapUnit* mmapUnit = GetMinimapUnit(object->GetMinimapHandle());
		if( (object->GetMinimapFlags() & MinimapFlags::MinimapSelectable) != 0 )
		{
			_selectedMarker->RemoveBillboard(mmapUnit->GetSelectedBillboard());
			_unitsDbvh->RemoveObject(object);
			static_cast<Unit*>(object)->IsSelectedChanged() -= &_onObjectSelected;
		}
		object->Moved() -= &_onObjectMoved;

		auto marker = _unitMarkers[mmapUnit->GetMarkerHandle()];
		marker->RemoveBillboard(mmapUnit->GetBillboard());
		_allUnits.remove(mmapUnit);
	}

	void Minimap::SetDrawCameraBounds(bool value)
	{
		if(_drawCameraBounds != value)
		{
			_drawCameraBounds = value;
			if(_drawCameraBounds && _gameCamera != 0)
			{
				_gameCameraNode->attachObject(_cameraBounds);
			}
			else
			{
				_gameCameraNode->detachAllObjects();
			}
		}
	}

	void Minimap::SetActiveCamera(GameCamera* camera)
	{
		if( _gameCamera != 0 )
		{
			_gameCameraNode->detachAllObjects();
			_gameCamera->Moved() -= &_onGameCameraMoved;
			_gameCamera->Rotated() -= &_onGameCameraRotated;
		}

		_gameCamera = camera;

		if( camera != 0 )
		{
			GameCameraMoved(camera);

			if( _drawCameraBounds )
			{
				_gameCameraNode->attachObject(_cameraBounds);
			}

			_gameCamera->Moved() += &_onGameCameraMoved;
			_gameCamera->Rotated() += &_onGameCameraRotated;
		}
	}

	Vector2 Minimap::ConvertMinimapToWorldCoords(const Vector2& screenMapPos)
	{
		// 1) get minimap bounds = internal bounds * zoomInv
		Vector2 size = _gameWorldSize * _zoomInv;
		// 2) convert range [0-1] to internal world coords and move to top-left corner
		return Vector2(size.x * screenMapPos.x + _cameraNode->getPosition().x - size.x*0.5f, 
			size.y * screenMapPos.y + _cameraNode->getPosition().z - size.y*0.5f);
	}

	Vector2 Minimap::ConvertWorldToMinimapCoords(const Vector2& internalCoords)
	{
		// Minimap coords may be outside of [0-1] -> point is not visible then
		// 1) Get visible minimap bounds
		Vector2 size = _gameWorldSize * _zoomInv;
		// 2) Convert coords
		return  Vector2((_gameWorldSize.x - _cameraNode->getPosition().x + size.x*0.5f)/size.x, 
			(_gameWorldSize.y - _cameraNode->getPosition().z + size.y*0.5f)/size.y);
	}

	void Minimap::SetUseTerrainTexture()
	{
		_bgPlane->setMaterialName(0, "MinimapTerrainMat", "Materials");
	}

	void Minimap::SetUseBlackTexture()
	{
		_bgPlane->setMaterialName(0, "MinimapBlackMat", "Materials");
	}

	void Minimap::ObjectMoved(SceneObject* object)
	{
		MinimapUnit* mmapUnit = GetMinimapUnit(object->GetMinimapHandle());
		const Vector3& pos(object->GetPositionAbsolute());
		mmapUnit->GetBillboard()->setPosition(Vector3(pos.x, 10.f, pos.z));
		mmapUnit->GetSelectedBillboard()->setPosition(Vector3(pos.x, 10.f, pos.z));

		if( (object->GetMinimapFlags() & MinimapFlags::MinimapSelectable) != 0 )
		{
			_unitsDbvh->ObjectChanged(object, PathFinding::Box(
				Vector2(pos.x - object->GetMinimapSize().x*0.5f, 
				pos.z - object->GetMinimapSize().y*0.5f),
				Vector2(pos.x + object->GetMinimapSize().x*0.5f, 
				pos.z + object->GetMinimapSize().y*0.5f)));
		}
	}

	void Minimap::ObjectIsSelectedChanged(Unit* unit, bool isSelected)
	{
		MinimapUnit* mmapUnit = GetMinimapUnit(unit->GetMinimapHandle());
		BillboardMarker* oldMarker = 0;
		BillboardMarker* newMarker = 0;
		if( isSelected == true )
		{
			// Unit selected
			unit->SetMinimapFlags( unit->GetMinimapFlags() | MinimapFlags::Selected );
			// Change billboard set of unit
			mmapUnit->GetBillboard()->setDimensions(0.f,0.f);
			mmapUnit->GetSelectedBillboard()->setDimensions(
				unit->GetMinimapSize().x, unit->GetMinimapSize().y);
		}
		else
		{
			// Unit de-selected
			unit->SetMinimapFlags( unit->GetMinimapFlags() & (~MinimapFlags::Selected) );
			// Change billboard set of unit
			mmapUnit->GetSelectedBillboard()->setDimensions(0.f,0.f);
			mmapUnit->GetBillboard()->setDimensions(
				unit->GetMinimapSize().x, unit->GetMinimapSize().y);
		}
	}

	void Minimap::GameCameraMoved(SceneObject* obj)
	{
		GameCamera* camera = static_cast<GameCamera*>(obj);
		// Get 4 corners of camera frustum in internal coords
		Vector2 corner1(GetInternalCornerOfGameCamera(camera, 0.f, 0.f));
		Vector2 corner2(GetInternalCornerOfGameCamera(camera, 0.f, 1.f));
		Vector2 corner3(GetInternalCornerOfGameCamera(camera, 1.f, 1.f));
		Vector2 corner4(GetInternalCornerOfGameCamera(camera, 1.f, 0.f));

		// Update camera bounds
		_cameraBounds->beginUpdate(0);
		// Set outside bounds to be corners of game camera frustum
		_cameraBounds->position(corner1.x, 0.f, corner1.y);
		_cameraBounds->position(corner2.x, 0.f, corner2.y);
		_cameraBounds->position(corner3.x, 0.f, corner3.y);
		_cameraBounds->position(corner4.x, 0.f, corner4.y);
		// Set inside corners so lines are few units wide ( 20 units / zoom )
		// Inside corners are directed towards opposite corners
		float lineWidth = std::min(_gameWorldSize.x, _gameWorldSize.y) * 0.015f * _zoomInv;
		Vector2 dir_1to3 = (corner3 - corner1).normalisedCopy() * lineWidth;
		Vector2 dir_2to4 = (corner4 - corner2).normalisedCopy() * lineWidth;
		_cameraBounds->position(corner1.x + dir_1to3.x, 0.f, corner1.y + dir_1to3.y);
		_cameraBounds->position(corner2.x + dir_2to4.x, 0.f, corner2.y + dir_2to4.y);
		_cameraBounds->position(corner3.x - dir_1to3.x, 0.f, corner3.y - dir_1to3.y);
		_cameraBounds->position(corner4.x - dir_2to4.x, 0.f, corner4.y - dir_2to4.y);
		// Set quads reresenting lines
		_cameraBounds->quad(0,1,5,4);
		_cameraBounds->quad(1,2,6,5);
		_cameraBounds->quad(2,3,7,6);
		_cameraBounds->quad(3,0,4,7);
		_cameraBounds->end();
	}

	Vector2 Minimap::GetInternalCornerOfGameCamera(GameCamera* camera, float x, float y)
	{
		Ogre::Ray ray(camera->GetOgreCamera()->getCameraToViewportRay(x, y));
		// Get intersection with terrain
		Vector3 gameCorner(GameWorld::GlobalWorld->GetMap()->GetTerrainInterscetion(ray));
		if( gameCorner.x < 0.f )
		{
			// Corner outside of map
			// Get intersection with much larger 0 plane then
			gameCorner = ray.getPoint(
				ray.intersects(
				Ogre::AxisAlignedBox(
				Vector3(-10*_gameWorldSize.x, -10.f, -10*_gameWorldSize.y),
				Vector3(11*_gameWorldSize.x, 0.f, 11*_gameWorldSize.y)))
				.second);
		}
		return Vector2(gameCorner.x, gameCorner.z);
	}

	void Minimap::GameCameraRotated(SceneObject* obj)
	{
		GameCameraMoved(obj);
	}

	namespace MinimapFlags
	{
		std::map<string, FlagsType>& GetFlagsTypeNamesMap()
		{
			static std::map<string, FlagsType> map;
			map["None"] = None;
			map["NeverVisible"] = NeverVisible;
			map["AlwaysVisibile"] = AlwaysVisibile;
			map["RevealedVisible"] = RevealedVisible;
			map["RadarVisible"] = RadarVisible;
			map["RadarIconVisible"] = RadarIconVisible;
			map["MinimapSelectable"] = MinimapSelectable;
			map["MinimapTargetable"] = MinimapTargetable;
			map["OwnedUnit"] = OwnedUnit;
			map["AllyUnit"] = AllyUnit;
			map["EnemyUnit"] = EnemyUnit;
			map["OwnerFlags"] = OwnerFlags;
			map["Selected"] = Selected;
			map["UseCustomIcon"] = UseCustomIcon;
			return map;
		}

		FlagsType ParseMinimapFlags(const char* name)
		{
			static std::map<string, FlagsType>& map = GetFlagsTypeNamesMap();
			auto typeIt = map.find(name);
			if(typeIt != map.end())
			{
				return typeIt->second;
			}
			return None;
		}
	}
}