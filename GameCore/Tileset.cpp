#include "Tileset.h"
#include <OgreTextureManager.h>
#include <OgreImage.h>

namespace FlagRTS
{
	Tileset::Tileset(const string& tsName, const string& texName, 
		uint8 textureCount, uint16 texSize, const string& extension,const string& resourceGroup) :
		_resourceGroup(resourceGroup), _tilesetName(tsName)
	{
		AddTexturesWithConvention(texName, textureCount, texSize, extension);
	}

	Tileset::Tileset(const string& tsName, const string& resourceGroup) :
		_resourceGroup(resourceGroup), _tilesetName(tsName)
	{

	}

	void Tileset::LoadTileset()
	{
		_loadedTextures.reserve(GetAllTexturesCount());

		for (int tex = 0; tex < GetAllTexturesCount(); tex++) 
		{
			string num = std::to_string(tex);
			Ogre::Image img;

			img.load(_textureNames[tex], _resourceGroup);
			int textureSize = (int)img.getWidth();

			// create a manual texture	  
			_loadedTextures.push_back(Ogre::TextureManager::getSingleton().createManual(
				_tilesetName + "_" + num, _resourceGroup, Ogre::TEX_TYPE_2D, _textureSizes[tex], _textureSizes[tex], 
				1, img.getFormat()));	
			// load the imagefile 
			_loadedTextures[tex]->loadImage(img);
		}
	}

	void Tileset::AddTexture(const string& name, uint16 texSize)
	{
		_textureNames.push_back(name);
		_textureSizes.push_back(texSize);
	}

	void Tileset::RemoveTexture(const string& name)
	{
		// find texture and remove it ( do not unload it if loaded )
		for(auto it = _textureNames.begin(); it != _textureNames.end(); it++)
		{
			if(it->compare(name) == 0)
			{
				_textureNames.erase(it);
				break;
			}
		}
	}

	void Tileset::UnloadTileset()
	{
		for(auto it = _loadedTextures.begin(); it != _loadedTextures.end(); it++)
		{
			it->setNull();
		}

		_loadedTextures.clear();
	}

	void Tileset::AddTexturesWithConvention(const string& name, uint8 textureCount, uint16 textureSize, const string& extension)
	{
		for(int i = 0; i < textureCount; i++)
		{
			_textureNames.push_back(name + "_" + std::to_string(i) + "." + extension);
			_textureSizes.push_back(textureSize);
		}
	}
}