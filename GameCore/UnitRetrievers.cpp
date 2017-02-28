#include "UnitRetrievers.h"
#include <Exception.h>

#include "IGameObjectPoolManager.h"
#include "MainEventPool.h"
#include "SceneObjectPools.h"
#include "GameWorld.h"
#include "IObjectDefinitionManager.h"
#include "ISceneObjectSpawner.h"
#include "OwnerRetrievers.h"

namespace FlagRTS
{
	IUnitRetriever* UnitRetrieverFactory::Create(XmlNode* soNode)
	{
		// Find retriever type
		string from = XmlUtility::XmlGetString(soNode, "from", 4);

		if(from.compare("Definition") == 0)
		{
			const char* name = XmlUtility::XmlGetString(soNode, "name");
			XmlNode* ownerNode = soNode->first_node("Owner");
			auto owner = OwnerRetrieverFactory::Create(ownerNode);
			return xNew2(UnitNewFromDefinitionRetriever, name, owner);
		}
		else if(from.compare("Variable") == 0)
		{
			const char* varName = XmlUtility::XmlGetString(soNode, "value");
			return xNew1(UnitVariableObjectRetriever, varName);
		}
		//else if(from.compare("Handle") == 0)
		//{
		//	return xNew0(UnitTriggeringObjectRetriever);
		//}
		else if(from.compare("LastCreated") == 0)
		{
			return xNew0(UnitLastCreatedRetriever);
		}
		else
		{
			CastException((string("Bad Unit from :") + from).c_str());
			return 0;
		}
	}

	UnitVariableObjectRetriever::operator Unit*() const
	{
		return *MainEventPool::GlobalPool->FindVariable(_varName).GetData<Unit*>();
	}

	UnitByHandleRetriever::operator Unit*() const
	{
		return reinterpret_cast<Unit*>(
			GameInterfaces::GetGameObjectPool()->FindByHandle(GetTypeId<Unit>(), _handle));
	}

	UnitLastCreatedRetriever::operator Unit*() const
	{
		return reinterpret_cast<Unit*>(
			GameInterfaces::GetGameObjectPoolManager()->GetObjectPool(GetTypeId<Unit>())->
			GetLastCreatedObject());
	}

	UnitNewFromDefinitionRetriever::operator Unit*() const
	{
		return reinterpret_cast<Unit*>(
			GameInterfaces::GetGameObjectPool()->Create(
			GameInterfaces::GetObjectDefinitionManager()->GetObjectDefinitionByName("Unit", _defName), *_owner));
	}
}