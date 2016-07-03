#pragma once

#include <PathFindingIDebugDrawer.h>
#include "DataTypes.h"

namespace FlagRTS
{
	class PathingDrawer : public PathFinding::IDebugDrawer
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

		void DrawLine(const PFVector2& startPoint, const PFVector2& endPoint);
		void DrawMessage(const PFVector2& position, const PFVector2& size, const char* text);

		void AttachToScene(Ogre::SceneNode* debugNode);
		void DetachFromScene();

		void ClearRender();
		void Draw();
	};
}