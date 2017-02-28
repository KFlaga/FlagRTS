#pragma once

#include <rapidxml.hpp>
#include "Pointers.h"
#include "TypeDefs.h"
#include "Array.h"

namespace FlagRTS
{


	// Typedef fro convienece
	typedef rapidxml::xml_document<char> XmlDoc;
	typedef rapidxml::xml_node<char> XmlNode;
	typedef rapidxml::xml_base<char> XmlBase;
	typedef rapidxml::xml_attribute<char> XmlAttribute;

	// RapidXml document which stores xml string
	class XmlDocument : public XmlDoc
	{
	private:
		char* _document;

	public:
		// Copies contents of string and pareses text
		XmlDocument(const std::string& xml);

		~XmlDocument();
	};

	// Static utility class for xml operations
	class XmlUtility
	{
	private:
		XmlUtility();
		XmlUtility(const XmlUtility&);
		XmlUtility& operator=(const XmlUtility&);

	public:
		// Returns XmlDoc from given filename ( or invalid pointer if failed to open/parse )
		static RefPtr<XmlDocument> XmlDocFromFile(const char* filename);

		// Parses XmlDoc from given text
		static RefPtr<XmlDocument> XmlDocFromString(const char* xmlTxt);

		// Returns XmlDoc from given file resource name in resource group using OgreResourceGroupManager
		static RefPtr<XmlDocument> XmlDocFromOgreResource(const char* resourceName, const char* groupName);

		// Returns all nodes that are childs or grand...childs of given node which have given name
		static Array<XmlNode*> XmlFindAllNodesWithName(XmlNode* parent, const char* nodeName);

		// Return value of attribute as float
		static float XmlGetFloat(XmlAttribute* att);
		// Return value of attribute with given name in given node as float
		static float XmlGetFloat(XmlNode* node, const char* attName, int nameSize = 0, bool caseDependent = false);
		// If node and atribute exists then returns attribute value as float. Otgerwise returns 'defVal'
		static float XmlGetFloatIfExists(XmlNode* node, const char* attName, float defVal = 0.0f, int nameSize = 0, bool caseDependent = false);

		// Return value of attribute as int
		static int XmlGetInt(XmlAttribute* att);
		// Return value of attribute with given name in given node as int
		static int XmlGetInt(XmlNode* node, const char* attName, int nameSize = 0, bool caseDependent = false);
		// If node and atribute exists then returns attribute value as int. Otgerwise returns 'defVal'
		static int XmlGetIntIfExists(XmlNode* node, const char* attName, int defVal = 0, int nameSize = 0, bool caseDependent = false);

		// Return value of attribute as const char*
		static const char* XmlGetString(XmlAttribute* att);
		// Return value of attribute with given name in given node as const char*
		static const char* XmlGetString(XmlNode* node, const char* attName, int nameSize = 0, bool caseDependent = false);
		// If node and atribute exists then returns attribute value as const char*. Otgerwise returns 'defVal'
		static const char* XmlGetStringIfExists(XmlNode* node, const char* attName, const char* defVal = "", int nameSize = 0, bool caseDependent = false);

		// Return string read from attribute text or loc/sec via LocalizationManager
		// depending on locale atributte in node
		static string XmlGetLocaleString(XmlNode* node);

		// Return value 'x','y','z' values of attributes in node
		static Vector3 XmlGetXYZ(XmlNode* node);
		static Vector3 XmlGetXYZIfExists(XmlNode* node, Vector3 defVal);
		
		// Returns 'x','y','z','w' as quaternion
		static Quaternion XmlGetQuaternion(XmlNode* node);
		static Quaternion XmlGetQuaternionIfExists(XmlNode* node, Quaternion defVal);
		
		// Return value 'x','y' values of attributes in node
		static Vector2 XmlGetXY(XmlNode* node);
		static Vector2 XmlGetXYIfExists(XmlNode* node, Vector2 defVal);

		// Return value 'x','z' values of attributes in node
		static Vector2 XmlGetXZ(XmlNode* node);
		static Vector2 XmlGetXZIfExists(XmlNode* node, Vector2 defVal);

		// Return value 'r','g','b' values of attributes in node
		static Vector3 XmlGetRGB(XmlNode* node);
		static Vector3 XmlGetRGBIfExists(XmlNode* node, Vector3 defVal);

		// Return value 'r','g','b','a' values of attributes in node
		static Vector4 XmlGetRGBA(XmlNode* node);
		static Vector4 XmlGetRGBAIfExists(XmlNode* node, Vector4 defVal);

		// Return value 't','l','b','r' (vector4 rect) values of attributes in node
		static Vector4 XmlGetTLBR(XmlNode* node);
		static Vector4 XmlGetTLBRIfExists(XmlNode* node, Vector4 defVal);

		// Return value of attribute as bool ( if value() == "true", otherwise returns false )
		static bool XmlGetBool(XmlAttribute* att);
		// Return value of attribute as bool ( if value() == "true", otherwise returns false )
		static bool XmlGetBool(XmlNode* node, const char* attName, int nameSize = 0, bool caseDependent = false);
		static bool XmlGetBoolIfExists(XmlNode* node, const char* attName, const bool defVal = false, int nameSize = 0, bool caseDependent = false);

		// Return value of flag saved in node -> if node exists return is same as XmlGetBool, but
		// if node with flag name doesn't exist in 'node' then returns defVal
		static bool XmlGetFlag(XmlNode* node, const char* flagName, bool defVal = false, int nameSize = 0, bool caseDependent = false);
	};
}