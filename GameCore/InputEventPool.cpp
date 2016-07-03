#include "InputEventPool.h"
#include "Trigger.h"
#include "InputEvents.h"
#include <InputManager.h>

namespace FlagRTS
{
	InputEventPool::InputEventPool() :
		IEventPool("Input")
	{
		_availableEvents["KeyPressed"] = xNew0(KeyPressedEvent);
		_availableEvents["KeyReleased"] = xNew0(KeyReleasedEvent);
		_availableEvents["MousePressed"] = xNew0(MousePressedEvent);
		_availableEvents["MouseReleased"] = xNew0(MouseReleasedEvent);
		_availableEvents["MouseClicked"] = xNew0(MouseClickedEvent);
		_availableEvents["MouseDoubleClicked"] = xNew0(MouseDoubleClickedEvent);
	}

	InputEventPool::~InputEventPool()
	{
		for(auto eIt = _availableEvents.begin(); eIt != _availableEvents.end(); ++eIt)
		{
			xDelete(eIt->second);
		}
	}

	IEvent* InputEventPool::FindEvent(const string& typeName)
	{
		auto eIt = _availableEvents.find(typeName);
		if( eIt != _availableEvents.end() )
		{
			return eIt->second;
		}
		return 0;
	}

	void InputEventPool::Update(float ms)
	{
		// Check if any key state changed ( if such is registered )
		KeyPressedEvent* kpEvent = static_cast<KeyPressedEvent*>(_availableEvents["KeyPressed"]);
		if( kpEvent->GetActionsCount() > 0 )
		{
			for(unsigned char c = 0; c < 255; ++c)
			{
				if(InputManager::Instance()->CheckKeyWasPressed(c))
				{
					kpEvent->Fire((int)c);
				}
			}
		}

		KeyReleasedEvent* krEvent = static_cast<KeyReleasedEvent*>(_availableEvents["KeyReleased"]);
		if( krEvent->GetActionsCount() > 0 )
		{
			for(unsigned char c = 0; c < 255; ++c)
			{
				if(InputManager::Instance()->CheckKeyWasReleased(c))
				{
					krEvent->Fire((int)c);
				}
			}
		}

		MousePressedEvent* mpEvent = static_cast<MousePressedEvent*>(_availableEvents["MousePressed"]);
		if( mpEvent->GetActionsCount() > 0 )
		{
			for(unsigned char mb = 0; mb < 3; ++mb)
			{
				if(InputManager::Instance()->CheckMouseWasPressed(mb))
				{
					mpEvent->Fire((int)mb);
				}
			}
		}

		MouseReleasedEvent* mrEvent = static_cast<MouseReleasedEvent*>(_availableEvents["MouseReleased"]);
		if( mrEvent->GetActionsCount() > 0 )
		{
			for(unsigned char mb = 0; mb < 3; ++mb)
			{
				if(InputManager::Instance()->CheckMouseWasReleased(mb))
				{
					mrEvent->Fire((int)mb);
				}
			}
		}

		MouseClickedEvent* mcEvent = static_cast<MouseClickedEvent*>(_availableEvents["MouseClicked"]);
		if( mcEvent->GetActionsCount() > 0 )
		{
			for(unsigned char mb = 0; mb < 3; ++mb)
			{
				if(InputManager::Instance()->CheckMouseWasClicked(mb))
				{
					mcEvent->Fire((int)mb);
				}
			}
		}

		MouseDoubleClickedEvent* mc2Event = static_cast<MouseDoubleClickedEvent*>(_availableEvents["MouseDoubleClicked"]);
		if( mc2Event->GetActionsCount() > 0 )
		{
			for(unsigned char mb = 0; mb < 3; ++mb)
			{
				if(InputManager::Instance()->CheckMouseWasDoubleClicked(mb))
				{
					mc2Event->Fire((int)mb);
				}
			}
		}
	}
}