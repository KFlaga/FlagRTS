#include "SliderControl.h"

namespace FlagRTS
{
	Slider3ImagesControl::Slider3ImagesControl(MyGUI::ImageBox* background, 
		MyGUI::ImageBox* foreground,
		MyGUI::ImageBox* valueImage, 
		float min, float max, bool scaleValueImage) :
	_rangeMin(min),
		_rangeMax(max),
		_scaleValueImage(scaleValueImage),
		_rangeCurrent(0.f),
		_allowLeftDrag(true),
		_allowRightDrag(true),
		_ratioCurrent(0.f)
	{
		_background = background;
		_foreground = foreground;
		_valueImageWidget = valueImage;
		_valueImageSkin = _valueImageWidget->getSubWidgetMain(); 
		_maxValueWidth = (float)_valueImageWidget->getWidth();
		// Reposition for common center
		int centerX = _background->getLeft() + _background->getWidth() / 2;
		int centerY = _background->getTop() + _background->getHeight() / 2;
		_foreground->setPosition(centerX + _foreground->getWidth() / 2,
			centerY + _foreground->getHeight() / 2);
		_valueImageWidget->setPosition(centerX + _valueImageWidget->getWidth() / 2,
			centerY + _valueImageWidget->getHeight() / 2);

		UpdateValue();
		_foreground->eventMouseDrag += MyGUI::newDelegate(this, &Slider3ImagesControl::MouseDragged);
	}

	Slider3ImagesControl::Slider3ImagesControl(const MyGUI::IntCoord& coords, 
		const string& layer,
		const string& backgroundImage,
		const string& foregroundImage,
		const string& valueImage, 
		float min, float max, bool scaleValueImage) :
	_rangeMin(min),
		_rangeMax(max),
		_scaleValueImage(scaleValueImage),
		_rangeCurrent(0.f),
		_allowLeftDrag(true),
		_allowRightDrag(true)
	{
		// Create bg widget
		_background = MyGUI::Gui::getInstance().createWidget<MyGUI::ImageBox>("ImageBox", coords.left, coords.top,
			coords.width, coords.height, MyGUI::Align::Default, layer);
		_valueImageWidget = _background->createWidgetReal<MyGUI::ImageBox>("ImageBox",
			0.f, 0.f, 1.f, 1.f, MyGUI::Align::Default);
		_foreground = _background->createWidgetReal<MyGUI::ImageBox>("ImageBox",
			0.f, 0.f, 1.f, 1.f, MyGUI::Align::Default);

		_background->setImageTexture(backgroundImage);
		_valueImageWidget->setImageTexture(foregroundImage);
		_foreground->setImageTexture(valueImage);

		_valueImageSkin = _valueImageWidget->getSubWidgetMain(); 
		_maxValueWidth = (float)_valueImageWidget->getWidth();

		UpdateValue();
		_foreground->eventMouseDrag += MyGUI::newDelegate(this, &Slider3ImagesControl::MouseDragged);
	}

	Slider3ImagesControl::Slider3ImagesControl(MyGUI::ImageBox* background, 
		const string& backgroundImage,
		const string& foregroundImage,
		const string& valueImage, 
		float min, float max, bool scaleValueImage) :
	_rangeMin(min),
		_rangeMax(max),
		_scaleValueImage(scaleValueImage),
		_rangeCurrent(0.f),
		_allowLeftDrag(true),
		_allowRightDrag(true)
	{
		_valueImageWidget = _background->createWidgetReal<MyGUI::ImageBox>("ImageBox",
			0.f, 0.f, 1.f, 1.f, MyGUI::Align::Default);
		_foreground = _background->createWidgetReal<MyGUI::ImageBox>("ImageBox",
			0.f, 0.f, 1.f, 1.f, MyGUI::Align::Default);

		_background->setImageTexture(backgroundImage);
		_valueImageWidget->setImageTexture(foregroundImage);
		_foreground->setImageTexture(valueImage);

		_valueImageSkin = _valueImageWidget->getSubWidgetMain(); 
		_maxValueWidth = (float)_valueImageWidget->getWidth();

		UpdateValue();
		_foreground->eventMouseDrag += MyGUI::newDelegate(this, &Slider3ImagesControl::MouseDragged);
	}


	void Slider3ImagesControl::SetRange(float min, float max)
	{
		_rangeMax = max;
		_rangeMax = min;
		UpdateValue();
	}

	void Slider3ImagesControl::SetCurrentValue(float val)
	{
		float oldVal = _rangeCurrent;
		_rangeCurrent = val;
		UpdateValue();
		_valueChanged.Fire(this, oldVal, val);
	}

	void Slider3ImagesControl::SetCurrentValueSilient(float val)
	{
		_rangeCurrent = val;
		UpdateValue();
	}

	void Slider3ImagesControl::SetScaleValueImage(bool value)
	{
		if( value != _scaleValueImage )
		{
			_scaleValueImage = value;
			UpdateValue();
		}
	}

	void Slider3ImagesControl::UpdateValue()
	{
		_ratioCurrent = std::min(0.f, std::max(1.f, (_rangeCurrent - _rangeMin) / (_rangeMax - _rangeMin)));
		_valueImageWidget->setSize((int)(_ratioCurrent*_maxValueWidth), _valueImageWidget->getHeight());
		if( _scaleValueImage == false )
		{
			_valueImageSkin->_setUVSet(MyGUI::FloatRect(
				0.f, 0.f, _ratioCurrent, 1.f));
		}
		else
		{
			_valueImageSkin->_setUVSet(MyGUI::FloatRect(
				0.f, 0.f, 1.f, 1.f));
		}
	}

	void Slider3ImagesControl::Show()
	{
		_background->setVisible(true);
		_foreground->setVisible(true);
		_valueImageWidget->setVisible(true);
	}

	void Slider3ImagesControl::Hide()
	{
		_background->setVisible(false);
		_foreground->setVisible(false);
		_valueImageWidget->setVisible(false);
	}

	bool Slider3ImagesControl::IsVisible() const
	{
		return _background->getVisible();
	}

	void Slider3ImagesControl::Resize(float horizontalRatio, float verticalRatio)
	{
		auto oldCoords = _background->getCoord();
		_background->setCoord((float)oldCoords.left * horizontalRatio,
			(float)oldCoords.top * verticalRatio,
			(float)oldCoords.width * horizontalRatio,
			(float)oldCoords.height * verticalRatio);

		oldCoords = _foreground->getCoord();
		_foreground->setCoord((float)oldCoords.left * horizontalRatio,
			(float)oldCoords.top * verticalRatio,
			(float)oldCoords.width * horizontalRatio,
			(float)oldCoords.height * verticalRatio);

		oldCoords = _valueImageWidget->getCoord();
		_valueImageWidget->setCoord((float)oldCoords.left * horizontalRatio,
			(float)oldCoords.top * verticalRatio,
			(float)oldCoords.width * horizontalRatio,
			(float)oldCoords.height * verticalRatio);
	}

	void Slider3ImagesControl::MouseDragged(MyGUI::Widget*, int left, int top)
	{
		int widgetX = left - _foreground->getAbsoluteLeft();
		float relX = (float)widgetX / (float)_foreground->getWidth();

		SetCurrentValue(_rangeMin + relX * (_rangeMax - _rangeMin));
		UpdateValue();
	}
}