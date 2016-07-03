#pragma once

#include "TypeDefs.h"

namespace FlagRTS
{
	// Simple 2d point on ints, provide minimal interface
	struct IntVector2
	{
		int X;
		int Y;
		IntVector2() { }
		IntVector2(const int x, const int y) :
			X(x), Y(y)
		{ }

		float Length() const
		{
			return std::sqrtf((float)(X*X+Y*Y));
		}

		float SquaredLength() const 
		{
			return (float)(X*X + Y*Y);
		}

		IntVector2& operator+=(const IntVector2& a)
		{
			X += a.X;
			Y += a.Y;
			return *this;
		}

		IntVector2& operator-=(const IntVector2& a)
		{
			X -= a.X;
			Y -= a.Y;
			return *this;
		}

		bool operator==(const IntVector2& a)
		{
			return X == a.X && Y == a.Y;
		}

		bool operator!=(const IntVector2& a)
		{
			return X != a.X || Y != a.Y;
		}
	};

	inline IntVector2 operator+(const IntVector2& a, const IntVector2& b)
	{
		return IntVector2(a.X + b.X, a.Y + b.Y);
	}

	inline IntVector2 operator-(const IntVector2& a, const IntVector2& b)
	{
		return IntVector2(a.X - b.X, a.Y - b.Y);
	}
}