#include "SceneMarkerDefinition.h"
#include "AnimationDefinitionFactory.h"
#include <Exception.h>
#include "SceneObjectState.h"

namespace FlagRTS
{
	SceneMarkerDefinition::SceneMarkerDefinition() :
		SceneObjectDefinition(),
		_animations(0),
		_isInteractable(false),
		_isTimed(false)
	{

	}

	SceneMarkerDefinition::SceneMarkerDefinition(XmlNode* node) :
		SceneObjectDefinition(node),
		_isInteractable(false),
		_isTimed(false),
		_animations(xNew0(AnimationDefinitionMap))
	{
		// Get node specific to SM
		XmlNode* smNode = node->first_node("SceneMarker");
		if(smNode != 0)
		{
			XmlNode* animsNode = smNode->first_node("Animations",10);
			if(animsNode != 0)
				ParseAnimations(animsNode);

			_isInteractable = XmlUtility::XmlGetFlag(smNode, "IsInteractable");
			_isTimed = XmlUtility::XmlGetFlag(smNode, "IsTimed");
			_timeout = XmlUtility::XmlGetFloatIfExists(smNode->first_node("Timeout"), "value");
		}
	}

	SceneMarkerDefinition::~SceneMarkerDefinition()
	{
		if(_animations != 0)
		{
			for(auto asetIt = _animations->begin(), end = _animations->end();
				asetIt != end; ++asetIt)
			{
				for(auto animIt = asetIt->second->begin(), end = asetIt->second->end();
					animIt != end; ++animIt)
				{
					xDelete(animIt->Value);
				}
				xDelete(asetIt->second);
			}
			xDelete(_animations);
		}
	}

	void SceneMarkerDefinition::ParseAnimations(XmlNode* animsNode)
	{
		AnimationDefinitionFactory animFactory;

		// Parse all states
		XmlNode* stateNode = animsNode->first_node("State",5);
		while(stateNode != 0)
		{
			const char* stateName = XmlUtility::XmlGetString(stateNode,"name",4);
			size_t stateNum = SceneObjectStates::Unknown;
			// Find state number from string
			stateNum = SceneObjectStates::ParseStateType(stateName);

			if(stateNum == SceneObjectStates::Unknown)
			{
				CastException((string("Not supported state name in animations. Def name = ") +
					GetName() + ". State name = " + stateName).c_str());
			}

			AnimationDefinitionsSet* animsForState(xNew0(AnimationDefinitionsSet));
			// Parse all animation for this state
			XmlNode* animNode = stateNode->first_node("Animation",9);
			while(animNode != 0)
			{
				AnimationDefinition* anim = 0;

				try
				{
					anim = animFactory.CreateCast(animNode);
				}
				catch(std::exception& e)
				{
					CastException((string("Failed to create animation definition. Def name = ") +
						GetName() + ". State name = " + stateName + "Reason: " + e.what()).c_str());
				}

				animsForState->push_back(anim);

				animNode = animNode->next_sibling("Animation",9);
			}

			_animations->insert(std::make_pair(
				stateNum, animsForState));

			stateNode = stateNode->next_sibling("State",5);
		}
	}
}