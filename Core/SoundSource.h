#pragma once

#include "AudioPreReqs.h"
#include "Event.h"
#include <OgreVector3.h>

namespace FlagRTS
{
	namespace Media
	{
		class Sound;
		class AudioManager;

		// Wrappers around AlSources -> Sounds can be bind to source and be played with it
		// Actual playing is requested via AudioManager however
		class SoundSource
		{
		protected:
			Sound* _playedSound;
			SourceId _alSourceId;
			SoundState _state;
			SoundPriority _priority;
			float _progress;

			Vector3 _position;
			Vector3 _direction;
			Vector3 _velocity;
			float _gain;
			float _pitch;
			float _halfDistance;
			float _maxAttDistance;
			float _rolloff;
			float _outerConeGain;
			float _innerConeAngle;
			float _outerConeAngle;
			bool _isLoop;
			bool _relativeToListener;
			bool _changed;

			Event<SoundSource*, bool> _onFinished;

		public:
			SoundSource();
			~SoundSource();

			void SetSound(Sound* sound);
			Sound* GetSound() const { return _playedSound; }

			void SetALSource(SourceId alsource);
			SourceId GetAlSource() { return _alSourceId; }

			void SetCurrentState(SoundState state) { _state = state; }
			SoundState GetCurrentState() const { return _state; }

			// Updates sound progress
			virtual void Update(float ms);

			// Returns progress of current sound ( may be > 1.f for looped sounds )
			float GetProgress() const { return _progress; }

			// ALSource must be set before Starting
			// Sets stored fields to alSource and starts playing
			void Start();
			// ALSource must be set before Pausing
			// Pauses ALSource
			void Pause();
			// ALSource must be set before Stopping
			// Stops ALSource
			void Stop();
			// ALSource must be set before Stopping
			// Reqinds ALSource
			void Rewind();

			// Event used when non-looped sound finished playing
			// If second arg == false sound was stopped before finishing
			Event<SoundSource*, bool>& GetOnFinsihedEvent() { return _onFinished; }

			void SetPriority(SoundPriority priority) { _priority = priority; }
			SoundPriority GetPriority() const { return _priority; }

			void SetPosition(const Vector3& pos);
			const Vector3& GetPosition() const { return _position; }

			// To reset direction ( make sound non-directional ) set dir = (0.f,0.f,0.f)
			void SetDirection(const Vector3& direction);
			const Vector3& GetDirection() const { return _direction; }

			void SetVelocity(const Vector3& vel);
			const Vector3& GetVelocity() const { return _velocity; }

			// Gain in range [0.0-1.0]
			void SetGain(float gain);
			float GetGain() const { return _gain; }

			// Pitch in range [0.5-2.0]
			void SetPitch(float pitch);
			float GetPitch() { return _pitch; }

			// Maximal attuenation influence distance
			void SetMaxAttDistance(float dist);
			float GetMaxAttDistance() const { return _maxAttDistance; }

			// Rolloff factor influence attuenation
			void SetRolloffFactor(float rolloff);
			float GetRolloffFactor() const { return _rolloff; }

			// Distance at which volume is halved
			void SetHalfVolumeDistance(float halfDist);
			float GetHalfVolumeDistance() const { return _halfDistance; }

			void SetOuterConeGain(float gain);
			float GetOuterConeGain()  const { return _outerConeGain; }

			void SetInnerConeAngle(float innerConeAngle);
			float getInnerConeAngle() const { return _innerConeAngle; }

			void SetOuterConeAngle(float outerConeAngle);
			float GetOuterConeAngle() { return _outerConeAngle; }

			virtual void SetLoop(bool loop);
			bool IsLooping() const { return _isLoop; }

			void SetIsSourceRelativeToListener(bool value);
			bool IsSourceRelativeToListener() const { return _relativeToListener; }

		protected:
			void SetSourceValues();
			void CheckALError(const string& source);
			string ALErrorToString(int error);	
		};
	}
}