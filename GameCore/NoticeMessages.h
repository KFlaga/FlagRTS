#pragma once

#include "DataTypes.h"

namespace FlagRTS
{
	// Notice types may have specific colors/prefixes/sounds
	namespace Notices
	{
		enum NoticeType : int
		{
			Unknown = 0,
			GameInfo = 1, // Info about in-game events
			AppInfo, // Info about app state
			GameError, // In-game error -> invalid user action
			AppError, // Application error
			NoticeTypeCount
		};

		extern const Vector3* NoticeTypeColors;
	}
	typedef Notices::NoticeType NoticeType;

	struct NoticeMessage
	{
		NoticeType Type;
		const char* Message;
		uint8 Player; // Which player actions its about
		// Sound??

		NoticeMessage() :
			Type(Notices::Unknown),
			Message(0)
		{ }

		NoticeMessage(NoticeType type, const char* msg, uint8 player) :
			Type(type),
			Message(msg),
			Player(player)
		{ }
	};
}