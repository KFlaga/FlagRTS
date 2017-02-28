#include "Xml.h"
#include <fstream>
#include <sstream>
#include <OgreVector2.h>
#include <OgreVector3.h>
#include <OgreVector4.h>
#include <cctype>
#include <memory.h>
#include "LocalizationManager.h"
#include <OgreResourceGroupManager.h>

namespace FlagRTS
{
	XmlDocument::XmlDocument(const std::string& xml)
	{
		_document = new char[xml.size() + 1];
		xml.copy(_document, xml.size());
		_document[xml.size()] = '\0';
		this->parse<0>(_document);
	}

	XmlDocument::~XmlDocument()
	{
		this->clear();
		delete _document;
	}

	RefPtr<XmlDocument> XmlUtility::XmlDocFromFile(const char* filename)
	{
		std::ifstream file;
		file.open(filename);

		if (!file.is_open())
		{
			return RefPtr<XmlDocument>::InvalidRefPointer();
		}

		std::stringstream sstream;
		sstream << file.rdbuf();
		const std::string str(sstream.str());

		XmlDocument* doc = xNew1(XmlDocument,str);
		file.close();
		sstream.clear();

		return RefPtr<XmlDocument>(doc);
	}

	RefPtr<XmlDocument> XmlUtility::XmlDocFromString(const char* xml)
	{
		XmlDocument* doc = xNew1(XmlDocument,xml);
		return RefPtr<XmlDocument>(doc);
	}

	
	void FindAllNodesWithName(XmlNode* parent, const char* nodeName, std::vector<XmlNode*> nodes)
	{
		XmlNode* node = parent->first_node();
		while(node != 0)
		{
			if( StringIsEqual()(nodeName, node->name()) )
			{
				nodes.push_back(node);
			}

			FindAllNodesWithName(node, nodeName, nodes);
			node = node->next_sibling();
		}
	}

	Array<XmlNode*> XmlFindAllNodesWithName(XmlNode* parent, const char* nodeName)
	{
		std::vector<XmlNode*> nodes;
		FindAllNodesWithName(parent, nodeName, nodes);
		return nodes;
	}

	RefPtr<XmlDocument> XmlUtility::XmlDocFromOgreResource(const char* resourceName, const char* groupName)
	{
		auto files = Ogre::ResourceGroupManager::getSingleton().findResourceFileInfo(
			groupName, resourceName);
		auto stream = files->begin()->archive->open(resourceName);
		string fileContents = stream->getAsString();
		return XmlUtility::XmlDocFromString(fileContents.c_str());
	}

		float  XmlUtility::XmlGetFloat(XmlAttribute* att)
		{
			char* num = att->value();
			int size = att->value_size();
			int pos = 0;
			// My own float parser, as std is culture dependent
			// Generally no error checking, just assume xml's are proper numbers
			float val = 0.0f;
			// Find if is negative
			int sign = 1;
			int dotPos = 0;
			if(*num == '-')
			{
				sign = -1;
				++num;
				++pos;
			}
			// Get intergral part
			while(pos < size)
			{
				if(*num == '.' || *num == ',')
				{
					break;
				}
				
				++dotPos;
				++pos;
				++num;
			}
			int mult = 1;
			for(int p = 1; p <= dotPos; p++)
			{
				val += (*(num - p) - '0')*mult;
				mult *= 10;
			}

			if(pos < size)
			{
				++num;
				++pos;
				// Get fractional part
				float div = 0.1f;
				while(pos < size)
				{
					val += (*num - '0') * div; 
					div *= 0.1f;
					++num;
					++pos;
				}
			}
			val *= sign;
			return val;
		}

		float  XmlUtility::XmlGetFloat(XmlNode* node, const char* attName, int nameSize, bool caseDependent)
		{
			return XmlGetFloat(node->first_attribute(attName, nameSize, caseDependent));
		}

		float  XmlUtility::XmlGetFloatIfExists(XmlNode* node, const char* attName, float defVal, int nameSize, bool caseDependent)
		{
			if(node == 0 )
				return defVal;

			XmlAttribute* att = node->first_attribute(attName, nameSize, caseDependent);
			if(att != 0)
				return XmlGetFloat(att);
			else
				return defVal;
		}

		int  XmlUtility::XmlGetInt(XmlAttribute* att)
		{
			return std::stoi(att->value());
		}

