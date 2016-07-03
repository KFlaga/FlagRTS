#pragma once

#include "Sound.h"
#include <OgreVector3.h>

namespace FlagRTS
{
	namespace Media
	{
		// Wrapper around OpenAL Listener
		// Only one instance should be created in AudioManager
		// Listener position/direction is updated every frame if changed
		// It should be attached to scene node in AudioManager
		class SoundListener
		{
		protected:
			Vector3 _position;
			Vector3 _direction;
			Vector3 _velocity;
			Vector3 _up;
			ALfloat _orientation[6];
			float _gain;

			bool _transformChanged;

		public:
			SoundListener();
			~SoundListener();

			// Sets gain of the listener ( range [0-1] )
			// May be treated as master voulme, as all sound will be multiplied by this
			void SetGain(float gain);
			float GetGain() const { return _gain; }

			void SetPosition(const Vector3& vec);
			const Vector3& GetPosition() const;

			void SetDirection(const Vector3& vec);
			const Vector3& GetDirection() const;

			void SetUp(const Vector3& vec);
			const Vector3& GetUp() const;
		
			void SetVelocity(const Vector3& vec);
			const Vector3& GetVelocity() const;

			// Updates position of listener of changed
			void Update(float ms);

	protected:
			// Set default values
			void ResetValues();
		};
	}
}