#include "QuitConfirmDialog.h"
#include <LocalizationManager.h>
#include "MainMenu.h"
#include <LogManager.h>

namespace FlagRTS
{
	QuitConfirmDialog::QuitConfirmDialog(RefPtr<GUI>& gui) :
		_parentGui(gui), _closePending(false), _accepted(false)
	{
		ADD_LOG("Begin Creating QuitConfirmDialog");
		_parentGui->LoadLayout("QuitConfirmDialog.layout", 0, true);
		_loaded = true;

		RefPtr<Layout> layout = _parentGui->GetLayout("QuitConfirmDialog.layout");
		_dialog = static_cast<MyGUI::Window*>(_parentGui->FindInLayout(layout, "DialogWindow"));
		_text =  static_cast<MyGUI::TextBox*>(_parentGui->FindInLayout(layout, "DialogText"));
		_butAccept = static_cast<MyGUI::Button*>(_parentGui->FindInLayout(layout, "ButtonAccept"));
		_butCancel = static_cast<MyGUI::Button*>(_parentGui->FindInLayout(layout, "ButtonCancel"));

		// Set button texts
		_dialog->setCaption("");
		_text->setCaption(GET_TEXT("DialogText","Gui.MainMenu.QuitConfirmDialog"));
		_butAccept->setCaption(GET_TEXT("ButtonAccept","Gui.MainMenu.QuitConfirmDialog"));
		_butCancel->setCaption(GET_TEXT("ButtonCancel","Gui.MainMenu.QuitConfirmDialog"));

		_dialog->setMovable(false);

		// Set event handlers for buttons:
		_butAccept->eventMouseButtonClick += MyGUI::newDelegate(this, &QuitConfirmDialog::Accepted);
		_butCancel->eventMouseButtonClick += MyGUI::newDelegate(this, &QuitConfirmDialog::Canceled);
		ADD_LOG("End Creating QuitConfirmDialog");
	}

	QuitConfirmDialog::~QuitConfirmDialog()
	{
		Unload();
	}

	void QuitConfirmDialog::Accepted(MyGUI::Widget* s)
	{
		_closePending = true;
		_accepted = true;
	}

	void QuitConfirmDialog::Canceled(MyGUI::Widget* s)
	{
		_closePending = true;
		_accepted = false;
	}

	void QuitConfirmDialog::Show()
	{
		_parentGui->ShowLayout("QuitConfirmDialog.layout");
		MyGUI::InputManager::getInstance().addWidgetModal(_dialog);
	}

	void QuitConfirmDialog::Hide()
	{
		_parentGui->HideLayout("QuitConfirmDialog.layout");
		MyGUI::InputManager::getInstance().removeWidgetModal(_dialog);
	}

	void QuitConfirmDialog::Unload()
	{
		if(_loaded)
		{
			ADD_LOG("Unloading QuitConfirmDialog");
			_parentGui->UnloadLayout("QuitConfirmDialog.layout");
		}
	}
}