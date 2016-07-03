#pragma once

#include "FXObject.h"
#include "FXSoundDefinition.h"

namespace FlagRTS
{
	
namespace Media
{
	class SoundSource;
}

	class FXSound : public FXObject
	{
	protected:
		Media::SoundSource* _soundSource;

	public:
		FXSound(FXSoundDefinition* sfxDef);
		~FXSound();

		FXSoundDefinition* GetFXSoundDefinition() const
		{ 
			return static_cast<FXSoundDefinition*>(_fxDef); 
		}

		void Update(float ms);

		void Start();
		void Stop();
		void Pause();

		Media::SoundSource* GetSoundSource() const { return _soundSource; }
		Media::ISoundPlayer* GetSoundPlayer() const { return GetFXSoundDefinition()->GetSoundPlayer(); }
		bool IsDirectional() const { return GetFXSoundDefinition()->IsDirectional(); }
		bool IsSceneSound() const { return GetFXSoundDefinition()->IsSceneSound(); }
	};
}