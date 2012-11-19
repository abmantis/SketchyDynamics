#ifndef SketchyDynamicsMaterialManager_h__
#define SketchyDynamicsMaterialManager_h__

#include "SketchyDynamicsDefinitions.h"
#include "SketchyDynamicsSingleton.h"
#include "SketchyDynamicsMaterial.h"


namespace SketchyDynamics
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
#endif // SketchyDynamicsMaterialManager_h__