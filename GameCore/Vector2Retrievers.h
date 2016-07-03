#pragma once

#include "IObjectRetriever.h"
#include <OgreVector2.h>

namespace FlagRTS
{
	class Vector2RetrieverFactory
	{
	public:
		static IObjectRetriever<Vector2>* Create(XmlNode* vectorNode);
	};

	class Vector2FixedRetriever : public IObjectRetriever<Vector2>
	{
	private:
		Vector2 _vector;

	public:
		Vector2FixedRetriever(const Vector2& vector) :
			_vector(vector)
		{ }

		operator Vector2() const
		{
			return _vector;
		}
		
		IObjectRetriever<Vector2>* GetCopy() 
		{ 
			return xNew1(Vector2FixedRetriever, _vector); 
		}
	};

	class Vector2XmlXYZRetriever : public IObjectRetriever<Vector2>
	{
	private:
		Vector2 _vector;

	public:
		Vector2XmlXYZRetriever(XmlNode* vectorNode)
		{
			_vector = XmlUtility::XmlGetXY(vectorNode);
		}

		operator Vector2() const
		{
			return _vector;
		}
		
		IObjectRetriever<Vector2>* GetCopy() 
		{ 
			return xNew1(Vector2FixedRetriever, _vector); 
		}
	};
}