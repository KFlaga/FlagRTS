#pragma once

#include "IDataStorage.h"
#include <Array.h>


namespace FlagRTS
{
	/// Implementation of IDataSorage for storing object instance related data
	/**
	Key is index in data array, so access is pretty fast.
	Released keys are reused in LIFO manner.
	Underlying data array does not shrink when keys are released to ensure
	that returned keys remains valid.
	*/
	class ObjectDataStorage : public IDataStorage<size_t>
	{
		DISALLOW_COPY(ObjectDataStorage);

	protected:
		Array<Any> _dataStorage;
		Array<size_t> _emptySlots;

	public:
		ObjectDataStorage() { }
		~ObjectDataStorage() { }

		size_t ReserveKey()
		{
			if(_emptySlots.size() > 0)
			{
				// If we have released key, reuse it
				size_t key = _emptySlots.back();
				_emptySlots.pop_back();
				return key;
			}
			// New key is index of last element
			size_t key = _dataStorage.size();
			_dataStorage.push_back(Any());
			return key;
		}

		void ReleaseKey(size_t key)
		{
			if(IsKeyReserved(key))
			{
				_dataStorage[key] = Any();
				_emptySlots.push_back(key);
			}
		}

		bool IsKeyReserved(size_t key)
		{
			return _dataStorage.size() > key &&
				_emptySlots.getFirstIndexOf(key) < 0;
		}

		bool HaveDataSet(size_t key)
		{
			return _dataStorage.size() > key &&
				_dataStorage[key].empty() == false;
		}

	protected:
		void SetAny(size_t key, const Any& data)
		{
			_ASSERT(IsKeyReserved(key));
			_dataStorage[key] = data;
		}

		Any& GetAny(size_t key)
		{
			_ASSERT(IsKeyReserved(key));
			_ASSERT(HaveDataSet(key));
			return _dataStorage[key];
		}
	};
}