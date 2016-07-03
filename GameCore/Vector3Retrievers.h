#pragma once

#include "IObjectRetriever.h"
#include <OgreVector3.h>

namespace FlagRTS
{
	class SceneObject;

	class Vector3RetrieverFactory
	{
	public:
		static IObjectRetriever<Vector3>* Create(XmlNode* vectorNode);
	};

	class Vector3FixedRetriever : public IObjectRetriever<Vector3>
	{
	private:
		Vector3 _vector;

	public:
		Vector3FixedRetriever(const Vector3& vector) :
			_vector(vector)
		{ }

		operator Vector3() const
		{
			return _vector;
		}
		
		IObjectRetriever<Vector3>* GetCopy() 
		{ 
			return xNew1(Vector3FixedRetriever, _vector); 
		}
	};

	class Vector3XmlXYZRetriever : public IObjectRetriever<Vector3>
	{
	private:
		Vector3 _vector;

	public:
		Vector3XmlXYZRetriever(XmlNode* vectorNode)
		{
			_vector = XmlUtility::XmlGetXYZ(vectorNode);
		}

		operator Vector3() const
		{
			return _vector;
		}
		
		IObjectRetriever<Vector3>* GetCopy() 
		{ 
			return xNew1(Vector3FixedRetriever, _vector); 
		}
	};
	
	class Vector3ObjectPositionRetriever : public IObjectRetriever<Vector3>
	{
	private:
		IObjectRetriever<SceneObject*>* _object;

	public:
		Vector3ObjectPositionRetriever(IObjectRetriever<SceneObject*>* object) :
			_object(object)
		{ }
		~Vector3ObjectPositionRetriever();
		operator Vector3() const;
		
		IObjectRetriever<Vector3>* GetCopy() 
		{ 
			return xNew1(Vector3ObjectPositionRetriever, _object->GetCopy()); 
		}
	};

	class Vector3CursorTerrainPositionRetriever : public IObjectRetriever<Vector3>
	{
	public:
		Vector3CursorTerrainPositionRetriever() { }

		operator Vector3() const;
		
		IObjectRetriever<Vector3>* GetCopy() 
		{ 
			return xNew0(Vector3CursorTerrainPositionRetriever); 
		}
	};

	class Vector3StartPositionRetriever : public IObjectRetriever<Vector3>
	{
		IObjectRetriever<int>* _player;

	public:
		Vector3StartPositionRetriever(IObjectRetriever<int>* player) :
			_player(player)
		{ }

		operator Vector3() const;
		
		IObjectRetriever<Vector3>* GetCopy() 
		{ 
			return xNew1(Vector3StartPositionRetriever, _player->GetCopy()); 
		}
	};

	class Vector3VariableRetriever : public IObjectRetriever<Vector3>
	{
		string _varName;
	public:
		Vector3VariableRetriever(const string& value) :
			_varName(value)
		{ }

		operator Vector3() const;
		
		IObjectRetriever<Vector3>* GetCopy() 
		{ 
			return xNew1(Vector3VariableRetriever, _varName); 
		}
	};

	// Math functions
	class Vector3AddRetriever : public IObjectRetriever<Vector3>
	{
		IObjectRetriever<Vector3>* _v1;
		IObjectRetriever<Vector3>* _v2;
	public:
		Vector3AddRetriever(IObjectRetriever<Vector3>* v1, IObjectRetriever<Vector3>* v2) :
			_v1(v1), _v2(v2)
		{ }

		operator Vector3() const;
		
		IObjectRetriever<Vector3>* GetCopy() 
		{ 
			return xNew2(Vector3AddRetriever, _v1->GetCopy(), _v2->GetCopy()); 
		}
	};
}