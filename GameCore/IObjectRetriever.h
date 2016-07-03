#pragma once

#include <Xml.h>

namespace FlagRTS
{
	template<typename ObjT>
	class IObjectRetriever
	{
	public:
		virtual operator ObjT() const = 0;

		virtual IObjectRetriever* GetCopy() = 0;
	};
}