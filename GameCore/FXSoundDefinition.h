#pragma once

#include "FXObjectDefinition.h"

namespace FlagRTS
{
	namespace Media
	{
		class Sound;
		class ISoundPlayer;
	}


	// Defines all sound effects placeable on Scene
	// Created FXSound instances should use common ISoundPlayer but each should
	// have own SoundSource created
	// Length of effect need not to be equal to length of sound ->
	// if its shorter, sound will be stopped before it finished
	class FXSoundDefinition : public FXObjectDefinition
	{
	protected:
		const char* _soundName;
		const char* _soundPack;
		Media::ISoundPlayer* _soundPlayer; // Sound player used - same for every instance of FXSound

		// Parameters of this sound
		float _gain;
		
		float _halfVolumeDistance;
		float _maxAttDistance;
		float _rolloffFactor;
		float _innerConeAngle;
		float _outerConeAngle;
		float _outerConeGain;

		Vector3 _positionOffset; // Offset from attached object center
		Quaternion _orientationOffset; // Offset orientation from attached object orentation
		bool _isSoundDirectional; // If sound is not directional it don't have directinal params set
		bool _isSoundSceneSound; // If sound is SceneSound it is not relative to Listener and have position/direction equal
								 // to SceneNode + Offset. If it is not its position/direction is constant and equal to Offset

	public:
		// Sets default values
		FXSoundDefinition();
		// Reads values from xml node
		FXSoundDefinition(XmlNode* soDefNode);
		~FXSoundDefinition();

		Media::ISoundPlayer* GetSoundPlayer() const { return _soundPlayer; }
		// Requests new sound player from AudioManager
		void UpdateSoundPlayer();

		const char* GetSoundName() const { return _soundName; }
		void SetSoundName(const char* name);

		const char* GetSoundPack() const { return _soundPack; }
		void SetSoundPack(const char* pack);

		bool IsDirectional() const { return _isSoundDirectional; }
		void SetIsDirectional(bool value) { _isSoundDirectional = value; }

		bool IsSceneSound() const { return _isSoundSceneSound; }
		void SetIsSceneSound(bool value) { _isSoundSceneSound = value; }

		const Vector3& GetPositionOffset() const { return _positionOffset; }
		void SetPositionOffset(const Vector3& pos) { _positionOffset = pos; }

		const Quaternion& GetOrientationOffset() const { return _orientationOffset; }
		void SetOrientationOffset(const Quaternion& off) { _orientationOffset = off; }

		float GetGain() const { return _gain; }
		void SetGain(float gain) { _gain = gain; }

		float GetHalfVolumeDistance() const { return _halfVolumeDistance; }
		void SetHalfVolumeDistance(float value) { _halfVolumeDistance = value; }

		float GetMaxAttDistance() const { return _maxAttDistance; }
		void SetMaxAttDistance(float dist) { _maxAttDistance = dist; }

		float GetRolloffFactor() const { return _rolloffFactor; }
		void SetRolloffFactor(float value) { _rolloffFactor = value; }
		
		float GetInnerConeAngle() const { return _innerConeAngle; }
		void SetInnerConeAngle(float value) { _innerConeAngle = value; }
		
		float GetOunterConeAngle() const { return _outerConeAngle; }
		void SetOuterConeAngle(float value) { _outerConeAngle = value; }
		
		float GetOunterConeGain() const { return _outerConeGain; }
		void SetOuterConeGain(float value) { _outerConeGain = value; }
	};
}