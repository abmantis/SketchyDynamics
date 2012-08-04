#include "PhySketchMaterialManager.h"
#include <FreeImage.h>
#include "PhySketchLogger.h"

namespace PhySketch
{
	template<> MaterialManager* Singleton<MaterialManager>::ms_Singleton = 0;

	MaterialManager::MaterialManager()
	{
		_defaultMaterial = createMaterial("defaultMaterial", Color(0.0f, 0.0f, 0.0f, 1.0f));
		_blackMaterial = createMaterial("blackMaterial", Color(0.0f, 0.0f, 0.0f, 1.0f));
		_whiteMaterial = createMaterial("whiteMaterial", Color(1.0f, 1.0f, 1.0f, 1.0f));
	}

	MaterialManager::~MaterialManager()
	{
		NamedMaterialMap::iterator it = _materials.begin();
		NamedMaterialMap::iterator it_end = _materials.end();
		for(; it != it_end; it++)
		{			
			glDeleteTextures(1, &it->second->_textureID);
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
		PHYSKETCH_ASSERT(insresult.second && "A Material with the same name already exists.");			

		return m;
	}

	Material* MaterialManager::createMaterial( std::string name, std::string texture, bool generateMipMaps /*= false*/ )
	{
		FREE_IMAGE_FORMAT fif = FIF_UNKNOWN; 
		FIBITMAP* dib(0); 

		fif = FreeImage_GetFileType(texture.c_str(), 0); // Check the file signature and deduce its format

		if(fif == FIF_UNKNOWN) // If still unknown, try to guess the file format from the file extension
		{
			fif = FreeImage_GetFIFFromFilename(texture.c_str()); 
		}

		if(fif == FIF_UNKNOWN) // If still unkown, return failure
		{	
			return nullptr; 
		}

		if(FreeImage_FIFSupportsReading(fif)) // Check if the plugin has reading capabilities and load the file
		{
			dib = FreeImage_Load(fif, texture.c_str()); 
		}

		if(!dib) 
		{
			PHYSKETCH_LOG_ERROR("Texture not fount: " + texture);
			return nullptr; 			
		}

		BYTE* bDataPointer = FreeImage_GetBits(dib); // Retrieve the image data

		int width = FreeImage_GetWidth(dib); // Get the image width and height
		int height = FreeImage_GetHeight(dib); 
		int bpp = FreeImage_GetBPP(dib); 

		// If somehow one of these failed (they shouldn't), return failure
		if(bDataPointer == NULL || width == 0 || height == 0) 
		{
			return nullptr; 
		}

		// The texture is loaded, create a new Material

		Material* m = new Material(name);
		
		std::pair<NamedMaterialMap::iterator, bool> insresult = 
			_materials.insert(NamedMaterialMap::value_type(name, m));
		PHYSKETCH_ASSERT(insresult.second && "A Material with the same name already exists.");

		
		int format = bpp == 32 ? GL_BGRA : bpp == 24 ? GL_BGR : bpp == 8 ? GL_LUMINANCE : 0; 
		//int internalFormat = bpp == 32 ? GL_RGBA : bpp == 24 ? GL_RGB : GL_DEPTH_COMPONENT;  
		int internalFormat = bpp == 32 ? GL_COMPRESSED_RGBA_S3TC_DXT5_EXT : bpp == 24 ? GL_COMPRESSED_RGB_S3TC_DXT1_EXT : GL_DEPTH_COMPONENT;  
		// TODO: check if the DXT5 and DXT1 extensions are available 

		glGenTextures(1, &m->_textureID);
		glBindTexture(GL_TEXTURE_2D, m->_textureID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_REPEAT);
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, bDataPointer);
		
		if(generateMipMaps)
		{
			glGenerateMipmap(GL_TEXTURE_2D); 
		}

		FreeImage_Unload(dib); 
		
		return m;
	}

	Material* MaterialManager::getDefaultMaterial() const
	{
		return _defaultMaterial;
	}

	Material* MaterialManager::getWhiteMaterial() const
	{
		return _whiteMaterial;
	}

	Material* MaterialManager::getBlackMaterial() const
	{
		return _blackMaterial;
	}

}


