#include "IGameObject.h"
#include "TypeId.h"

namespace FlagRTS
{
	IGameObject::IGameObject()
	{
		_handle.Type = GetTypeId<IGameObject>();
		_handle.Object = reinterpret_cast<size_t>(this);
	}
}