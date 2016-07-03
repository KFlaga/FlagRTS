#pragma once

#include "Memory.h"

#ifdef _DEBUG
#include "Array.h"
#endif

namespace FlagRTS
{
	// Represent static 2d array in row-major order
	template<typename T>
	class Array2d
	{
	protected:
#ifdef _DEBUG
		Array<Array<T>> _data;
#else
		T* _data;
#endif
		int _rows, _cols;
	public:
#ifdef _DEBUG
		Array2d(const int rows, const int cols) :
			_rows(rows), _cols(cols)
		{ 
			_data.resize(rows);
			for(int r = 0; r < rows; ++r)
			{
				_data[r].resize(cols);
			}
		}

		Array2d(const int rows, const int cols, const T& val) :
			_rows(rows), _cols(cols)
		{ 
			_data.resize(rows);
			for(int r = 0; r < rows; ++r)
			{
				_data[r].resize(cols, val);
			}
		}
		~Array2d() { }

		T& operator()(const int r, const int c) { return _data[r][c]; }
		const T& operator()(const int r, const int c) const { return _data[r][c]; }

		T& operator()(const IntVector2& pos) { return _data[pos.Y][pos.X]; }
		const T& operator()(const IntVector2& pos) const { return _data[pos.Y][pos.X]; }

		T GetCopyAt(const int r, const int c) const { return _data[r][c]; }
		T& GetAt(const int r, const int c) { return _data[r][c]; }
		const T& GetAt(const int r, const int c) const { return _data[r][c]; }
		void SetAt(const int r, const int c, const T& val) { _data[r][c] = val; }

		int GetRowCount() const { return _rows; }
		int GetColumnCount() const { return _cols; }

		// Zeroes array ( only for numerics/pointers )
		void Zero() 
		{ 
			for(int r = 0; r < _rows; ++r) 
			{ 
				for(int c = 0; c < _cols; ++c) 
					_data[r][c] = (T)0; 
			} 
		} 
		// Fills array with given value
		void Fill(const T& value) 
		{ 
			for(int r = 0; r < _rows; ++r) 
			{ 
				for(int c = 0; c < _cols; ++c) 
					_data[r][c] = value; 
			} 
		} 
#else
		Array2d(const int rows, const int cols) :
			_rows(rows), _cols(cols),
			_data(xNewArray0(T, rows*cols))
		{ }
		Array2d(const int rows, const int cols, const T& val) :
			_rows(rows), _cols(cols),
			_data(xNewArray1(T, rows*cols, val))
		{ 

		}
		~Array2d() { uDeleteArray(T, _data); }

		T& operator()(const int r, const int c) { return _data[r*_cols + c]; }
		const T& operator()(const int r, const int c) const { return _data[r*_cols + c]; }

		T& operator()(const IntVector2& pos) { return _data[pos.Y*_cols + pos.X]; }
		const T& operator()(const IntVector2& pos) const { return _data[pos.Y*_cols + pos.X]; }

		T GetCopyAt(const int r, const int c) const { return _data[r*_cols + c]; }
		T& GetAt(const int r, const int c) { return _data[r*_cols + c]; }
		const T& GetAt(const int r, const int c) const { return _data[r*_cols + c]; }
		void SetAt(const int r, const int c, const T& val) { _data[r*_cols + c] = val; }

		int GetRowCount() const { return _rows; }
		int GetColumnCount() const { return _cols; }

		// Zeroes array ( only for numerics/pointers )
		void Zero() { for(int v = 0, size = _rows*_cols; v < size; ++v) { _data[v] = (T)0; } } 
		// Fills array with given value
		void Fill(const T& value) { for(int v = 0, size = _rows*_cols; v < size; ++v) { _data[v] = value; } } 
#endif
	};
}