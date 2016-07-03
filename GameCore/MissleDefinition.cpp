#include "MissleDefinition.h"
#include "Missle.h"
#include "GameWorld.h"
#include "MainGameObjectPool.h"
#include "IEffect.h"
#include "IMover.h"

namespace FlagRTS
{
	MissleDefinition::MissleDefinition() :
		PhysicalObjectDefinition(),
		_onImpactEffect(0),
		_mover(0)
	{
		SetFinalType(GetTypeId<Missle>());
		SetFinalTypeName("Missle");
	}

	MissleDefinition::MissleDefinition(XmlNode* soDefNode) :
		PhysicalObjectDefinition(soDefNode),
		_onImpactEffect(0),
		_mover(0)
	{
		SetFinalType(GetTypeId<Missle>());
		SetFinalTypeName("Missle");

		// Get node specific to MapDecorator
		XmlNode* missleNode = soDefNode->first_node("PhysicalObject")->
			first_node("Missle");

		_maxRange = XmlUtility::XmlGetFloatIfExists(missleNode->first_node("Range"), "value");

		const char* collTypeName = XmlUtility::XmlGetString(missleNode->first_node("CollisionType"), "value");
		if( strcmp(collTypeName, "OnObject") == 0 )
			_collisionType = MissleCollsionTypes::OnObject;
		else if( strcmp(collTypeName, "OnPoint") == 0 )
			_collisionType = MissleCollsionTypes::OnPoint;
		else
			_collisionType = MissleCollsionTypes::OnTarget;

		XmlNode* impactEffectNode = missleNode->first_node("OnImpact");
		if(impactEffectNode != 0)
		{
			string effectName = XmlUtility::XmlGetString(impactEffectNode, "effect");

			auto getEffect = [this, effectName]() 
			{
				IEffect* effect = static_cast<IEffect*>(
					GameWorld::GlobalWorld->GetGameObjectDefinition("Effect", effectName));
				_onImpactEffect = effect;
			};
			MainGameObjectPool::GlobalPool->OnAllDefinitionsLoaded() +=
				xNew1(DelegateEventHandler<decltype(getEffect)>, getEffect);
		}

		XmlNode* moverNode = missleNode->first_node("Mover");
		string moverName = XmlUtility::XmlGetString(moverNode, "definition");
		auto getMover = [this, moverName]() 
		{
			IMover* mover = static_cast<IMover*>(
				GameWorld::GlobalWorld->GetGameObjectDefinition("Mover", moverName));
			_mover = mover;
		};
		MainGameObjectPool::GlobalPool->OnAllDefinitionsLoaded() +=
			xNew1(DelegateEventHandler<decltype(getMover)>, getMover);
	}
}