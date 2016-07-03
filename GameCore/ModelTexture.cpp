#include "ModelTexture.h"
#include "PhysicalObject.h"
#include <OgreSceneManager.h>
#include <OgreTextureManager.h>
#include <OgreHardwarePixelBuffer.h>
#include <OgreManualObject.h>

namespace FlagRTS
{
	ModelTexture::ModelTexture(Ogre::SceneManager* ogreMgr, 
		const string& name, int width, int height, 
		const Vector3& cameraPosition) :
	_ogreMgr(ogreMgr),
		_name(name),
		_width(width),
		_height(height),
		_useBgTexture(false),
		_offset(0.f,0.f,0.f),
		_orientation(1.f,0.f,0.f,0.f),
		_renderObject(0),
		_bgColor(0.f,0.f,0.f,1.f),
		_bgTexture()
	{
		_finalTexture = Ogre::TextureManager::getSingleton().createManual(
			name + "RTT", 
			"Base", 
			Ogre::TEX_TYPE_2D, 
			_width, _height, 0,
			Ogre::PF_R8G8B8, 
			Ogre::TU_RENDERTARGET);

		_camera = ogreMgr->createCamera(name + "Camera");
		_camera->setAspectRatio((float)width / (float)height);
		_camera->setDirection(Vector3::NEGATIVE_UNIT_Z);
		_camera->setFarClipDistance(1000.f);
		_camera->setNearClipDistance(1.f);
		_camera->setFOVy(Radian(Degree(45.f)));
		_ctgFovx = (float)height / (float)width;

		Ogre::RenderTexture* renderTexture = _finalTexture->getBuffer()->getRenderTarget();
		renderTexture->addViewport(_camera);
		renderTexture->getViewport(0)->setClearEveryFrame(true);
		renderTexture->getViewport(0)->setBackgroundColour(_bgColor);
		renderTexture->getViewport(0)->setOverlaysEnabled(false);
		//renderTexture->getViewport(0)->setShadowsEnabled(false);

		_cameraNode = _ogreMgr->getRootSceneNode()->createChildSceneNode(
			name + "CameraNode", cameraPosition);
		_cameraNode->attachObject(_camera);

		_objectNode = _cameraNode->createChildSceneNode(
			name + "ObjectNode", _offset, _orientation);

		CreateBackground();
	}

	ModelTexture::ModelTexture(Ogre::SceneManager* ogreMgr, 
		const string& name, const Vector3& cameraPosition) :
	_ogreMgr(ogreMgr),
		_name(name),
		_width(0),
		_height(0),
		_useBgTexture(false),
		_offset(0.f,0.f,0.f),
		_orientation(1.f,0.f,0.f,0.f),
		_renderObject(0),
		_bgColor(0.f,0.f,0.f,1.f),
		_bgTexture()
	{
		_camera = ogreMgr->createCamera(name + "Camera");
		_camera->setDirection(Vector3::NEGATIVE_UNIT_Z);
		_camera->setFarClipDistance(1000.f);
		_camera->setNearClipDistance(1.f);
		_camera->setFOVy(Radian(Degree(45.f)));

		_cameraNode = _ogreMgr->getRootSceneNode()->createChildSceneNode(
			name + "CameraNode", cameraPosition);
		_cameraNode->attachObject(_camera);

		_objectNode = _cameraNode->createChildSceneNode(
			name + "ObjectNode", _offset, _orientation);
		
		CreateBackground();
	}

	ModelTexture::~ModelTexture()
	{
		if(_renderObject != 0)
		{
			_renderObject->Despawn();
			_renderObject->SetSceneNode(0);
			_objectNode->detachAllObjects();
		}
		_ogreMgr->destroySceneNode(_objectNode);
		_cameraNode->detachAllObjects();

		_bgNode->detachAllObjects();
		_ogreMgr->destroySceneNode(_bgNode);
		_ogreMgr->destroyManualObject(_bgPlane);

		_ogreMgr->destroySceneNode(_cameraNode);

		Ogre::RenderTexture* renderTexture = _finalTexture->getBuffer()->getRenderTarget();
		renderTexture->removeAllViewports();
		_ogreMgr->destroyCamera(_camera);
	}

	void ModelTexture::CreateBackground()
	{
		_bgNode = _cameraNode->createChildSceneNode(
			_name + "BgNode", _offset + Vector3(0.f,0.f,-100.f));

		// Create material 
		Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().
			create(_name + "BgMaterial", "Base");

		material->setReceiveShadows(false);
		material->getTechnique(0)->setLightingEnabled(false);

		_bgPlane = _ogreMgr->createManualObject();
		_bgPlane->estimateVertexCount(4);
		_bgPlane->estimateIndexCount(6);
		_bgPlane->begin(_name + "BgMaterial");
		_bgPlane->position(-50.f,-50.f,0.f);
		_bgPlane->textureCoord(0.f,0.f);
		_bgPlane->position(-50.f,50.f,0.f);
		_bgPlane->textureCoord(0.f,1.f);
		_bgPlane->position(50.f,50.f,0.f);
		_bgPlane->textureCoord(1.f,1.f);
		_bgPlane->position(50.f,-50.f,0.f);
		_bgPlane->textureCoord(1.f,0.f);
		_bgPlane->quad(0,1,2,3);
		_bgPlane->end();
	}

