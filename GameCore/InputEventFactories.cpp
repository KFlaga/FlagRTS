#include "InputEventFactories.h"
#include <LogManager.h>
#include <SystemSettings.h>
#include <KeyBindings.h>

namespace ORTS
{
	InputEventFactory::InputEventFactory() :
		IFactoryCast("InputEvent")
	{
		_eventFactories.insert(std::make_pair(
			"KeyPressed",
			new KeyPressedEventFactory()));

		_eventFactories.insert(std::make_pair(
			"KeyReleased",
			new KeyReleasedEventFactory()));

		_eventFactories.insert(std::make_pair(
			"MousePressed",
			new MousePressedEventFactory()));

		_eventFactories.insert(std::make_pair(
			"MouseReleased",
			new MouseReleasedEventFactory()));
	}

	InputEventFactory::~InputEventFactory()
	{
		for(auto facIt = _eventFactories.begin(); facIt != _eventFactories.end(); ++facIt)
		{
			delete facIt->second;
		}
		_eventFactories.clear();
	}

	IGameObject* InputEventFactory::Create(XmlNode* eventNode)
	{
		InputEvent* ievent = 0;

		try
		{
			// First find higher level type of object and corretponding factory
			char* typeName = XmlUtility::XmlGetString(eventNode, "type", 4, false);
			auto subFactory = _eventFactories.find(typeName);
			if(subFactory != _eventFactories.end()) // If subtype factory found
			{
				// Delegate creation of final object to subtype factory
				ievent = (InputEvent*)subFactory->second->Create(eventNode);
			}
		}
		catch(...) 
		{
			ievent = 0;
			if(eventNode != 0)
				ADD_LOG("Failed to parse input event definition node");
			else
				ADD_LOG("Passed nullptr to InputEvent factory");
		}

		return ievent;
	}

	KeyPressedEventFactory::KeyPressedEventFactory() :
		IFactoryCast("KeyPressedEvent")
	{ }

	KeyPressedEventFactory::~KeyPressedEventFactory() { }

	IGameObject* KeyPressedEventFactory::Create(XmlNode* eventNode)
	{
		KeyPressedEvent* keyevent = 0;

		// KeyPressed event to be valid must have key attribute set
		try
		{
			char* keyName = XmlUtility::XmlGetString(eventNode, "key", 3, false);
			int keyCode = SystemSettings::GetKeyBindings().KeyCodeNamesMap.find(keyName)->second;
			// modifiers are optional
			//char* modName = XmlUtility::XmlGetString(eventNode, "modifier", 8, false);
			//int modCode = SystemSettings::GetKeyBindings().KeyCodeNamesMap.find(keyName)->second;
			int modCode = 0;
			keyevent = new KeyPressedEvent(keyCode, modCode);
		}
		catch(...)
		{
			keyevent = 0;
			ADD_LOG("Failed to create key pressed event");
		}

		return keyevent;
	}

	KeyReleasedEventFactory::KeyReleasedEventFactory() :
		IFactoryCast("KeyReleasedEvent")
	{ }

	KeyReleasedEventFactory::~KeyReleasedEventFactory() { }

	IGameObject* KeyReleasedEventFactory::Create(XmlNode* eventNode)
	{
		KeyReleasedEvent* keyevent = 0;

		// KeyPressed event to be valid must have key attribute set
		try
		{
			char* keyName = XmlUtility::XmlGetString(eventNode, "key", 3, false);
			int keyCode = SystemSettings::GetKeyBindings().KeyCodeNamesMap.find(keyName)->second;
			// modifiers are optional
			//char* modName = XmlUtility::XmlGetString(eventNode, "modifier", 8, false);
			//int modCode = SystemSettings::GetKeyBindings().KeyCodeNamesMap.find(keyName)->second;
			int modCode = 0;
			keyevent = new KeyReleasedEvent(keyCode, modCode);
		}
		catch(...)
		{
			keyevent = 0;
			ADD_LOG("Failed to create key released event");
		}

		return keyevent;
	}

	MousePressedEventFactory::MousePressedEventFactory() :
		IFactoryCast("MousePressedEvent")
	{ }

	MousePressedEventFactory::~MousePressedEventFactory() { }

	IGameObject* MousePressedEventFactory::Create(XmlNode* eventNode)
	{
		MousePressedEvent* mouseEvent = 0;

		// KeyPressed event to be valid must have key attribute set
		try
		{
			int butCode = XmlUtility::XmlGetInt(eventNode, "button", 6, false);
			// modifiers are optional
			//char* modName = XmlUtility::XmlGetString(eventNode, "modifier", 8, false);
			//int modCode = SystemSettings::GetKeyBindings().KeyCodeNamesMap.find(keyName)->second;
			int modCode = 0;
			mouseEvent = new MousePressedEvent(butCode, modCode);
		}
		catch(...)
		{
			mouseEvent = 0;
			ADD_LOG("Failed to create mouse pressed event");
		}

		return mouseEvent;
	}

	MouseReleasedEventFactory::MouseReleasedEventFactory() :
		IFactoryCast("MouseReleasedEvent")
	{ }

	MouseReleasedEventFactory::~MouseReleasedEventFactory() { }

	IGameObject* MouseReleasedEventFactory::Create(XmlNode* eventNode)
	{
		MouseReleasedEvent* mouseEvent = 0;

		// KeyPressed event to be valid must have key attribute set
		try
		{
			int butCode = XmlUtility::XmlGetInt(eventNode, "button", 6, false);
			// modifiers are optional
			//char* modName = XmlUtility::XmlGetString(eventNode, "modifier", 8, false);
			//int modCode = SystemSettings::GetKeyBindings().KeyCodeNamesMap.find(keyName)->second;
			int modCode = 0;
			mouseEvent = new MouseReleasedEvent(butCode, modCode);
		}
		catch(...)
		{
			mouseEvent = 0;
			ADD_LOG("Failed to create mouse released event");
		}

		return mouseEvent;
	}
}