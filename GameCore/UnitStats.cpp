#include "UnitStats.h"
#include <StringUtils.h>

namespace FlagRTS
{
	UnitStats::UnitStats() :
		HitPoints(1),
		MaxHitPoints(1),
		MoveSpeed(0.f),
		RotateSpeed(0.f),
		BuildTime(10000.f)
	{	}

	UnitStats::UnitStats(UnitStats* stats) :
		HitPoints(stats->HitPoints),
		MaxHitPoints(stats->MaxHitPoints),
		MoveSpeed(stats->MoveSpeed),
		RotateSpeed(stats->RotateSpeed),
		BuildTime(stats->BuildTime)
	{ 
		Cost.reserve(stats->Cost.size());
		for(unsigned int i = 0; i < stats->Cost.size(); ++i)
		{
			ResourceCost cost;
			cost.ResAmount = stats->Cost[i].ResAmount;
			cost.ResName = CopyChar(stats->Cost[i].ResName);
			cost.ReturnOnDeath = stats->Cost[i].ReturnOnDeath;
			Cost.push_back(cost);
		}
	}

	UnitStats::UnitStats(const UnitStats& stats):
		HitPoints(stats.HitPoints),
		MaxHitPoints(stats.MaxHitPoints),
		MoveSpeed(stats.MoveSpeed),
		RotateSpeed(stats.RotateSpeed),
		BuildTime(stats.BuildTime)
	{
		Cost.reserve(stats.Cost.size());
		for(unsigned int i = 0; i < stats.Cost.size(); ++i)
		{
			ResourceCost cost;
			cost.ResAmount = stats.Cost[i].ResAmount;
			cost.ResName = CopyChar(stats.Cost[i].ResName);
			cost.ReturnOnDeath = stats.Cost[i].ReturnOnDeath;
			Cost.push_back(cost);
		}
	}

	UnitStats::~UnitStats()
	{
		for(unsigned int i = 0; i < Cost.size(); ++i)
		{
			sDeleteArray(Cost[i].ResName);
		}
	}

	void UnitStats::Parse(XmlNode* statsNode)
	{
		HitPoints = XmlUtility::XmlGetInt(statsNode->first_node("Life", 4), "value", 5);
		MaxHitPoints = HitPoints;
		MoveSpeed = (float)XmlUtility::XmlGetInt(statsNode->first_node("MoveSpeed", 9), "value", 5) / 1000.0f;
		RotateSpeed = (float)XmlUtility::XmlGetInt(statsNode->first_node("RotateSpeed", 11), "value", 5) / 1000.0f;

		XmlNode* costNode = statsNode->first_node("Cost",4);
		BuildTime = (float)XmlUtility::XmlGetInt(costNode->first_node("Time", 4), "value", 5);

		XmlNode* resNode = costNode->first_node("Resource", 8);
		while(resNode != 0)
		{
			ResourceCost cost;
			cost.ResAmount = XmlUtility::XmlGetInt(resNode, "value", 5);
			cost.ResName = CopyChar(XmlUtility::XmlGetString(resNode, "name", 4));
			cost.ReturnOnDeath = XmlUtility::XmlGetBool(resNode, "return", 6);
			Cost.push_back(cost);

			resNode = resNode->next_sibling();
		}
	}
}