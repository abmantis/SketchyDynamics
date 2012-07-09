#include "PhySketchMaterialManager.h"

namespace PhySketch
{
	template<> MaterialManager* Singleton<MaterialManager>::ms_Singleton = 0;

	MaterialManager::MaterialManager()
	{
		_defaultMaterial = createMaterial("blackMaterial", Color(0.0f, 0.0f, 0.0f, 0.0f));
	}

	MaterialManager::~MaterialManager()
	{
		NamedMaterialMap::iterator it = _materials.begin();
		NamedMaterialMap::iterator it_end = _materials.end();
		for(; it != it_end; it++)
		{			
			delete it->second;
			it->second = nullptr;
		}

		_materials.clear();
	}

	MaterialManager* MaterialManager::getSingletonPtr( void )
	{
		PHYSKETCH_ASSERT(ms_Singleton != NULL);
		return ms_Singleton;
	}

	MaterialManager& MaterialManager::getSingleton( void )
	{
		PHYSKETCH_ASSERT(ms_Singleton != NULL);
		return *ms_Singleton;
	}

	Material* MaterialManager::createMaterial(std::string name, Color c )
	{
		Material* m = new Material(name);
		m->_color = c;

		std::pair<NamedMaterialMap::iterator, bool> insresult = 
			_materials.insert(NamedMaterialMap::value_type(name, m));
		PHYSKETCH_ASSERT(insresult.second && "A Polygon with the same name already exists in the renderer.");			

		return m;
	}

// 	Material* MaterialManager::createMaterial( std::string name, std::string texture )
// 	{
// 
// 	}

	Material* MaterialManager::getDefaultMaterial() const
	{
		return _defaultMaterial;
	}

	



}


