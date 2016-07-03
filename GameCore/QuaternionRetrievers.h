#pragma once

#include "IObjectRetriever.h"
#include <OgreQuaternion.h>

namespace FlagRTS
{
	class QuaternionRetrieverFactory
	{
	public:
		static IObjectRetriever<Quaternion>* Create(XmlNode* vectorNode);
	};

	class QuaternionFixedRetriever : public IObjectRetriever<Quaternion>
	{
	private:
		Quaternion _quaternion;

	public:
		QuaternionFixedRetriever(const Quaternion& q) :
			_quaternion(q)
		{ }

		operator Quaternion() const
		{
			return _quaternion;
		}
		
		IObjectRetriever<Quaternion>* GetCopy() 
		{ 
			return xNew1(QuaternionFixedRetriever, _quaternion); 
		}
	};

	class QuaternionXmlWXYZRetriever : public IObjectRetriever<Quaternion>
	{
	private:
		Quaternion _quaternion;

	public:
		QuaternionXmlWXYZRetriever(XmlNode* quatNode) :
			_quaternion(
				XmlUtility::XmlGetFloat(quatNode, "w", 1),
				XmlUtility::XmlGetFloat(quatNode, "x", 1),
				XmlUtility::XmlGetFloat(quatNode, "y", 1),
				XmlUtility::XmlGetFloat(quatNode, "z", 1))
		{

		}

		operator Quaternion() const
		{
			return _quaternion;
		}
		
		IObjectRetriever<Quaternion>* GetCopy() 
		{ 
			return xNew1(QuaternionFixedRetriever, _quaternion); 
		}
	};

	class QuaternionVariableRetriever : public IObjectRetriever<Quaternion>
	{
		string _varName;
	public:
		QuaternionVariableRetriever(const string& value) :
			_varName(value)
		{ }

		operator Quaternion() const;
		
		IObjectRetriever<Quaternion>* GetCopy() 
		{ 
			return xNew1(QuaternionVariableRetriever, _varName); 
		}
	};
}