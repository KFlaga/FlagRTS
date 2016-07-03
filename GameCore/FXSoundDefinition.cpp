#include "FXSoundDefinition.h"
#include <AudioManager.h>
#include <Exception.h>
#include <StringUtils.h>
#include "FXSound.h"

namespace FlagRTS
{
	FXSoundDefinition::FXSoundDefinition() :
		FXObjectDefinition(),
		_soundPlayer(0),
		_soundName(0),
		_soundPack(0),
		_gain(1.f),
		_halfVolumeDistance(100.f),
		_maxAttDistance(1000.f),
		_rolloffFactor(1.f),
		_innerConeAngle(360.f),
		_outerConeAngle(360.f),
		_outerConeGain(0.f),
		_positionOffset(0.f,0.f,0.f),
		_orientationOffset(1.f,0.f,0.f,0.f),
		_isSoundDirectional(false),
		_isSoundSceneSound(false)
	{
		SetFinalType(GetTypeId<FXSound>());
		SetFinalTypeName("FXSound");
	}

	FXSoundDefinition::FXSoundDefinition(XmlNode* node) :
		FXObjectDefinition(node),
		_soundPlayer(0)
	{
		SetFinalType(GetTypeId<FXSound>());
		SetFinalTypeName("FXSound");
		// Get node specific to FXSound
		XmlNode* sfxNode = node->first_node("FXObject")->first_node("FXSound");
		if(sfxNode != 0)
		{
			XmlNode* soundNode = sfxNode->first_node("Sound");
			_soundName = CopyChar(XmlUtility::XmlGetString(soundNode, "name"));
			_soundPack = CopyChar(XmlUtility::XmlGetString(soundNode, "pack"));

			_gain = XmlUtility::XmlGetFloatIfExists(sfxNode->first_node("Gain"), "value", 1.f);
			_halfVolumeDistance = XmlUtility::XmlGetFloatIfExists(sfxNode->first_node("HalfDistance"), "value", 100.f);
			_maxAttDistance = XmlUtility::XmlGetFloatIfExists(sfxNode->first_node("MaxDistance"), "value", 1000.f);
			_rolloffFactor = XmlUtility::XmlGetFloatIfExists(sfxNode->first_node("Rolloff"), "value", 1.f);
			_innerConeAngle = XmlUtility::XmlGetFloatIfExists(sfxNode->first_node("InnerConeAngle"), "value", 360.f);
			_outerConeAngle = XmlUtility::XmlGetFloatIfExists(sfxNode->first_node("OuterConeAngle"), "value", 360.f);
			_outerConeGain = XmlUtility::XmlGetFloatIfExists(sfxNode->first_node("OuterConeGain"), "value", 0.f);

			_positionOffset = XmlUtility::XmlGetXYZIfExists(sfxNode->first_node("Position"), Vector3(0.f,0.f,0.f));
			_orientationOffset = XmlUtility::XmlGetQuaternionIfExists(sfxNode->first_node("Orientation"), Quaternion(1.f,0.f,0.f,0.f));
			_orientationOffset.normalise();
			_isSoundDirectional = XmlUtility::XmlGetBoolIfExists(sfxNode->first_node("IsDirectional"), "value", false);
			_isSoundSceneSound = XmlUtility::XmlGetBoolIfExists(sfxNode->first_node("IsSceneSound"), "value", false);
		
			_soundPlayer = Media::AudioManager::Instance()->GetSoundPlayer(
				_soundName, _soundPack);
		}
	}

	FXSoundDefinition::~FXSoundDefinition()
	{
		if(_soundName != 0)
			sDeleteArray(_soundName);
		if(_soundPack != 0)
			sDeleteArray(_soundPack);
	}

	void FXSoundDefinition::SetSoundName(const char* name)
	{
		if(_soundName != 0)
			sDeleteArray(_soundName);
		_soundName = CopyChar(name);
	}

	void FXSoundDefinition::SetSoundPack(const char* pack)
	{
		if(_soundPack != 0)
			sDeleteArray(_soundPack);
		_soundPack = CopyChar(pack);
	}

	void FXSoundDefinition::UpdateSoundPlayer()
	{
		_soundPlayer = Media::AudioManager::Instance()->GetSoundPlayer(
				_soundName, _soundPack);
	}
}