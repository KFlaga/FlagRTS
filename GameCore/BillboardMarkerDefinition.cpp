#include "BillboardMarkerDefinition.h"
#include <Exception.h>
#include "BillboardMarker.h"

namespace FlagRTS
{
	BillboardMarkerDefinition::BillboardMarkerDefinition() :
		SceneMarkerDefinition()
	{
		SetFinalType(GetTypeId<BillboardMarker>());
		SetFinalTypeName("BillboardMarker");
	}

	BillboardMarkerDefinition::BillboardMarkerDefinition(XmlNode* node) :
		SceneMarkerDefinition(node)
	{
		SetFinalType(GetTypeId<BillboardMarker>());
		SetFinalTypeName("BillboardMarker");

		XmlNode* mainNode = node->first_node("SceneMarker")->first_node("BillboardMarker");
		if(mainNode != 0)
		{
			_materialName = XmlUtility::XmlGetString(mainNode->first_node("Material"), "name");
			_materialGroup = XmlUtility::XmlGetString(mainNode->first_node("Material"), "group");
			_billboardType = ParseBillboardType(XmlUtility::XmlGetStringIfExists(
				mainNode->first_node("BillboardType"), "value", "point"));
			_originType = ParseBillboardOriginType(XmlUtility::XmlGetStringIfExists(
				mainNode->first_node("OriginType"), "value", "Center"));
			_defSize = XmlUtility::XmlGetXY(mainNode->first_node("DefaultSize"));
			_commonUp = XmlUtility::XmlGetXYZIfExists(mainNode->first_node("CommonUp"), Vector3(0.f,1.f,0.f));
			_poolSize = XmlUtility::XmlGetInt(mainNode->first_node("PoolSize"), "value");
			_commonDir = XmlUtility::XmlGetXYZIfExists(mainNode->first_node("CommonDirection"), Vector3(0.f,0.f,1.f));
			_autoUpdate = XmlUtility::XmlGetBoolIfExists(mainNode->first_node("AutoUpdate"), "value", true);
			_isInWorldSpace = XmlUtility::XmlGetBoolIfExists(mainNode->first_node("IsInWorldSpace"), "value", true);
			_cullIndividualy = XmlUtility::XmlGetBoolIfExists(mainNode->first_node("CullIndividualy"), "value", false);
			_useIdentityProjection = XmlUtility::XmlGetBoolIfExists(mainNode->first_node("UseIdentityProjection"), "value", false);
			_useIdentityView = XmlUtility::XmlGetBoolIfExists(mainNode->first_node("UseIdentityView"), "value", false);

		}
	}

	int BillboardMarkerDefinition::ParseBillboardType(const char* typeName)
	{
		if( strcmp(typeName, "point") == 0 )
			return Ogre::BBT_POINT;
		if( strcmp(typeName, "perpendicular_common") == 0 )
			return Ogre::BBT_PERPENDICULAR_COMMON;
		if( strcmp(typeName, "perpendicular_self") == 0 )
			return Ogre::BBT_PERPENDICULAR_SELF;
		if( strcmp(typeName, "oriented_common") == 0 )
			return Ogre::BBT_ORIENTED_COMMON;
		if( strcmp(typeName, "oriented_self") == 0 )
			return Ogre::BBT_ORIENTED_SELF;
		return Ogre::BBT_POINT;
	}

	int BillboardMarkerDefinition::ParseBillboardOriginType(const char* typeName)
	{
		if( strcmp(typeName, "c") == 0 )
			return Ogre::BBO_CENTER;
		if( strcmp(typeName, "b") == 0 )
			return Ogre::BBO_BOTTOM_CENTER;
		if( strcmp(typeName, "t") == 0 )
			return Ogre::BBO_TOP_CENTER;
		if( strcmp(typeName, "cl") == 0 )
			return Ogre::BBO_CENTER_LEFT;
		if( strcmp(typeName, "cr") == 0 )
			return Ogre::BBO_CENTER_RIGHT;
		if( strcmp(typeName, "bl") == 0 )
			return Ogre::BBO_BOTTOM_LEFT;
		if( strcmp(typeName, "br") == 0 )
			return Ogre::BBO_BOTTOM_RIGHT;
		if( strcmp(typeName, "tl") == 0 )
			return Ogre::BBO_TOP_LEFT;
		if( strcmp(typeName, "tr") == 0 )
			return Ogre::BBO_TOP_RIGHT;
		return Ogre::BBO_CENTER;
	}
}