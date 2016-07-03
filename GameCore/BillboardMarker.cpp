#include "BillboardMarker.h"
#include <OgreSceneManager.h>
#include <OgreBillboard.h>

namespace FlagRTS
{
	BillboardMarker::BillboardMarker(BillboardMarkerDefinition* def) :
		SceneMarker(def)
	{
		SetFinalType(GetTypeId<BillboardMarker>());
	}

	BillboardMarker::~BillboardMarker()
	{

	}

	void BillboardMarker::LoadResources(Ogre::SceneManager* ogreMgr)
	{
		if(!_loaded)
		{
			auto def = GetBillboardMarkerDefinition();
			Ogre::BillboardSet* billSet= ogreMgr->createBillboardSet(def->GetPoolSize());
			billSet->setMaterialName(def->GetMaterialName(), def->GetMaterialGroup());
			billSet->setBillboardType((Ogre::BillboardType)def->GetBillboardType());
			billSet->setCommonUpVector(def->GetCommonUpVector());
			billSet->setCommonDirection(def->GetCommonDirection());
			billSet->setBillboardsInWorldSpace(def->GetIsBillboardInWorldSpace());
			billSet->setBillboardOrigin((Ogre::BillboardOrigin)def->GetBillboardOrigin());
			billSet->setDefaultDimensions(def->GetDefaultBillboardSize().x, 
				def->GetDefaultBillboardSize().y);
			billSet->setAutoextend(true);
			billSet->setAutoUpdate(def->GetBillboardAutoUpdate());
			billSet->setUseIdentityProjection(def->GetUseIdentityProjection());
			billSet->setUseIdentityView(def->GetUseIdentityView());
			billSet->Ogre::MovableObject::setUserAny(Ogre::Any(this));
			billSet->setBillboardRotationType(Ogre::BBR_VERTEX);

			_sceneObject = billSet;

			SceneMarker::LoadResources(ogreMgr);
			_loaded = true;
		}
	}

	void BillboardMarker::UnloadResources(Ogre::SceneManager* ogreMgr)
	{
		if(_loaded)
		{
			ogreMgr->destroyBillboardSet(static_cast<Ogre::BillboardSet*>(_sceneObject));
			_sceneObject = 0;

			_loaded = false;
		}
	}

	void BillboardMarker::Spawn()
	{
		if( !_spawned )
		{
			_sceneNode->attachObject(_sceneObject);
			SceneMarker::Spawn();
			_spawned = true;
		}
	}

	void BillboardMarker::Despawn()
	{
		if( _spawned )
		{
			_sceneNode->detachObject(_sceneObject);
			SceneMarker::Despawn();
			_spawned = false;
		}
	}

	Ogre::Billboard* BillboardMarker::AddBillboard(const Vector3& position, const Radian rotation,
		const Ogre::ColourValue& color)
	{
		return CreateBillboard(position, rotation, color);
	}

	Ogre::Billboard* BillboardMarker::AddBillboard(const Vector3& position, const Radian rotation,
		const Ogre::FloatRect& textureCoords)
	{
		auto bill = CreateBillboard(position, rotation, Ogre::ColourValue::White);
		bill->setTexcoordRect(textureCoords);
		return bill;
	}

	Ogre::Billboard* BillboardMarker::AddBillboard(const Vector3& position, const Radian rotation,
		const Ogre::ColourValue& color, const Ogre::FloatRect& textureCoords)
	{
		auto bill = CreateBillboard(position, rotation, color);
		bill->setTexcoordRect(textureCoords);
		return bill;
	}
	
	inline Ogre::Billboard* BillboardMarker::CreateBillboard(const Vector3& position, 
		const Radian rotation, 
		const Ogre::ColourValue& color)
	{
		_ASSERT(_sceneObject != 0);
		auto billSet = GetBillboardSet();
		auto bill = billSet->createBillboard(position, color);
		bill->setRotation(rotation);
		return bill;
	}
}