		int  XmlUtility::XmlGetInt(XmlNode* node, const char* attName, int nameSize, bool caseDependent)
		{
			return XmlGetInt(node->first_attribute(attName, nameSize, caseDependent));
		}

		int  XmlUtility::XmlGetIntIfExists(XmlNode* node, const char* attName, int defVal, int nameSize, bool caseDependent)
		{
			if(node == 0 )
				return defVal;

			XmlAttribute* att = node->first_attribute(attName, nameSize, caseDependent);
			if(att != 0)
				return std::stoi(att->value());
			else
				return defVal;
		}

		const char* XmlUtility::XmlGetString(XmlAttribute* att)
		{
			return att->value();
		}

		const char*  XmlUtility::XmlGetString(XmlNode* node, const char* attName, int nameSize , bool caseDependent)
		{
			return node->first_attribute(attName, nameSize, caseDependent)->value();
		}

		const char*  XmlUtility::XmlGetStringIfExists(XmlNode* node, const char* attName, const char* defVal, int nameSize , bool caseDependent)
		{
			if(node == 0 )
				return defVal;

			XmlAttribute* att = node->first_attribute(attName, nameSize, caseDependent);
			if(att != 0)
				return att->value();
			else
				return defVal;
		}

		
		bool XmlUtility::XmlGetBool(XmlAttribute* att)
		{
			return  ::strcmp(att->value(),"true") == 0 ? true : false;
		}

		bool XmlUtility::XmlGetBool(XmlNode* node, const char* attName, int nameSize, bool caseDependent)
		{
			return  ::strcmp(node->first_attribute(attName,nameSize,caseDependent)->value(),
							"true") == 0 ? true : false;
		}

		bool XmlUtility::XmlGetBoolIfExists(XmlNode* node, const char* attName, const bool defVal, int nameSize, bool caseDependent)
		{
			if(node == 0 )
				return defVal;

			XmlAttribute* att = node->first_attribute(attName,nameSize,caseDependent);
			if(att == 0)
				return defVal;

			return ::strcmp(att->value(), "true") == 0 ? true : false;
		}

		bool XmlUtility::XmlGetFlag(XmlNode* node, const char* flagName, bool defVal, int nameSize, bool caseDependent)
		{
			XmlNode* flagNode = node->first_node(flagName, nameSize, caseDependent);
			if(flagNode != 0)
			{
				return  ::strcmp(flagNode->first_attribute("value",5,false)->value(),
							"true") == 0 ? true : false;
			}
			else
				return defVal;
		}

		Vector3  XmlUtility::XmlGetXYZ(XmlNode* node)
		{
			Vector3 v;
			v.x = XmlGetFloat(node->first_attribute("x",1,false));
			v.y = XmlGetFloat(node->first_attribute("y",1,false));
			v.z = XmlGetFloat(node->first_attribute("z",1,false));
			return v;
		}

		Vector3  XmlUtility::XmlGetXYZIfExists(XmlNode* node, Vector3 defVal)
		{
			if(node == 0 )
				return defVal;

			Vector3 v;
			v.x = XmlGetFloatIfExists(node, "x", defVal.x, 1, false);
			v.y = XmlGetFloatIfExists(node, "y", defVal.y, 1, false);
			v.z = XmlGetFloatIfExists(node, "z", defVal.z, 1, false);
			return v;
		}
		
		Vector2  XmlUtility::XmlGetXY(XmlNode* node)
		{
			Vector2 v;
			v.x = XmlGetFloat(node->first_attribute("x",1,false));
			v.y = XmlGetFloat(node->first_attribute("y",1,false));
			return v;
		}

		Vector2  XmlUtility::XmlGetXYIfExists(XmlNode* node, Vector2 defVal)
		{
			if(node == 0 )
				return defVal;

			Vector2 v;
			v.x = XmlGetFloatIfExists(node, "x", defVal.x, 1, false);
			v.y = XmlGetFloatIfExists(node, "y", defVal.y, 1, false);
			return v;
		}

		Vector2  XmlUtility::XmlGetXZ(XmlNode* node)
		{
			Vector2 v;
			v.x = XmlGetFloat(node->first_attribute("x",1,false));
			v.y = XmlGetFloat(node->first_attribute("z",1,false));
			return v;
		}

