#pragma once

#include "IGameObjectPool.h"
#include "ObjectDefinitionsCreator.h"
#include <TypeId.h>
#include <ArrayMap.h>

namespace FlagRTS
{
	// Stores all GameObjects created in game
	// All objects are should also be created via it
	class MainGameObjectPool : public IGameObjectPool
	{
	public:
		static MainGameObjectPool* GlobalPool;

	protected:
		ObjectDefinitionsCreator _defCreator;
		ArrayMap<TypeId, IGameObjectPool*> _objPools;

	public:
		MainGameObjectPool();
		~MainGameObjectPool();

		void LoadAllDefinitions(XmlNode* defNode, const string& modName)
		{
			_defCreator.LoadAllObjectsDefinitions(defNode, modName);
		}

		void LoadAllResources(Ogre::SceneManager*);
		void UnloadAllResources(Ogre::SceneManager*);
		void UpdateAll(float ms);

		ObjectDefinition* GetObjectDefinitionByHandle(ObjectHandle handle) 
		{ 
			return _defCreator.GetObjectDefinitionByHandle(handle); 
		}

		template<typename DefType>
		DefType* GetObjectDefinitionByHandleCast(ObjectHandle handle) 
		{ 
			return _defCreator.GetObjectDefinitionByHandleCast<DefType>(handle); 
		}

		ObjectDefinition* GetObjectDefinitionByName(const char* name, const char* defType) 
		{ 
			return _defCreator.GetObjectDefinitionByName(name, defType);
		}

		template<typename DefType>
		DefType* GetObjectDefinitionByNameCast(const char* name, const char* defType) 
		{ 
			return _defCreator.GetObjectDefinitionByNameCast<DefType>(name, defType); 
		}

		ObjectDefinition* GetObjectDefinitionByName(const string& name, const string& defType) 
		{ 
			return _defCreator.GetObjectDefinitionByName(name, defType);
		}

		template<typename DefType>
		DefType* GetObjectDefinitionByNameCast(const string& name, const string& defType) 
		{ 
			return _defCreator.GetObjectDefinitionByNameCast<DefType>(name, defType); 
		}

		// Event fired when all definition parsing is finished
		// Event owns all added handlers, they are xDeleted after firing
		Event<>& OnAllDefinitionsLoaded()
		{
			return _defCreator.OnAllDefinitionsLoaded();
		}
		
		IGameObject* FindByHandle(ObjectHandle handle);

		IGameObject* FindByHandle(TypeId objectType, ObjectHandle handle)
		{
			return _objPools.find(objectType)->Value->FindByHandle(handle);
		}

		IGameObject* GetLastCreatedObject(TypeId ofType)
		{
			return _objPools.find(ofType)->Value->GetLastCreatedObject();
		}

		IGameObject* Create(ObjectDefinition* objDef, int owner);
		
		IGameObject* Create(const string& defName, const string& typeName, int owner)
		{
			return Create(GetObjectDefinitionByName(defName, typeName), owner);
		}

		IGameObject* Create(ObjectHandle handleToDefinition, int owner)
		{
			return Create(GetObjectDefinitionByHandle(handleToDefinition), owner);
		}

		void Destroy(IGameObject* object);

		void DestroyAll();

		IGameObjectPool* GetObjectPool(TypeId ofType)
		{
			return _objPools.find(ofType)->Value;
		}

		template<typename ObjType, typename DefType>
		IGameObjectPoolCast<ObjType, DefType>* GetObjectPoolCast(TypeId ofType)
		{
			return static_cast<IGameObjectPoolCast<ObjType, DefType>*>(_objPools.find(ofType)->Value);
		}

		ObjectDefinitionsCreator* GetDefinitionsCreator() { return &_defCreator; }
	};
}