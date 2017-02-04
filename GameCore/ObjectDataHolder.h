#pragma once

#include <TypeDefs.h>
#include <Array.h>
#include <HashMap.h>
#include <ArrayMap.h>

#include <Any.h>

namespace FlagRTS
{
	typedef size_t SlotId;
	typedef ArrayMaps<Any>::KeyCCString NamedParamsList;
	
	/// Store of object instance specific data
	/**
		Stores arbitrary data saved/accessed via key 
		Every game object can reserve a slot (or some slots) for Any
	*/
	class ObjectDataHolder
	{
		DISALLOW_COPY(ObjectDataHolder);

	protected:
		Array<Any> _dataStorage;
		Array<SlotId> _emptySlots;

	public:
		ObjectDataHolder();
		~ObjectDataHolder();

		///
		/**

		*/
		SlotId ReserveSlot();

		///
		/**

		*/
		void ReleaseSlot(SlotId slotHandle);

		///
		/**

		*/
		template<typename T>
		void SetData(SlotId slot, const boost::add_reference<T> data)
		{
			SetAny(slot, Any(data));
		}

		///
		/**

		*/
		template<typename T>
		boost::add_reference<T> GetData(SlotId slot)
		{
			return GetAnyData(GetAny(slot));
		}
		
	protected:
		void SetAny(SlotId slot, const Any& data)
		{
			// Check if slot is reserved
			_ASSERT(_dataStorage.size() > slot);
			_ASSERT(_emptySlots.getFirstIndexOf(slot) < 0);
			_dataStorage[slot] = data;
		}
		
		Any& GetAny(SlotId slot)
		{
			// Check if slot is reserved
			_ASSERT(_dataStorage.size() > slot);
			_ASSERT(_emptySlots.getFirstIndexOf(slot) < 0);
			return _dataStorage[slot];
		}
	};
}