		Vector2  XmlUtility::XmlGetXZIfExists(XmlNode* node, Vector2 defVal)
		{
			if(node == 0 )
				return defVal;

			Vector2 v;
			v.x = XmlGetFloatIfExists(node, "x", defVal.x, 1, false);
			v.y = XmlGetFloatIfExists(node, "z", defVal.y, 1, false);
			return v;
		}

		Vector3  XmlUtility::XmlGetRGB(XmlNode* node)
		{
			Vector3 v;
			v.x = XmlGetFloat(node->first_attribute("r",1,false));
			v.y = XmlGetFloat(node->first_attribute("g",1,false));
			v.z = XmlGetFloat(node->first_attribute("b",1,false));
			return v;
		}

		Vector3  XmlUtility::XmlGetRGBIfExists(XmlNode* node, Vector3 defVal)
		{
			if(node == 0 )
				return defVal;

			Vector3 v;
			v.x = XmlGetFloatIfExists(node, "r", defVal.x, 1, false);
			v.y = XmlGetFloatIfExists(node, "g", defVal.y, 1, false);
			v.z = XmlGetFloatIfExists(node, "b", defVal.z, 1, false);
			return v;
		}

		Vector4  XmlUtility::XmlGetRGBA(XmlNode* node)
		{
			Vector4 v;
			v.x = XmlGetFloat(node->first_attribute("r",1,false));
			v.y = XmlGetFloat(node->first_attribute("g",1,false));
			v.z = XmlGetFloat(node->first_attribute("b",1,false));
			v.w = XmlGetFloat(node->first_attribute("a",1,false));
			return v;
		}

		Vector4  XmlUtility::XmlGetRGBAIfExists(XmlNode* node, Vector4 defVal)
		{
			if(node == 0 )
				return defVal;

			Vector4 v;
			v.x = XmlGetFloatIfExists(node, "r", defVal.x, 1, false);
			v.y = XmlGetFloatIfExists(node, "g", defVal.y, 1, false);
			v.z = XmlGetFloatIfExists(node, "b", defVal.z, 1, false);
			v.w = XmlGetFloatIfExists(node, "a", defVal.w, 1, false);
			return v;
		}

		Vector4  XmlUtility::XmlGetTLBR(XmlNode* node)
		{
			Vector4 v;
			v.x = XmlGetFloat(node->first_attribute("t",1,false));
			v.y = XmlGetFloat(node->first_attribute("l",1,false));
			v.z = XmlGetFloat(node->first_attribute("b",1,false));
			v.w = XmlGetFloat(node->first_attribute("r",1,false));
			return v;
		}

		Vector4  XmlUtility::XmlGetTLBRIfExists(XmlNode* node, Vector4 defVal)
		{
			if(node == 0 )
				return defVal;

			Vector4 v;
			v.x = XmlGetFloatIfExists(node, "t", defVal.x, 1, false);
			v.y = XmlGetFloatIfExists(node, "l", defVal.y, 1, false);
			v.z = XmlGetFloatIfExists(node, "b", defVal.z, 1, false);
			v.w = XmlGetFloatIfExists(node, "r", defVal.w, 1, false);
			return v;
		}

		Quaternion XmlUtility::XmlGetQuaternion(XmlNode* node)
		{
			Quaternion q;
			q.x = XmlGetFloat(node->first_attribute("x",1,false));
			q.y = XmlGetFloat(node->first_attribute("y",1,false));
			q.z = XmlGetFloat(node->first_attribute("z",1,false));
			q.w = XmlGetFloat(node->first_attribute("w",1,false));
			return q;
		}

		Quaternion XmlUtility::XmlGetQuaternionIfExists(XmlNode* node, Quaternion defVal)
		{
			if(node == 0)
				return defVal;

			Quaternion q;
			q.x = XmlGetFloatIfExists(node, "x", defVal.x, 1, false);
			q.y = XmlGetFloatIfExists(node, "y", defVal.y, 1, false);
			q.z = XmlGetFloatIfExists(node, "z", defVal.z, 1, false);
			q.w = XmlGetFloatIfExists(node, "w", defVal.w, 1, false);
			return q;
		}

		string XmlUtility::XmlGetLocaleString(XmlNode* node)
		{
			// Check if locale == true or false
			if(XmlUtility::XmlGetBool(node, "locale", 6))
			{
				return GET_TEXT_XML(node);
			}
			else
			{
				return node->first_attribute("text",4)->value();
			}
		}
}
