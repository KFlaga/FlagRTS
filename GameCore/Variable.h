#pragma once

namespace FlagRTS
{
	// Data storage capable of storing any type (stores copy of data)
	class Variable
	{
	protected:
		struct Storage;
		Storage* _data;

	public:
		Variable() :
			_data(nullptr)
		{ }

		Variable(const Variable& other) :
			_data(other._data != nullptr ? other._data->Clone() : nullptr)
		{ }

		template<typename ValueT>
		Variable(const ValueT& value) :
			_data(new Holder<ValueT>(value))
		{ }

		~Variable()
		{
			delete _data;
		}

		Variable& Swap(Variable& other)
		{
			auto temp = _data;
			_data = other._data;
			other._data = temp;
			return *this;
		}

		template<typename ValueT>
		Variable& operator= (const ValueT& other)
		{
			delete _data;
			_data = new Variable::Holder<ValueT>(other);
			return *this;
		}

		Variable& operator= (const Variable& other)
		{
			delete _data;
			_data = other._data->Clone();
			return *this;
		}

		bool IsNull() const
		{
			return _data == nullptr;
		}

		template<typename ValueT>
		ValueT* GetData() const
		{
			Variable::Holder<ValueT>* data = reinterpret_cast<Variable::Holder<ValueT>*>(this->_data);
			if (data != nullptr)
				return &data->Data;
			return nullptr;
		}

		template<typename ValueT>
		void SetData(const ValueT& val) const
		{
			delete _data;
			_data = new Variable::Holder<ValueT>(val);
			return *this;
		}
		

		bool operator== (const Variable& other)
		{
			return *_data == *other._data;
		}
		
		bool operator!= (const Variable& other)
		{
			return *_data != *other._data;
		}
		
		bool operator< (const Variable& other)
		{
			return *_data < *other._data;
		}
		
		bool operator> (const Variable& other)
		{
			return *_data > *other._data;
		}

	protected:
		struct Storage
		{
		public:
			virtual ~Storage() { }

			virtual bool operator==(const Storage& other) = 0;
			virtual bool operator!=(const Storage& other) = 0;

			virtual bool operator<(const Storage& other) { return false; }
			virtual bool operator>(const Storage& other) { return false; }

		public:
			virtual Storage* Clone() const = 0;
		};

		template<typename ValueT>
		struct Holder :
			public Storage
		{
		public:
			ValueT Data;

		public:
			Holder(const ValueT& value) :
				Data(value)
			{
			}

			Storage* Clone() const
			{
				return new Holder(Data);
			}

			bool operator==(const Storage& other)
			{
				return Data == reinterpret_cast<const Holder<ValueT>&>(other).Data;
			}

			bool operator!=(const Storage& other)
			{
				return Data != reinterpret_cast<const Holder<ValueT>&>(other).Data;
			}

			bool operator<(const Storage& other)
			{
				return Data < reinterpret_cast<const Holder<ValueT>&>(other).Data;
			}

			bool operator>(const Storage& other)
			{
				return Data > reinterpret_cast<const Holder<ValueT>&>(other).Data;
			}

		private:
			Holder& operator=(const Holder& other) { }
		};
	};
}