#pragma once

#include <GUIManager.h>

namespace FlagRTS
{
	class QuitConfirmDialog
	{
	private:
		RefPtr<GUI> _parentGui;
		MyGUI::Window* _dialog;
		MyGUI::TextBox* _text;
		MyGUI::Button* _butAccept;
		MyGUI::Button* _butCancel;
		bool _loaded; // True if gui is loaded
		bool _closePending;
		bool _accepted;

	public:
		// In ctor loads layout to 'parentGui'
		// 'parentMenu' is used to for callback when quit is accepted/canceled
		QuitConfirmDialog(RefPtr<GUI>& parentGui);
		~QuitConfirmDialog();

		void Show();
		void Hide();
		void Unload();

		bool IsClosePending() const { return _closePending; }
		bool IsAccepted() const { return _accepted; }

	private:
		// Callback for accept button -> quit confirmed
		// Calls QuitConfirmed(true) from main menu
		void Accepted(MyGUI::Widget*);
		// Callback for cancel button -> quit canceled
		// Calls QuitConfirmed(false) from main menu
		void Canceled(MyGUI::Widget*);
	};
}