#pragma once

#include "SceneMarkerDefinition.h"
#include "DataTypes.h"

namespace FlagRTS
{
	class BillboardMarkerDefinition : public SceneMarkerDefinition
	{
		string _materialName;
		string _materialGroup;
		int _poolSize;
		int _billboardType;
		int _originType;
		Vector2 _defSize;
		Vector3 _commonUp;
		Vector3 _commonDir;
		bool _autoUpdate;
		bool _isInWorldSpace;
		bool _cullIndividualy;
		bool _useIdentityProjection;
		bool _useIdentityView;

	public:
		// Sets default values
		BillboardMarkerDefinition();
		// Reads values from xml node
		BillboardMarkerDefinition(XmlNode* soDefNode);

		const string& GetMaterialName() const { return _materialName; }
		void GetMaterialName(const string& name) { _materialName = name; }
		
		const string& GetMaterialGroup() const { return _materialGroup; }
		void GetMaterialGroup(const string& group) { _materialGroup = group; }

		int GetBillboardType() const { return _billboardType; }
		void SetBillboardType(int value) { _billboardType = value; }

		int GetBillboardOrigin() const { return _originType; }
		void SetBillboardOrigin(int value) { _originType = value; }

		const Vector2& GetDefaultBillboardSize() const { return _defSize; }
		void SetDefaultBillboardSize(const Vector2& size) { _defSize = size; }

		const Vector3& GetCommonUpVector() const { return _commonUp; }
		void SetCommonUpVector(const Vector3& up) { _commonUp = up; }

		const Vector3& GetCommonDirection() const { return _commonDir; }
		void SetCommonDirection(const Vector3& dir) { _commonDir = dir; }
		
		int GetPoolSize() const { return _poolSize; }
		void SetPoolSize(int size) { _poolSize = size; }

		bool GetBillboardAutoUpdate() const { return _autoUpdate; }
		void SetBillboardAutoUpdate(bool value) { _autoUpdate = value; }
	
		bool GetIsBillboardInWorldSpace() const { return _isInWorldSpace; }
		void SetIsBillboardInWorldSpace(bool value) { _isInWorldSpace = value; }
	
		bool GetCullIndividualy() const { return _cullIndividualy; }
		void SetCullIndividualy(bool value) { _cullIndividualy = value; }

		bool GetUseIdentityProjection() const { return _useIdentityProjection; }
		void SetUseIdentityProjection(bool value) { _useIdentityProjection = value; }

		bool GetUseIdentityView() const { return _useIdentityView; }
		void SetUseIdentityView(bool value) { _useIdentityView = value; }

	protected:
		int ParseBillboardType(const char* typeName);
		int ParseBillboardOriginType(const char* typeName);
	};
}