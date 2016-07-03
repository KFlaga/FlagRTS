#include "GameGuiResourcesPanel.h"
#include <MyGUI\MyGUI.h>

#include <GameWorld.h>
#include <Resources.h>
#include <Player.h>

namespace FlagRTS
{
	using namespace GameGui;

	ResourcesPanel::ResourcesPanel(RefPtr<GUI>& gameGui, RefPtr<Layout>& mainLayout) :
		_isVisible(false)
	{
		_mainPanel = static_cast<MyGUI::ImageBox*>(gameGui->FindInLayout(mainLayout, "ResourcesPanel"));

		// Create resource icons and fields
		// 1) Get number of resources
		_resources = GameWorld::GlobalWorld->GetResources();
		unsigned int resCount = _resources->GetResourcesCount();
		_resourceFieldsIcons.reserve(resCount);
		_resourceFieldsValues.reserve(resCount);

		// 2) For each resource create image box and text box
		for(unsigned int i = 0; i < resCount; ++i)
		{
			MyGUI::ImageBox* resBox = static_cast<MyGUI::ImageBox*>(gameGui->FindInLayout(
				mainLayout, "Resource_" + std::to_string(i)));

			resBox->eventToolTip += MyGUI::newDelegate(this, &ResourcesPanel::ShowTooltip);
			resBox->setUserData(i);
			_resourceFieldsIcons.push_back(resBox);

			MyGUI::TextBox* resText = static_cast<MyGUI::TextBox*>(gameGui->FindInLayout(
				mainLayout, "ResourceText_" + std::to_string(i)));

			resText->eventToolTip += MyGUI::newDelegate(this, &ResourcesPanel::ShowTooltip);
			resText->setUserData(i);
			_resourceFieldsValues.push_back(resText);
		}

		auto bgPanel = static_cast<MyGUI::Widget*>(gameGui->FindInLayout(mainLayout, "MainPanel"));
		_tooltipResource = bgPanel->createWidgetReal<MyGUI::ImageBox>("Panel",
			0.0f, 0.0f, 0.01f, 0.01f, MyGUI::Align::Center, "TooltipResources");
		_tooltipResource->setVisible(false);
		_tooltipResourceText = _tooltipResource->createWidgetReal<MyGUI::TextBox>("TextBox",
			0.0f, 0.0f, 0.01f, 0.01f, MyGUI::Align::Center, "TooltipResourcesText");
		_tooltipResourceText->setVisible(false);
		_tooltipResourceText->setTextAlign(MyGUI::Align::Center);
		_tooltipResourceText->setFontHeight(16);

		_updateTickLength = 200.f;
		_tickTimer = _updateTickLength;
	}

	ResourcesPanel::~ResourcesPanel()
	{

	}

	void ResourcesPanel::Update(float ms)
	{
		_tickTimer -= ms;
		if(_tickTimer <= 0.f)
		{
			int clientPlayer = GameWorld::GlobalWorld->GetPlayers()->GetClientPlayer();
			for(unsigned int i = 0; i < _resourceFieldsIcons.size(); ++i)
			{
				_resourceFieldsValues[i]->setCaption(
					std::to_string(_resources->GetPlayerResourceAmount(clientPlayer, i)) 
					//+ "/" +
					//std::to_string(_resources->GetMaxAmountForPlayer(clientPlayer, i))
					);
			}
			_tickTimer += _updateTickLength;
		}
	}

	void ResourcesPanel::Show()
	{
		_mainPanel->setVisible(true);
		for(unsigned int i = 0; i < _resourceFieldsIcons.size(); ++i)
		{
			_resourceFieldsIcons[i]->setVisible(true);
			_resourceFieldsValues[i]->setVisible(true);
		}
		_isVisible = true;
	}

	void ResourcesPanel::Hide()
	{
		_mainPanel->setVisible(false);
		for(unsigned int i = 0; i < _resourceFieldsIcons.size(); ++i)
		{
			_resourceFieldsIcons[i]->setVisible(false);
			_resourceFieldsValues[i]->setVisible(false);
		}
		_isVisible = false;
	}

	void ResourcesPanel::ShowTooltip(MyGUI::Widget* sender, const MyGUI::ToolTipInfo& info)
	{
		if (info.type == MyGUI::ToolTipInfo::Show)
		{
			unsigned int resNum = *sender->getUserData<unsigned int>(false);
			// Set resource name for now - later may add some statisic data, so dont store tooltips staticaly
			_tooltipResourceText->setCaption(_resources->GetResourceName(resNum));
			// ... fetch the new text size from the tooltip's Edit control...
			const MyGUI::IntSize& textsize = _tooltipResourceText->getTextSize();

			// ... and resize the tooltip panel to match it.  The Stretch property on the Edit
			// control will see to it that the Edit control resizes along with it.
			// The constants are padding to fit in the edges of the PanelSmall skin; adjust as
			// necessary for your theme.
			_tooltipResource->setSize(textsize.width + 16, textsize.height + 8);
			_tooltipResourceText->setSize(textsize.width + 16, textsize.height + 8);
			// You can fade it in smooth if you like, but that gets obnoxious.
			_tooltipResource->setPosition(_resourceFieldsIcons[resNum]->getAbsolutePosition() + 
				MyGUI::IntPoint(0, _resourceFieldsIcons[resNum]->getSize().height));
			_tooltipResourceText->setRealPosition(0.0f,0.0f);
			_tooltipResource->setVisible(true);
			_tooltipResourceText->setVisible(true);
		}
		else if (info.type == MyGUI::ToolTipInfo::Hide)
		{
			_tooltipResource->setVisible(false);
			_tooltipResourceText->setVisible(false);
		}
	}

	void ResourcesPanel::Resize(float ratioHorizontal, float ratioVertical)
	{
		const MyGUI::IntCoord& oldCoord = _mainPanel->getCoord();
		_mainPanel->setCoord(
			(int)((float)oldCoord.left * ratioHorizontal), (int)((float)oldCoord.top * ratioVertical), 
			(int)((float)oldCoord.width * ratioHorizontal), (int)((float)oldCoord.height * ratioVertical));

		for(unsigned int i = 0; i < _resourceFieldsIcons.size(); ++i)
		{
			const MyGUI::IntCoord& oldCoordIcon = _resourceFieldsIcons[i]->getCoord();
			_resourceFieldsIcons[i]->setCoord(
			(int)((float)oldCoordIcon.left * ratioHorizontal), (int)((float)oldCoordIcon.top * ratioVertical), 
			(int)((float)oldCoordIcon.width * ratioHorizontal), (int)((float)oldCoordIcon.height * ratioVertical));

			const MyGUI::IntCoord& oldCoordText = _resourceFieldsValues[i]->getCoord();
			_resourceFieldsValues[i]->setCoord(
			(int)((float)oldCoordText.left * ratioHorizontal), (int)((float)oldCoordText.top * ratioVertical), 
			(int)((float)oldCoordText.width * ratioHorizontal), (int)((float)oldCoordText.height * ratioVertical));

			/*_resourceFieldsIcons[i]->setRealCoord(
				(float)i/ (float)_resourceFieldsIcons.size(), 0.0f, 
				1.f / (float)_resourceFieldsIcons.size(), 1.0f);
			_resourceFieldsValues[i]->setRealCoord(0.3f, 0.1f, 0.6f, 0.9f); */
		}
	}
}