	void ModelTexture::SetRenderTexture(const Ogre::TexturePtr& texture, int width, int height)
	{
		if(!_finalTexture.isNull())
		{
			Ogre::RenderTexture* renderTexture = _finalTexture->getBuffer()->getRenderTarget();
			renderTexture->removeAllViewports();
		}

		_finalTexture = texture;
		if(_finalTexture.isNull())
		{
			return;
		}

		_camera->setAspectRatio((float)width / (float)height);
		_ctgFovx = (float)height / (float)width;

		Ogre::RenderTexture* renderTexture = _finalTexture->getBuffer()->getRenderTarget();
		renderTexture->addViewport(_camera);
		renderTexture->getViewport(0)->setClearEveryFrame(true);
		renderTexture->getViewport(0)->setBackgroundColour(_bgColor);
		renderTexture->getViewport(0)->setOverlaysEnabled(false);
		//renderTexture->getViewport(0)->setShadowsEnabled(false);
	}

	void ModelTexture::UpdateTexture()
	{


	}

	void ModelTexture::SetRenderObject(PhysicalObject* object)
	{
		if(_renderObject != 0)
		{
			_renderObject->Despawn();
			_renderObject->SetSceneNode(0);
		}
		_renderObject = object;
		if(_renderObject != 0)
		{
			_renderObject->SetSceneNode(_objectNode);
			_renderObject->Spawn();
			// Set proper offset :
			// center object -> off.y = height/2
			auto& bbox = object->GetEntity()->getBoundingBox();
			Vector3 halfSize(bbox.getHalfSize());
			_offset.y = -halfSize.y;
			// make object fully visible -> off.z = max(h/2 * ctg(fovy), w/2 * ctg(fovx)) + size.z/2
			// as fovy = 45deg, ctg(fovy) = 1
			_offset.z = -std::max(halfSize.y, halfSize.x * _ctgFovx) - halfSize.z;
			_objectNode->setPosition(_offset);
		}	
	}

	void ModelTexture::SetBackgroundColor(const Ogre::ColourValue& color)
	{
		_bgColor = color;
		_finalTexture->getBuffer()->getRenderTarget()->
			getViewport(0)->setBackgroundColour(_bgColor);
	}

	void ModelTexture::SetBackgroundTexture(const Ogre::TexturePtr& texture)
	{
		_bgTexture = texture;
		if(_useBgTexture)
		{
			if(_bgTexture.isNull())
			{
				_bgNode->detachAllObjects();
			}
			else
			{
				_bgNode->attachObject(_bgPlane);
				UpdateBackgroundPlanePosition();
			}
		}
	}

	void ModelTexture::SetUseBackgroundTexture(bool value)
	{
		if( _bgTexture.isNull() == false && _useBgTexture != value)
		{
			if(_useBgTexture == false) // Show texture
			{
				_bgNode->attachObject(_bgPlane);
				// Move bg if neccessary to not overlap object
				UpdateBackgroundPlanePosition();

			}
			else // Hide texture
			{
				_bgNode->detachAllObjects();
			}
		}
		_useBgTexture = value;
	}

	void ModelTexture::SetPosition(const Vector3& pos)
	{
		_offset = pos;
		_objectNode->setPosition(pos);
		if(_useBgTexture)
		{
			UpdateBackgroundPlanePosition();
		}
	}

	void ModelTexture::SetOrientation(const Quaternion& q)
	{
		_orientation = q;
		_objectNode->setOrientation(q);
		if(_useBgTexture)
		{
			UpdateBackgroundPlanePosition();
		}
	}

	void ModelTexture::UpdateBackgroundPlanePosition()
	{
		if( _renderObject != 0 )
		{
			auto& bbox = _renderObject->GetEntity()->getBoundingBox();
			Vector3 halfSize(bbox.getHalfSize());
			// Set position so that it wont touch object
			float offsetZ = -std::max(halfSize.x, std::max(halfSize.y, halfSize.z)) + _offset.z;
			_bgNode->setPosition(0.f,0.f,offsetZ);
			// Set scale so that it fills background ( base size is 100x100 )
			_bgNode->setScale((float)_width*std::abs(offsetZ)*0.01f/(float)_height, std::abs(offsetZ) * 0.01f, 1.f);
		}
		else
		{
			_bgNode->setPosition(0.f,0.f,-100.f);
			_bgNode->setScale((float)_width/(float)_height, 1.f, 1.f);
		}
	}
}