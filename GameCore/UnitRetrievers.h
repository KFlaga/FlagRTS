#pragma once

#include "ISceneObjectRetriever.h"
#include "Unit.h"

namespace FlagRTS
{
	class IUnitRetriever : 
		public IObjectRetriever<Unit*>
	{
	public:
		operator SceneObject*() const 
		{ 
			return static_cast<SceneObject*>(operator Unit*()); 
		}

		operator PhysicalObject*() const 
		{ 
			return static_cast<PhysicalObject*>(operator Unit*()); 
		}

		virtual IUnitRetriever* GetCopyUR() = 0;

		IObjectRetriever<Unit*>* IObjectRetriever<Unit*>::GetCopy() 
		{ 
			return static_cast<IObjectRetriever<Unit*>*>(GetCopyUR()); 
		}
	};

	class UnitRetrieverFactory
	{
	public:
		static IUnitRetriever* Create(XmlNode* soNode);
	};

	class UnitVariableObjectRetriever : public IUnitRetriever
	{
		string _varName;

	public:
		UnitVariableObjectRetriever(const string& varName) :
			_varName(varName)
		{ }

		operator Unit*() const;
		
		IUnitRetriever* GetCopyUR()
		{
			return xNew1(UnitVariableObjectRetriever, _varName);
		}
	};

	class UnitByHandleRetriever : public IUnitRetriever
	{
	private:
		ObjectHandle _handle;

	public:
		UnitByHandleRetriever(ObjectHandle handle) :
			_handle(handle)
		{ }

		operator Unit*() const;
		
		IUnitRetriever* GetCopyUR()
		{
			return xNew1(UnitByHandleRetriever, _handle);
		}
	};

	class UnitLastCreatedRetriever : public IUnitRetriever
	{
	public:
		operator Unit*() const;
		
		IUnitRetriever* GetCopyUR()
		{
			return xNew0(UnitLastCreatedRetriever);
		}
	};

	class UnitNewFromDefinitionRetriever : public IUnitRetriever
	{
	private:
		string _defName;
		IObjectRetriever<int>* _owner;

	public:
		UnitNewFromDefinitionRetriever(const string& defName, IObjectRetriever<int>* owner) :
			_defName(defName),
			_owner(owner)
		{ }

		operator Unit*() const;
		
		IUnitRetriever* GetCopyUR()
		{
			return xNew2(UnitNewFromDefinitionRetriever, _defName, _owner->GetCopy());
		}
	};
	
	// Returns unit that intersects with given point ( or nearest for multihit )
	class UnitFromPositionRetriever : public IUnitRetriever
	{
	private:
		IObjectRetriever<Vector3>* _position;

	public:
		UnitFromPositionRetriever(IObjectRetriever<Vector3>* position) :
			_position(position)
		{ }

		operator Unit*() const;
		
		IUnitRetriever* GetCopyUR()
		{
			return xNew1(UnitFromPositionRetriever, _position);
		}
	};
}