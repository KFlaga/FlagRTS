#pragma once

#include "DataTypes.h"
#include <OgreQuaternion.h>
#include <OgreVector3.h>

#include "ObjectTypes.h"

namespace FlagRTS
{
	// Class containg info on spawn properties for scene objects
	// Can be used to store objects to spawn on map creation
	class SpawnInfo
	{
	private:
		Quaternion _orientation; // Inital orientation ( relative to parent ( absolute for independent objs ) )
		Vector3 _position; // Inital position ( relative to parent ( absolute for independent objs ) )
		bool _spawnOnTerrain; // If true will use position.y = terrainHeight(x,z)

	public:
		SpawnInfo() { }

		SpawnInfo(const Quaternion& spawnOrientation, const Vector3& spawnPosition, 
			bool spawnOnTerrain = true) :
			_orientation(spawnOrientation),
			_position(spawnPosition),
			_spawnOnTerrain(spawnOnTerrain)
		{ }

		const Quaternion& GetSpawnOrientation() const { return _orientation; }
		void SetSpawnOrentation(const Quaternion& spawnOrientation) { _orientation = spawnOrientation; }

		const Vector3& GetSpawnPosition() const { return _position; }
		void SetSpawnPosition(const Vector3& spawnPos) { _position = spawnPos; }

		bool GetSpawnOnTerrain() const { return _spawnOnTerrain; }
		void SetSpawnOnTerrain(bool value) { _spawnOnTerrain = value; }
	};
}