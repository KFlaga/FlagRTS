#pragma once

#include "GameObjectPool.h"
#include "IObjectStateSupplier.h"
#include <ArrayMap.h>

namespace FlagRTS
{
	template<typename ObjType, typename DefType>
	class SceneObjectDefaultPool : public GameObjectPool<ObjType, DefType>
	{
	protected:
		ArrayMaps<IObjectStateSupplier*>::KeyCCString _stateSuppliers;

	public:
		SceneObjectDefaultPool() { }
		virtual ~SceneObjectDefaultPool()
		{
			for(auto it = _stateSuppliers.begin(); it != _stateSuppliers.end(); ++it)
			{
				xDelete(it->Value);
			}
		}

		void LoadAllResources(Ogre::SceneManager* ogre)
		{
			for(auto objIt = _objects.begin(); _objects.isNotEnd(objIt); ++objIt)
			{
				objIt->Value->LoadResources(ogre);
			}
		}

		void UnloadAllResources(Ogre::SceneManager* ogre)
		{
			for(auto objIt = _objects.begin(); _objects.isNotEnd(objIt); ++objIt)
			{
				objIt->Value->UnloadResources(ogre);
			}
		}

		void UpdateAll(float ms)
		{
			for(auto objIt = _objects.begin(); _objects.isNotEnd(objIt); ++objIt)
			{
				objIt->Value->Update(ms);
			}
		}

		void DestroyAll()
		{
			_objects.clear();
		}

		void RegisterObjectStateSupplier(const char* objectClassName, 
			IObjectStateSupplier* supplier)
		{
			auto suppIt = _stateSuppliers.find(objectClassName);
			if(suppIt != _stateSuppliers.end())
				xDelete(suppIt->Value);
			_stateSuppliers[objectClassName] = supplier;
		}

		void UnregisterObjectStateSupplier(const char* objectClassName)
		{
			_stateSuppliers.erase(objectClassName);
		}

		void AddStatesToObject(ObjType* object, DefType* definition)
		{
			if(definition->GetObjectClasses().size() == 0)
			{
				_stateSuppliers.find("Classless")->Value->AddStatesToObject(object);
			}
			else
			{
				for(auto classNameIt = definition->GetObjectClasses().begin();
					classNameIt != definition->GetObjectClasses().end(); ++classNameIt)
				{
					auto suppIt = _stateSuppliers.find(*classNameIt);
					if(suppIt != _stateSuppliers.end())
					{
						suppIt->Value->AddStatesToObject(object);
					}
				}
			}
		}
	};
}