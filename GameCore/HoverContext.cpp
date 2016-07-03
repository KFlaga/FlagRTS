#include "HoverContext.h"

namespace FlagRTS
{
	HoverContext::HoverContext() :
		_customLeftAction(false)
	{

	}

	HoverContext::~HoverContext()
	{

	}

	CustomHoverContext::CustomHoverContext(XmlNode* hcNode) :
		HoverContext()
	{
		// parse hover context
//		ParseHoverContext(ccNode->first_node("HoverContext",12));

		// Check if left action is customized

		// If it is, parse left target handlers
	}
}
