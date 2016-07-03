#pragma once

#include "TypeDefs.h"

namespace FlagRTS
{
	// Codes for varius errors, registered in ErrorManager, mainly for Core project
	// or general ones. For other parts other enums deriving from ErrorCode can be defined 
	enum ErrorCodes : ErrorCode
	{
		// Basic errors
		Success = 0,
		Unknown = 1,
		Exit = -1, // Indicates that app requested exit ( so its not bad )
		FileNotFound = 2,
		ResourceNotFound,
		ResourceNotLoaded,
		ResourceAlreadyLoaded,
		ResourceAlreadyAdded,
		FailedToLoad,
		FailedToSave,
		FailedToOpenFile,
		BadCast,
		Nullptr,

		// GUI errors
		LayoutNotFound,
		LayoutNotLoaded,
		LayoutAlreadyLoaded,
		LayoutAdded,

		// GUIManager errors
		GuiNotFound,
		GuiAlreadyRegistered,
	
		// RenderSystem errors
		RenderSystemNotFound,
		RenderSystemFailedToLoadOptions,
	
		// Config errors
		ConfigFileCoundntOpen,
		ConfigSectionCoundntFind,

		// AppStates errors
		AppStateNameNotFound,

		// CursorManager errors
		CursorFailedToLoadResources,
		CursorContextBadCast,
		CurserContextNotFound,
		CursorWin32BadCast,
		CursorWin32NotFound,

		// AudioManager errors
		AudioPackAlreadyLoaded,
		AudioPackNotFound,
		AudioPackNotLoaded,
		AudioSoundAlreadyPlaying,
		AudioSoundNotPlaying,
		AudioSoundNotFound,
	};
}