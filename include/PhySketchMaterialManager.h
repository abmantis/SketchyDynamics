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
		Material* createMaterial(std::string name, std::string texture, bool generateMipMaps = false, bool repeat = true);

		virtual Material* getDefaultMaterial() const;
		virtual Material* getWhiteMaterial() const;
		virtual Material* getBlackMaterial() const;

		static MaterialManager* getSingletonPtr(void);
		static MaterialManager& getSingleton(void);
	protected:
		Material* _defaultMaterial;
		Material* _whiteMaterial;
		Material* _blackMaterial;
		NamedMaterialMap _materials;
		
	};
}
#endif // PhySketchMaterialManager_h__