#pragma once

#include "SceneMarker.h"
#include "BillboardMarkerDefinition.h"
#include <OgreBillboardSet.h>

namespace FlagRTS
{
	// SceneMarker using Ogre::Entity as scene object
	class BillboardMarker : public SceneMarker
	{
	public:
		BillboardMarker(BillboardMarkerDefinition* tpmDef);
		~BillboardMarker();

		BillboardMarkerDefinition* GetBillboardMarkerDefinition() const { return static_cast<BillboardMarkerDefinition*>(_definition); }

		void LoadResources(Ogre::SceneManager* ogreMgr);
		void UnloadResources(Ogre::SceneManager* ogreMgr);
		void Spawn();
		void Despawn();

		Ogre::BillboardSet* GetBillboardSet() { return static_cast<Ogre::BillboardSet*>(_sceneObject); }

		Ogre::Billboard* AddBillboard(const Vector3& position = Vector3::ZERO, 
			const Radian rotation = Radian(0.f),
			const Ogre::ColourValue& color = Ogre::ColourValue::White);
		
		Ogre::Billboard* AddBillboard(const Vector3& position, const Radian rotation,
			const Ogre::FloatRect& textureCoords);
		
		Ogre::Billboard* AddBillboard(const Vector3& position, const Radian rotation,
			const Ogre::ColourValue& color, const Ogre::FloatRect& textureCoords);

		void RemoveBillboard(Ogre::Billboard* bill)
		{
			GetBillboardSet()->removeBillboard(bill);
		}

	protected:
		Ogre::Billboard* CreateBillboard(const Vector3& position, 
			const Radian rotation, const Ogre::ColourValue& color);
	};
}