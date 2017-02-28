#pragma once

#include "DataTypes.h"

namespace FlagRTS
{
	class PathingDrawer
	{
	protected:
		Ogre::SceneManager*_ogreMgr;
		Ogre::SceneNode* _debugNode;
		Ogre::ManualObject* _renderedLines;
		std::vector<Ogre::Vector3> _pointList;
		string _name;
		bool _linesDrawn;

	public:
		PathingDrawer(Ogre::SceneManager*, const Vector3& color, const string& name);
		~PathingDrawer();
		
		void DrawLine(const Vector3& startPoint, const Vector3& endPoint);
		void DrawLine(const Vector2& startPoint, const Vector2& endPoint);
		void DrawMessage(const Vector2& position, const Vector2& size, const char* text);

		void AttachToScene(Ogre::SceneNode* debugNode);
		void DetachFromScene();

		void ClearRender();
		void Draw();

		// Points in counter-clockwise order
		void DrawTriangle(const Vector2& v1, const Vector2& v2, const Vector2& v3)
		{
			DrawLine(v1,v2);
			DrawLine(v2,v3);
			DrawLine(v3,v1);
		}
		// Points in counter-clockwise order
		void DrawBox(const Vector2& v1, const Vector2& v2, const Vector2& v3, const Vector2& v4)
		{
			DrawLine(v1,v2);
			DrawLine(v2,v3);
			DrawLine(v3,v4);
			DrawLine(v4,v1);
		}
		// Points in order : cc-bottom starting with bot-left-back, then cc-top start top-left-back
		void DrawBox3d(const Vector3& v1, const Vector3& v2, const Vector3& v3, const Vector3& v4,
			const Vector3& v5, const Vector3& v6, const Vector3& v7, const Vector3& v8)
		{
			DrawLine(v1,v2);
			DrawLine(v2,v3);
			DrawLine(v3,v4);
			DrawLine(v4,v1);
			DrawLine(v5,v6);
			DrawLine(v6,v7);
			DrawLine(v7,v8);
			DrawLine(v8,v5);
			DrawLine(v1,v5);
			DrawLine(v2,v6);
			DrawLine(v3,v7);
			DrawLine(v4,v8);
		}
	};
}