#pragma once

#include "DataTypes.h"
#include <OgreTexture.h>
#include <Array.h>

namespace FlagRTS
{
	// Loads images and creates Ogre textures from them
	class Tileset
	{
		Array<string> _textureNames; // Array of names of textures
		Array<uint16> _textureSizes; // Array of sizes of textures
		Array<Ogre::TexturePtr> _loadedTextures; // Array of loaded textures
		string _resourceGroup;
		string _tilesetName;

	public:
		// Takes name of tileset images ( not neccessarly same as name
		// of tileset itself ) to load images ( convention 'name'_'num' )
		// and number of textures in tileset
		// By default looks for images in resource group "Tilesets"
		// Textures must be loaded via call to LoadTileset() prior to use
		Tileset(const string& tilesetName, const string& texNames, uint8 textureCount, uint16 textureSize = 256,
			const string& extension ="png", const string& resourceGroup = "Tilesets");

		// Creates empty tileset - textures are to be added manually
		// (useful when dynamicaly changing tilesets or textures are named
		// not in convention )
		Tileset(const string& tilesetName, const string& resourceGroup = "Tilesets");

		~Tileset() { UnloadTileset(); }

		// Loads all previously added textures
		// Assumes that no textures are loaded
		void LoadTileset();

		// Adds texture to tileset. Must be later loaded to use
		void AddTexture(const string& name, uint16 texSize);

		// Adds texture to tileset. Must be later loaded to use
		void RemoveTexture(const string& name);

		// Unload all textures. To refresh textures (eg after adding one) unload and load again
		void UnloadTileset();

		// Add all textures with names : 'name'_'num' 
		void AddTexturesWithConvention(const string& name, uint8 textureCount, uint16 textureSize, const string& extension);

		Ogre::TexturePtr GetTextureAt(uint8 idx)
		{
			return _loadedTextures[idx];
		}

		Ogre::TexturePtr operator[](uint8 idx)
		{
			return _loadedTextures[idx];
		}

		uint16 GetTextureSize(uint8 idx)
		{
			return _textureSizes[idx];
		}

		// Returns count of textures that are currently loaded
		uint8 GetLoadedTexturesCount() const { return (uint8)_loadedTextures.size(); }

		// Returns count of added textures ( not neccessarly loaded )
		uint8 GetAllTexturesCount() const { return (uint8)_textureNames.size(); }

		void SetResourceGroup(const string& group) { _resourceGroup = group; }
		const string& GetResourceGroup() const { return _resourceGroup; }
	};
}