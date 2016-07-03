#pragma once 

namespace FlagRTS
{
	namespace Media
	{
		class SoundSource;

		// Works similarly to SoundPlayer, but allows multiple Sounds to played
		// but only one at a time like SoundPlayer. Channels unlike SoundPlayers are
		// identified by their id and can be retrived with it from AudioManager
		// Intended for use with sound sets like Gui / Selection etc. where we may have multiple
		// different sounds but want only one/few to be played at one time
		// Every channel may have additional gain set ( which is multiplied by SoundSource gain )
		class SoundChannel
		{
		protected:
			SoundSource* _currentSource;
			int _id;
			float _gain;

		public:
			SoundChannel(int id);

			int GetId() const { return _id; }
			
			float GetGain() const { return _gain; }
			void SetGain(float value);

			SoundSource* GetSoundSource() const { return _currentSource; }

			bool IsPlaying() const;

			// Plays sound on this channel
			// If stopCurrent = false and chanel is busy it wont play
			// If stopCurrent = true it will stop current sound
			void Play(SoundSource* ssource, bool stopCurrent = false);
			void Pause(SoundSource* ssource);
			void Stop(SoundSource* ssource);

			// Some standard channels -> created with AudioManager, so they ids are ensured
			enum StdChannels : int
			{
				CH_Gui = 0, // Channel used for gui sounds (buttons hovers, clicks)
				CH_Alerts, // Channel used for event/messages alters
				CH_SelectionAcknowledge1, // Sound channel used when unit is selected for unit acknowledgement after command is issued
				CH_SelectionAcknowledge2,
				CH_Dialog1, // 2 Channels for cutscene's dialogs
				CH_Dialog2,

				CH_StdChannelsCount,
			};
		};
	}
}