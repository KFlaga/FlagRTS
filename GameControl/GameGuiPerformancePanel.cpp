#include "GameGuiPerformancePanel.h"

#include <MyGUI\MyGUI.h>

#include <RenderSystemManager.h>
#include <Memory.h>
#include <Exception.h>

namespace FlagRTS
{
	using namespace GameGui;

	PerformancePanel::PerformancePanel(MyGUI::Widget* parent) :
		_perfBox(0),
		_worstFps(100.0f),
		_lastFps(100.0f)
	{
		if(parent != 0)
		{
			_perfBox = parent->createWidget<MyGUI::ListBox>("ListBox",
				0, 0, 100, 100, MyGUI::Align::Right, "PerformancePanel");
		}
		else
		{
			_perfBox = MyGUI::Gui::getInstancePtr()->createWidget<MyGUI::ListBox>("ListBox",
				0, 0, 100, 100, MyGUI::Align::Right, "PerformancePanel");
		}
		_perfBox->setVisible(false);
		_perfBox->setInheritsAlpha(false);
		_perfBox->setAlpha(1.0f);

		_perfBox->addItem("CurFPS", 0);
		_perfBox->addItem("WorstFPS", 0);
		_perfBox->addItem("Batch", 0);
		_perfBox->addItem("Triangles", 0);
	}

	PerformancePanel::~PerformancePanel()
	{
		
	}

	void PerformancePanel::Update(float ms)
	{
		if(_perfBox->isVisible())
		{
			auto window = RenderSystemManager::Instance()->GetMainWindow();

			float fps = (1000.0f/ms);

			if(fps < _worstFps)
				_worstFps = fps;
			else
				_worstFps = _worstFps*0.999f + fps*0.001f;

			_lastFps = _lastFps*0.75f + fps*0.25f;
			
			_perfBox->setItemNameAt(0, string("FPS: ") + std::to_string((int)_lastFps));
			_perfBox->setItemNameAt(1, string("WorstFPS: ") + std::to_string((int)_worstFps));
			_perfBox->setItemNameAt(2, string("Batches: ") + std::to_string(window->getBatchCount()));
			_perfBox->setItemNameAt(3, string("Triangles: ") + std::to_string(window->getTriangleCount()));
		}
	}

	void PerformancePanel::SetVisible(bool value)
	{
		_perfBox->setVisible(value);
	}

	bool PerformancePanel::IsVisible() const
	{
		return _perfBox->getVisible();
	}
}