#include "PhySketchShader_GL.h"
#include "PhySketchUtils.h"
#include "PhySketchLogger.h"

namespace PhySketch
{
	//////////////////////////////////////////////////////////////////////////
	/// Shader_GL
	//////////////////////////////////////////////////////////////////////////

	Shader_GL::Shader_GL(std::string sFile, int a_iType)
	{
		_isLoaded = false;

		int iCompilationStatus;
		int length = 0;
		GLchar *source = (GLchar *)readFileContents(sFile.c_str(), length);
		if(!source)
		{
			PHYSKETCH_LOG_ERROR("Can't read shader file");
			return;
		}

		_shaderID = glCreateShader(a_iType);

		glShaderSource(_shaderID, 1, (const GLchar**)&source, &length);
		free(source);

		glCompileShader(_shaderID);

		glGetShaderiv(_shaderID, GL_COMPILE_STATUS, &iCompilationStatus);

		if(iCompilationStatus == GL_FALSE)
		{
			GLint log_length;
			GLchar *log;

			glGetShaderiv(_shaderID, GL_INFO_LOG_LENGTH, &log_length);
			log = (GLchar*) malloc(log_length);
			glGetShaderInfoLog(_shaderID, log_length, NULL, log);
			PHYSKETCH_LOG_ERROR(log);
			free(log);

			return;
		}

		_type = a_iType;
		_isLoaded = true;
	}
	
	Shader_GL::~Shader_GL()
	{		
		if(!isLoaded()) return;
		_isLoaded = false;
		glDeleteShader(_shaderID);
	}

	bool Shader_GL::isLoaded()
	{
		return _isLoaded;
	}

	UINT Shader_GL::getShaderID()
	{
		return _shaderID;
	}

	//////////////////////////////////////////////////////////////////////////
	/// ShaderProgram_GL
	//////////////////////////////////////////////////////////////////////////

	ShaderProgram_GL::ShaderProgram_GL()
	{
		_isLinked = false;
		_programID = glCreateProgram();
	}

	ShaderProgram_GL::~ShaderProgram_GL()
	{
		glDeleteProgram(_programID);
	}

	bool ShaderProgram_GL::addShaderToProgram( Shader_GL* glShader )
	{
		if(!glShader->isLoaded()) return false;

		glAttachShader(_programID, glShader->getShaderID());
		return true;
	}

	bool ShaderProgram_GL::linkProgram()
	{
		_isLinked = false;

		int iLinkStatus;
		glLinkProgram(_programID);		
		glGetProgramiv(_programID, GL_LINK_STATUS, &iLinkStatus);
		if(iLinkStatus == GL_FALSE)
		{
			GLint log_length;
			GLchar *log;

			glGetProgramiv(_programID, GL_INFO_LOG_LENGTH, &log_length);
			log = (GLchar *)malloc(log_length);
			glGetProgramInfoLog(_programID, log_length, NULL, log);
			PHYSKETCH_LOG_ERROR(log);
			free(log);
			glDeleteProgram(_programID);

			return false;
		}

		_isLinked = true;
		return true;
	}

	void ShaderProgram_GL::useProgram()
	{
		if(_isLinked) 
		{
			glUseProgram(_programID);
		}
	}

	UINT ShaderProgram_GL::getProgramID()
	{
		return _programID;
	}

	bool ShaderProgram_GL::isLinked()
	{
		return _isLinked;
	}

}