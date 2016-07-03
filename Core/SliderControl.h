#pragma once

#include "TypeDefs.h"
#include "Event.h"
#include <MyGUI\MyGUI.h>

namespace FlagRTS
{
	// Works like slider control -> value from range is set when 
	// mouse slides over it
	// Uses 3 images for display : one is for background, one is for foreground and
	// one indicates current value ( middle texture )
	// Middle texture is partially hidden to fit current value ( it is shrinked and
	// texture coords are set, so that image is not scaled unless set so )
	// Foreground may be used for displaying values / bars
	// Background ImageBox is parent widget, but foreground is used for value sliding
	// that is min range is when mouse is dragged on left edge of foreground ImageBox
	class Slider3ImagesControl
	{
	protected:
		MyGUI::ImageBox* _background;
		MyGUI::ImageBox* _foreground;
		MyGUI::ISubWidgetRect* _valueImageSkin;
		MyGUI::ImageBox* _valueImageWidget;
		float _maxValueWidth;
		float _rangeMax;
		float _rangeMin;
		float _rangeCurrent;
		float _ratioCurrent;
		bool _scaleValueImage;
		bool _allowLeftDrag;
		bool _allowRightDrag;

		Event<Slider3ImagesControl*, float, float> _valueChanged;

	public:
		// Creates slider from existing controls, probably loaded
		// from layout. Reposition them to have common center but doesn't scale them
		Slider3ImagesControl(MyGUI::ImageBox* background, 
			MyGUI::ImageBox* foreground,
			MyGUI::ImageBox* valueImage, 
			float min = 0.f, float max = 1.f, 
			bool scaleValueImage = false);
		
		// Creates slider by creating background with given coords then
		// creates foreground and value-image as childs of that widget
		// and sets images for them
		// All 3 widget have same size and position initially
		Slider3ImagesControl(const MyGUI::IntCoord& backgroundCoords, 
			const string& layerName,
			const string& backgroundImage,
			const string& foregroundImage,
			const string& valueImage, 
			float min = 0.f, float max = 1.f, 
			bool scaleValueImage = false);
		
		// Creates slider using existing ImageBox for background
		// creates foreground and value-image as childs of that widget
		// and sets images for them
		// All 3 widget have same size and position initially
		Slider3ImagesControl(MyGUI::ImageBox* background, 
			const string& backgroundImage,
			const string& foregroundImage,
			const string& valueImage, 
			float min = 0.f, float max = 1.f, 
			bool scaleValueImage = false);

		MyGUI::ImageBox* GetBackgroundImage() const { return _background; }
		MyGUI::ImageBox* GetForegroundImage() const { return _foreground; }
		MyGUI::ImageBox* GetValueImage() const { return _valueImageWidget; }

		void SetRange(float min, float max);
		float GetMin() const { return _rangeMin; }
		float GetMax() const { return _rangeMax; }
		
		// Sets current value, changes slider position but fires event
		void SetCurrentValue(float val);
		// Sets current value, changes slider position but doesn't fire event
		void SetCurrentValueSilient(float val);
		float GetCurentValue() const { return _rangeCurrent; }

		// If true value image texture will be scaled with ImageBox
		// Otherwise texture coords will be set so that texture have always base size
		void SetScaleValueImage(bool value);
		bool GetScaleValueImage() const { return _scaleValueImage; }

		void Show();
		void Hide();
		bool IsVisible() const;

		// Scales widgets, maintaing their centers
		void Resize(float horizontalRatio, float verticalRatio);

		// Enables/disables value change on left-button drag
		void SetAllowLeftDrag(bool value) { _allowLeftDrag = value; }
		bool GetAllowLeftDrag() const { return _allowLeftDrag; }
		// Enables/disables value change on right-button drag
		void SetAllowRightDrag(bool value) { _allowRightDrag = value; }
		bool GetAllowRightDrag() const { return _allowRightDrag; }

		// Event fired when current value is changed
		// 1st param is this slider, 2nd param is old value, 3rd param is new value
		Event<Slider3ImagesControl*, float, float>& GetValueChangedEvent()
		{
			return _valueChanged;
		}

	private:
		void MouseDragged(MyGUI::Widget*, int left, int top);
		void UpdateValue();
	};
	
	// Works like slider control -> value from range is set when 
	// mouse slides over it
	// Uses 2 images for display : one is for background, one is for foreground
	// Current value is indicated by silder arrow which is a button and may be dragged
	// Foreground may be used for displaying values / bars
	// Background ImageBox is parent widget
	class Slider2ImagesArrowControl
	{
	protected:
		MyGUI::ImageBox* _background;
		MyGUI::ImageBox* _foreground;
		MyGUI::Button* _valueSlider;
		float _rangeMax;
		float _rangeMin;
		float _rangeCurrent;

	public:
	};
}