#include "Vector3Retrievers.h"
#include <InputManager.h>
#include "SceneObject.h"
#include "ISceneObjectRetriever.h"
#include <Exception.h>
#include "GameWorld.h"
#include "Player.h"
#include "Map.h"
#include "OwnerRetrievers.h"
#include "MainEventPool.h"

namespace FlagRTS
{

	IObjectRetriever<Vector3>* Vector3RetrieverFactory::Create(XmlNode* vectorNode)
	{
		string from = XmlUtility::XmlGetString(vectorNode, "from", 4);
		if(from.compare("xyz") == 0)
		{
			return xNew1(Vector3XmlXYZRetriever, vectorNode);
		}
		else if(from.compare("CursorTerrain") == 0)
		{
			return xNew0(Vector3CursorTerrainPositionRetriever);
		}
		else if(from.compare("ObjectPosition") == 0)
		{
			IObjectRetriever<SceneObject*>* object = SceneObjectRetrieverFactory::Create(
				vectorNode->first_node("Object"));
			return xNew1(Vector3ObjectPositionRetriever, object);
		}
		else if(from.compare("StartLocation") == 0)
		{
			IObjectRetriever<int>* player = OwnerRetrieverFactory::Create(
				vectorNode->first_node("Player"));
			return xNew1(Vector3StartPositionRetriever, player);
		}
		else if(from.compare("Variable") == 0)
		{
			const char* varName = XmlUtility::XmlGetString(vectorNode, "value");
			return xNew1(Vector3VariableRetriever, varName);
		}
		else if(from.compare("Add") == 0)
		{
			IObjectRetriever<Vector3>* v1 = Vector3RetrieverFactory::Create(vectorNode->first_node());
			IObjectRetriever<Vector3>* v2 = Vector3RetrieverFactory::Create(vectorNode->last_node());
			return xNew2(Vector3AddRetriever, v1, v2);
		}
		else
		{
			CastException((string("Bad from :") + from).c_str());
			return 0;
		}
		return 0;
	}

	Vector3CursorTerrainPositionRetriever::operator Vector3() const
	{
		return InputManager::Instance()->GetMouseTerrainPos();
	}

	Vector3ObjectPositionRetriever::~Vector3ObjectPositionRetriever()
	{
		xDelete(_object);
	}

	Vector3ObjectPositionRetriever::operator Vector3() const
	{
		return static_cast<SceneObject*>(*_object)->GetPositionAbsolute();
	}

	Vector3StartPositionRetriever::operator Vector3() const
	{
		auto& loc = GameWorld::GlobalWorld->GetPlayers()->GetPlayer(*_player)->GetStartLocation();
		return Vector3(loc.x, GameWorld::GlobalWorld->GetMap()->GetTerrainHeight(loc.x,loc.y), loc.y);
	}

	Vector3VariableRetriever::operator Vector3() const
	{
		Variable& var = MainEventPool::GlobalPool->FindVariable(_varName);
		return *var.GetData<Vector3>();
	}

	Vector3AddRetriever::operator Vector3() const
	{
		return static_cast<Vector3>(*_v1) + static_cast<Vector3>(*_v2);
	}
}