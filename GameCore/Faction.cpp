#pragma once

#include "Faction.h"
#include "TechRequirement.h"
#include "GameWorld.h"
#include "IObjectDefinitionManager.h"
#include "ConstructionManager.h"
#include "Unit.h"
#include <StringUtils.h>

namespace FlagRTS
{
	Faction::Faction() :
		_reqsFactory( xNew0( TechRequirementFactory ) )
	{

	}

	Faction::~Faction()
	{
		xDelete(_reqsFactory);

		for(auto reqIt = _factionTechRequirements.begin(); reqIt != _factionTechRequirements.end(); ++reqIt)
		{
			xDelete( reqIt->Value );
		}

		for(auto builidngIt = _factionBuildings.begin(); builidngIt != _factionBuildings.end(); ++builidngIt)
		{
			xDelete( *builidngIt );
		}
	}

	void Faction::LoadFromXml(XmlNode* factionNode)		
	{
		const char* name = XmlUtility::XmlGetString(factionNode, "name");
		SetName(name);

		const char* cursors = XmlUtility::XmlGetStringIfExists(factionNode->first_node("CursorContext"), "name", "Cursor_Game_Normal");
		SetCursorContext(cursors);

		const char* layout = XmlUtility::XmlGetStringIfExists(factionNode->first_node("GuiLayout"), "name", "GameGui");
		SetGuiLayout(layout);

		const char* guiSounds = XmlUtility::XmlGetStringIfExists(factionNode->first_node("GuiSounds"), "pack", "Gui");
		SetGuiSoundPack(guiSounds);

		const char* noticesSounds = XmlUtility::XmlGetStringIfExists(factionNode->first_node("NoticeSounds"), "pack", "GameDefaults");
		SetNoticesSoundPack(noticesSounds);

		// Load technology:
		XmlNode* techsNode = factionNode->first_node("Technology");
		// Load requirements
		XmlNode* techReqsNode = techsNode->first_node("TechRequirements");
		XmlNode* reqNode = techReqsNode->first_node("Requirement");
		while( reqNode != 0 )
		{
			LoadTechRequirement(reqNode);
			reqNode = reqNode->next_sibling();
		}

		// Load buildings ( need Requirements to be loaded )
		XmlNode* buildingsNode = techsNode->first_node("Buildings");
		XmlNode* buildingNode = buildingsNode->first_node("Building");
		while( buildingNode != 0 )
		{
			LoadTechBuilding(buildingNode);
			buildingNode = buildingNode->next_sibling();
		}

	}

	void Faction::SetName(const string& name)		
	{
		_factionName = name;
	}

	void Faction::SetCursorContext(const string& cursors)
	{
		_cursorContext = cursors;
	}

	void Faction::SetGuiLayout(const string& layout)
	{
		_guiLayout = layout;
	}

	void Faction::SetGuiSoundPack(const string& pack)
	{
		_guiSoundPack = pack;
	}

	void Faction::SetNoticesSoundPack(const string& pack)
	{
		_noticesSoundPack = pack;
	}

	void Faction::LoadTechBuilding(XmlNode* buildingNode)
	{
		string defName =  XmlUtility::XmlGetString(buildingNode, "definition");
		string familyName =  XmlUtility::XmlGetString(buildingNode, "family");
		string plannerName = XmlUtility::XmlGetStringIfExists(buildingNode, "planner", "Default");

		TechBuilding* building(xNew0(TechBuilding));

		auto createTechBuilding = [this, defName, familyName, plannerName, building](IObjectDefinitionManager* mgr) 
		{	
			UnitDefinition* buildDef = static_cast<UnitDefinition*>(mgr->
				GetObjectDefinitionByName("Unit", defName));
			TechBuildingFamily family = ParseBuildingFamily(familyName.c_str());
			ConstructionPlanner* planner = GameWorld::GlobalWorld->GetConstructionManager()->
				FindConstructionPlanner(plannerName.c_str());
			
			building->Building = buildDef;
			building->Family = family;
			building->UsedPlanner = planner;
		};
		typedef DelegateEventHandler<decltype(createTechBuilding), IObjectDefinitionManager*> DefinitionsLoadedHandler;		
		GameInterfaces::GetObjectDefinitionManager()->OnAllDefinitionsLoaded() +=
			xNew1(DefinitionsLoadedHandler, createTechBuilding);

		_factionBuildings.push_back(building);

		// Load requirements
		XmlNode* reqNode = buildingNode->first_node("Requirements");

		LoadUnitRequirements(reqNode, building->Requirements);
	}

	void Faction::LoadUnitRequirements(XmlNode* reqsNode, Array<TechRequirement*>& reqsArray)
	{
		XmlNode* reqNode = reqsNode->first_node("Requirement");
		while( reqNode != 0 )
		{
			// Get requirement name and find it in reqs map
			string reqName = XmlUtility::XmlGetString(reqNode, "name");
			auto reqIt = _factionTechRequirements.find(reqName.c_str());
			if( reqIt != _factionTechRequirements.end() )
			{
				reqsArray.push_back( reqIt->Value );
			}

			reqNode = reqNode->next_sibling();
		}
	}

	void Faction::LoadTechRequirement(XmlNode* reqNode)
	{
		const char* reqName = XmlUtility::XmlGetString(reqNode, "name");

		TechRequirement* req = (TechRequirement*)_reqsFactory->Create(reqNode);
		_factionTechRequirements.insert(CopyChar(reqName), req);
	}
}