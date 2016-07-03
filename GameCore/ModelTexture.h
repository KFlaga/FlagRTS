#pragma once

#include <Ogre\OgreTexture.h>
#include <Ogre\OgreColourValue.h>
#include "DataTypes.h"

namespace FlagRTS
{
	class PhysicalObject;

	// Creates texture from given model -> rotates it and casts it 
	// on a plane with optional bg texture via camera
	// 
	class ModelTexture
	{
	protected:
		Ogre::SceneManager* _ogreMgr;
		Ogre::TexturePtr _finalTexture;
		Ogre::TexturePtr _bgTexture;
		Ogre::ColourValue _bgColor;
		Ogre::Camera* _camera;
		Ogre::SceneNode* _objectNode;
		Ogre::SceneNode* _cameraNode;
		Ogre::ManualObject* _bgPlane;
		Ogre::SceneNode* _bgNode;
		Ogre::MaterialPtr _bgMaterial;
		PhysicalObject* _renderObject;
		Vector3 _offset;
		Quaternion _orientation;
		string _name;
		float _ctgFovx;
		int _width;
		int _height;
		bool _useBgTexture;

	public:
		// Creates render texture / camera / camera scene node
		// Texture have size 'width' x 'height'
		// Camera is positioned in 'cameraPosition' and faces -Z
		ModelTexture(Ogre::SceneManager* ogreMgr, const string& name, int width, int height, 
			const Vector3& cameraPosition);

		// Doesnt create render texture, only camera and scene nodes
		ModelTexture(Ogre::SceneManager* ogreMgr, const string& name, const Vector3& cameraPosition);

		~ModelTexture();


		void UpdateTexture();

		// Sets new render texture, deatches camera from old one, attaches to new one
		void SetRenderTexture(const Ogre::TexturePtr& texture, int width, int height);
		const Ogre::TexturePtr& GetRenderTexture() const { return _finalTexture; }

		// Assumes object is created and spawned
		// Places object in starndard position, so that its fully visible on screen
		// assuming orientation is identity
		void SetRenderObject(PhysicalObject* object);
		PhysicalObject* GetRenderObject() const { return _renderObject; }

		int GetWidth() const { return _width; }
		int GetHeight() const { return _height; }

		void SetBackgroundColor(const Ogre::ColourValue& color);
		const Ogre::ColourValue& GetBackgroundColor() const { return _bgColor; }

		void SetBackgroundTexture(const Ogre::TexturePtr& texture);
		const Ogre::TexturePtr& GetBackgroundTexture() const { return _bgTexture; }

		void SetUseBackgroundTexture(bool value);
		bool GetUseBackgroundTexture() const { return _useBgTexture; }

		// Sets position of object relative to camera
		// by default when setting new object, its position is set to minimal for
		// which entire object is visible
		// Becouse camera is facing -Z, so of object needs to be further from camera
		// its Z position should be lower
		void SetPosition(const Vector3& pos);
		const Vector3& GetPosition() { return _offset; }
		
		// Sets orientation of object
		void SetOrientation(const Quaternion& q);
		const Quaternion& GetOrientation() { return _orientation; }

		void UpdateBackgroundPlanePosition();

	protected:
		void CreateBackground();
	};
}