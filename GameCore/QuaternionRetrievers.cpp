#include "QuaternionRetrievers.h"
#include <Exception.h>
#include "MainEventPool.h"

namespace FlagRTS
{
	
	IObjectRetriever<Quaternion>* QuaternionRetrieverFactory::Create(XmlNode* quatNode)
	{
		string from = XmlUtility::XmlGetString(quatNode, "from", 4);
		if(from.compare("wxyz") == 0)
		{
			return xNew1(QuaternionXmlWXYZRetriever, quatNode);
		}
		else if(from.compare("Variable") == 0)
		{
			const char* varName = XmlUtility::XmlGetString(quatNode, "value");
			return xNew1(QuaternionVariableRetriever, varName);
		}
		else
		{
			CastException((string("Bad from :") + from).c_str());
			return 0;
		}
		return 0;
	}

	QuaternionVariableRetriever::operator Quaternion() const
	{
		Variable& var = MainEventPool::GlobalPool->FindVariable(_varName);
		return *var.GetData<Quaternion>();
	}
}