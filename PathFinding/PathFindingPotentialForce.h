#pragma once

#include "PathFindingUtils.h"

namespace PathFinding
{
	class PotentialForce
	{
	public:
		enum PotentialMaskShape
		{
			Circle = 0,
			Square = 1
		};

	private:
		float _coeffInside;
		float _coeffStrong;
		float _coeffWeak;
		float _strongFieldSize;
		float _weakFieldSize;

	public:
		PotentialForce() { }
		PotentialForce(float coeffInside, float coeffStrong, float strongSize, float coeffWeak, float weakSize) :
			_coeffInside(coeffInside),
			_coeffStrong(coeffStrong),
			_coeffWeak(coeffWeak),
			_strongFieldSize(strongSize),
			_weakFieldSize(weakSize)
		{ }

		void SetCoeffInside(float coeff) { _coeffInside = coeff; }
		float GetCoeffInside() const { return _coeffInside; }

		void SetCoeffStrong(float coeff) { _coeffStrong = coeff; }
		float GetCoeffStrong() { return _coeffStrong; }
		
		void SetCoeffWeak(float coeff) { _coeffWeak = coeff; }
		float GetCoeffWeak() { return _coeffWeak; }

		void SetStrongFieldRadius(float radius) { _strongFieldSize = radius; }
		float GetStrongFieldRadius() { return _strongFieldSize; }

		void SetWeakFieldRadius(float radius) { _weakFieldSize = radius; }
		float GetWeakFieldRadius() { return _weakFieldSize; }

		float ComputeForce(float distanceFromSource) const
		{
			if(distanceFromSource > _weakFieldSize)
				return 0.f;
			else if(distanceFromSource > _strongFieldSize)
				return _coeffWeak * (_weakFieldSize-distanceFromSource) / (_weakFieldSize-_strongFieldSize);
			else if(distanceFromSource > 0)
			{
				float ratio = (_strongFieldSize-distanceFromSource)/_strongFieldSize;
				return _coeffStrong * ratio + _coeffWeak * (1.f-ratio);
			}
			else
				return _coeffInside;
		}
	};
}