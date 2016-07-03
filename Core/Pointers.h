#pragma once

#include <atomic>
#include "Memory.h"

namespace FlagRTS
{
	// Base class for smart poniters
	// If not subclassed, works like normal pointer
	template<class T>
	class Pointer
	{
	public:
		static const Pointer<T>& InvalidPointer()
		{
			static Pointer<T> invalid = Pointer<T>(0); 
			return invalid;
		}

	protected:
		T* _data;

	public:
		Pointer() : _data(0) { }

		Pointer(T* rawPtr)
		{
			_data = rawPtr;
		}

		virtual ~Pointer()
		{

		}

		virtual bool IsValid() const
		{
			return _data != 0;
		}

		operator T*() const
		{ 
			return _data; 
		}

		T* operator->() const
		{
			return _data;
		}

		T& operator*() const
		{
			return *_data;
		}

		T* GetRawPtr() const
		{
			return _data;
		}

		bool operator==(const Pointer<T>& ptr) const
		{
			return _data == ptr._data;
		}

		bool operator!=(const Pointer<T>& ptr) const
		{
			return _data != ptr._data;
		}

		bool operator>(const Pointer<T>& ptr) const
		{
			return _data > ptr._data;
		}

		bool operator<(const Pointer<T>& ptr) const
		{
			return _data < ptr._data;
		}

		template<typename TargetType>
		Pointer<TargetType> Cast() const
		{
			return Pointer<TargetType>( static_cast<TargetType*>(_data) );
		}

		template<typename TargetType>
		TargetType* CastRaw() const
		{
			return static_cast<TargetType*>(_data);
		}
	};

	// Refrence counter 'smart' pointer
	// Used in a same way as normal pointer, but automatically
	// deletes pointer when is no longer referenced
	// Reference counting should be thread safe by using atomic
	template<class T>
	class RefPtr : public Pointer<T>
	{
	public:
		static const RefPtr<T>& InvalidRefPointer()
		{
			static RefPtr<T> invalid = RefPtr<T>(0); 
			return invalid;
		}

	protected:
		std::atomic<int>* _refCount;

	public:
		RefPtr(T* rawPtr = 0) : Pointer(rawPtr),  _refCount(ndNew1(std::atomic<int>,(1)))
		{

		}

		RefPtr(T* rawPtr, std::atomic<int>* ref) : Pointer(rawPtr), _refCount(ref)
		{
			_refCount->fetch_add(1);
		}

		RefPtr(const RefPtr& ptr) : Pointer(ptr._data), _refCount(ptr._refCount)
		{
			_refCount->fetch_add(1);
		}

		RefPtr& operator=(const RefPtr& ptr)
		{
			Release();
			_data = ptr._data;
			_refCount = ptr._refCount;
			_refCount->fetch_add(1);

			return *this;
		}

		int Refrences() const 
		{
			return _refCount->load();
		}

		std::atomic<int>* GetRefernecesPtr() const
		{
			return _refCount;
		}

		bool IsValid() const
		{
			return _refCount->load() > 0 && _data != 0;
		}

		void Release()
		{
			int lastRef = 1; // If refs = 1, then its last ref
			int curRefsMinusOne = (_refCount->load()) - 1; // Current references not including this one
			// If current references == 1, then returns true - deletes data and set _refCount = 0
			// If current refrences > 1, then returns false and decrements refCount
			// Becouse only one thread can access this comparsion/decrement it is thread-safe
			if(_refCount->compare_exchange_strong(lastRef, curRefsMinusOne))
			{
				if(_data != 0)
					uDelete (T,_data);
				ndDelete(_refCount);
			}
			else
				_refCount->fetch_sub(1);
		}

		~RefPtr()
		{
			Release();
		}

		template<typename TargetType>
		RefPtr<TargetType> Cast() const
		{
			RefPtr<TargetType> ptr( static_cast<TargetType*>(_data), _refCount );
			return ptr;
		}
	};

	// Owned ptr cannot be copied - to copy WeakPtr must be used
	// Owned ptr is responsible for deleting raw ptr, do not transfer ownership
	// Raw ptr can be copied to weak ptr, which do not delete pointer in destructor,
	// the only advantage is that it knows if pointer was deleted
	// (WeakPtr can only be created using OwnedPtr)
	template<class T>
	class WeakPtr;

	template<class T>
	class OwnedPtr : public Pointer<T>
	{
	private:
		RefPtr<bool> _dataDeleted;

		OwnedPtr(const OwnedPtr& ptr);
		OwnedPtr& operator=(const OwnedPtr& ptr);
	public:
		OwnedPtr() : Pointer(0), _dataDeleted(xNew1(bool,(true)))
		{

		}

		OwnedPtr(T* rawPtr) : Pointer(rawPtr), _dataDeleted(xNew1(bool,(false)))
		{

		}

		OwnedPtr& operator=(const Pointer<T>& ptr)
		{
			_data = ptr.GetRawPtr();
			_dataDeleted = RefPtr<bool>(xNew1(bool,(false)));
			return *this;
		}

		WeakPtr<T> Copy() const
		{
			return WeakPtr<T>(*this, _dataDeleted);
		}

		// Deletes old data and sets new one
		void SetPtr(T* ptr)
		{
			if(_data != 0)
			{
				*_dataDeleted = true;
				uDelete (T,_data);
			}

			_data = ptr;
			_dataDeleted = RefPtr<bool>(xNew1(bool,(false)));
		}

		~OwnedPtr()
		{
			if(*_dataDeleted == false)
			{
				*_dataDeleted = true;
				uDelete (T,_data);
			}
		}
	};

	template<class T>
	class WeakPtr : public Pointer<T>
	{
	private:
		RefPtr<bool> _dataDeleted;

	public:
		WeakPtr() : Pointer(0), _dataDeleted(xNew1(bool,(true)))
		{
			
		}

		WeakPtr(const WeakPtr& ptr) : Pointer(ptr._data)
		{
			_dataDeleted = ptr._dataDeleted;
		}

		WeakPtr& operator=(const WeakPtr& ptr)
		{
			_data = ptr._data;
			_dataDeleted = ptr._dataDeleted;
			return *this;
		}

		WeakPtr(const OwnedPtr<T>& ptr, const RefPtr<bool>& dataDeleted) : Pointer(ptr.GetRawPtr())
		{
			_dataDeleted = dataDeleted;
		}

		~WeakPtr()
		{
			
		}

		bool IsValid() const
		{
			return *_dataDeleted;
		}
	};
}