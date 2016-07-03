#include "TransformAnimation.h"
#include <Exception.h>
#include "SceneObject.h"

namespace FlagRTS
{

	TransformAnimation::TransformAnimation(
		TransformAnimationDefinition* taDef, SceneObject* owner) :
	Animation(taDef),
		_owner(owner)
	{
		_animLength = taDef->GetLength();
	}

	TransformAnimation::~TransformAnimation()
	{

	}

	void TransformAnimation::Start()
	{
		_animProgress = 0.0f;
		_currentKeyFrame = 0;
		_currentMs = 0.f;
	}

	void TransformAnimation::End()
	{
		
	}

	void TransformAnimation::Update(float ms)
	{
		if( _animProgress >= 1.0f && _loop == false )
		{
			return;
		}

		_animProgress += ms / _animLength;

		Vector3 finalTranslation(0.0f,0.0f,0.0f);
		Quaternion finalRotation = Quaternion::IDENTITY;

		// Check if next keyframe time is > currentMs + ms
		while(_currentKeyFrame < (int)GetKeyFrames().size())
		{
			const KeyFrame& curFrame = GetKeyFrames()[_currentKeyFrame];
			
			if( curFrame.Length < _currentMs + ms)
			{
				// We moved beyond keyframe, so first save transformation from this keyframe
				float timeDiff = curFrame.Length - _currentMs;
				ms -= timeDiff;
				_currentMs = 0.f;
				float ratio = timeDiff / curFrame.Length;

				finalTranslation += ratio * curFrame.Translation;
				Quaternion thisRotation;
				thisRotation.FromAngleAxis( ratio * curFrame.Angle, curFrame.Axis);
				finalRotation = thisRotation * finalRotation;

				_currentKeyFrame += 1;
			}
			else
			{
				float timeDiff = ms;
				_currentMs += ms;
				ms = -1.f;
				float ratio = timeDiff / curFrame.Length;

				finalTranslation += ratio * curFrame.Translation;
				Quaternion thisRotation;
				thisRotation.FromAngleAxis( ratio * curFrame.Angle, curFrame.Axis);
				finalRotation = thisRotation * finalRotation;
				break;
			}
		}
		if(ms > 0.f)
		{
			// Animation finished, but we still have some time, so check if its looped
			if(_loop == true)
			{
				_currentKeyFrame = 0;
				_currentMs = 0.0f;
				_animProgress = 0.0f;
				
				_owner->MoveQuiet(finalTranslation, Ogre::Node::TS_LOCAL);
				_owner->RotateQuiet(finalRotation, Ogre::Node::TS_LOCAL);

				// Restart animation
				Update(ms);
			}
		}

		// Apply transformation
		_owner->Move(finalTranslation, Ogre::Node::TS_LOCAL);
		_owner->Rotate(finalRotation, Ogre::Node::TS_LOCAL);
	}

	void TransformAnimation::SetLoop(bool value)
	{
		_loop = value;
	}
}