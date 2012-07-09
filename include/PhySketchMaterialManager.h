#ifndef PhySketchMaterialManager_h__
#define PhySketchMaterialManager_h__

#include "PhySketchDefinitions.h"
#include "PhySketchSingleton.h"
#include "PhySketchMaterial.h"


namespace PhySketch
{
	class MaterialManager : public Singleton<MaterialManager>
	{	
	public:
		typedef std::map<std::string, Material*> NamedMaterialMap;

	public:
		MaterialManager();
		virtual ~MaterialManager();

		Material* createMaterial(std::string name, Color c);
//		Material* createMaterial(std::string name, std::string texture);

		virtual Material* getDefaultMaterial() const;

		static MaterialManager* getSingletonPtr(void);
		static MaterialManager& getSingleton(void);
	protected:
		Material* _defaultMaterial;
		NamedMaterialMap _materials;
		
	};
}
#endif // PhySketchMaterialManager_h__