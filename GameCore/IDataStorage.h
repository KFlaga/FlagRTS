#pragma once

#include <TypeDefs.h>
#include <Any.h>

namespace FlagRTS
{
	/// Interface for generic data storage
	/**
		Works pretty much the same as map<KeyT, Any> with one difference,
		namely user request for a key to store access its data rather than
		supplying the key and value together.
		Data is wrapped in Any objects.
	*/
	template<typename KeyT>
	class IDataStorage
	{
	public:
		IDataStorage() { }
		virtual ~IDataStorage() { }

		/// Resrves a key to access data
		/**
			Returns unique key to access slot with Any.
			It may return same key as one previous calls if it was released.
			\returns key to access data
		*/
		virtual KeyT ReserveKey() = 0;

		/// Releases and invalidate key
		/**
			Releases key, which is invalidated and data cannot be accessed with it.
			Before release associated data should be deleted if needed to prevent memory leak.
			If key wasn't reserved does nothing.
			\param key previously released key
		*/
		virtual void ReleaseKey(KeyT key) = 0;
		
		/// Checks if key is reserved
		/**
			Checks if key is reserved
			\param key a key to check
			\returns true if key is reserved or false otherwise
		*/
		virtual bool IsKeyReserved(KeyT key) = 0;

		/// Sets arbitrary data
		/**
			Sets data of arbitrary type which can be accessed later by key.
			Supplied key must be reserved first.
			Any data previously set is overriden, so it should be deleted before seting new one
			if needed to prevent memory leak.
			\param key reserved key
			\param data arbitrary data to store
		*/
		template<typename DataType>
		void SetData(KeyT key, DataType&& data)
		{
			SetAny(key, Any(std::forward<DataType>(data)));
		}

		
		/// Checks if data is set
		/**
			Checks if data associated with supplied key was set
			\param key a key to check
			\returns true if key is reserved or false otherwise
		*/
		virtual bool HaveDataSet(KeyT key) = 0;
		
		/// Gets arbitrary data
		/**
			Returns reference to data stored in slot associated with supplied key, unless
			stored type is reference itself, then returns this stored reference.
			Key must be reserved first.
			Template parameter must be the same as one used when calling 'SetData' and
			data must be set before trying to access it.
			\param key reserved key
			\returns reference to data associated with the key
		*/
		template<typename DataType>
		boost::add_reference<DataType> GetData(KeyT key)
		{
			return GetAnyData(GetAny(key));
		}

	protected:
		/// Stores Any object
		virtual void SetAny(KeyT key, const Any& data) = 0;

		/// Retrieves Any object
		virtual Any& GetAny(KeyT key) = 0;
	};
}