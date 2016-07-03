#include "ObjectDefinition.h"
#include "IKindSpecificDataSupplier.h"
#include "IObjectSpecificDataSupplier.h"

namespace FlagRTS
{
	ObjectDefinition::ObjectDefinition() :
		_objectDataName(""),
		_objectDataHandleSupplier(0),
		_kindDataHandleSupplier(0),
		_kindSpecificDataHandle(0)
	{

	}

	ObjectDefinition::ObjectDefinition(XmlNode* defNode) :
		_objectDataName(""),
		_objectDataHandleSupplier(0),
		_kindDataHandleSupplier(0),
		_kindSpecificDataHandle(0)
	{
		// Name of definition
		XmlAttribute* nameAtt = defNode->first_attribute("name",4,false);
		if(nameAtt != 0)
			SetName(nameAtt->value());
		// Object data if available
		XmlAttribute* dataAtt = defNode->first_attribute("data",4,false);
		if(dataAtt != 0)
			_objectDataName = dataAtt->value();
	}

	ObjectDefinition::~ObjectDefinition()
	{
		if(_kindSpecificDataHandle != 0)
			_kindDataHandleSupplier->DeleteHandle(_kindSpecificDataHandle);
	}

	size_t ObjectDefinition::RequestObjectDataHandle() const 
	{ 
		if(_objectDataHandleSupplier != 0)
			return _objectDataHandleSupplier->GetObjectSpecificDataHandle();
		else
			return 0;
	}

	void ObjectDefinition::DeleteObjectDataHandle(size_t handle) const
	{
		if(handle != 0)
			_objectDataHandleSupplier->DeleteHandle(handle);
	}
}