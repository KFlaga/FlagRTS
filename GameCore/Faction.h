#pragma once

#include "DataTypes.h"
#include "Technology.h"
#include <HashMap.h>
#include <Xml.h>

namespace FlagRTS
{
	class Faction
	{
		HashedMaps<TechRequirement*>::KeyCCString _factionTechRequirements;
		Array<TechBuilding*> _factionBuildings;
		TechRequirementFactory* _reqsFactory;

		string _factionName;
		string _cursorContext;
		string _guiLayout;
		string _guiSoundPack;
		string _noticesSoundPack;

	public:
		Faction();
		~Faction();

		void LoadFromXml(XmlNode* factionNode);

		const string& GetName() const { return _factionName; }
		void SetName(const string& name);

		const string& GetCursorContext() const { return _cursorContext; }
		void SetCursorContext(const string& cursors);

		const string& GetGuiLayout() const { return _guiLayout; }
		void SetGuiLayout(const string& layout);

		const string& GetGuiSoundPack() const { return _guiSoundPack; }
		void SetGuiSoundPack(const string& pack);

		const string& GetNoticesSoundPack() const { return _noticesSoundPack; }
		void SetNoticesSoundPack(const string& pack);

		Array<TechBuilding*>& GetTechBuildings() { return _factionBuildings; }
		const Array<TechBuilding*>& GetTechBuildings() const { return _factionBuildings; }

		HashedMaps<TechRequirement*>::KeyCCString& GetTechRequirements() { return _factionTechRequirements; }
		const HashedMaps<TechRequirement*>::KeyCCString& GetTechRequirements() const { return _factionTechRequirements; }

	private:
		void LoadTechBuilding(XmlNode* buildingNode);
		void LoadUnitRequirements(XmlNode* reqsNode, Array<TechRequirement*>& reqsArray);
		void LoadTechRequirement(XmlNode* reqNode);
	};

}