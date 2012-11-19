#pragma once
#ifndef Shader_GL_h__
#define Shader_GL_h__

#include "SketchyDynamicsDefinitions.h"

namespace SketchyDynamics
{
	// Base code from http://www.mbsoftworks.sk/index.php?page=tutorials&series=1&tutorial=6

	/// <summary> Wraps OpenGL shader loading and compiling. </summary>
	class Shader_GL
	{
	public:
		/// <summary> Loads and compiles a shader. </summary>
		/// <param name="sFile"> The shader code file. </param>
		/// <param name="a_iType"> The type (GL_VERTEX_SHADER, GL_FRAGMENT_SHADER...). </param>
		Shader_GL(std::string sFile, int a_iType); 

		~Shader_GL();
				
		/// <summary> Query if the shader is loaded. </summary>
		/// <returns> true if loaded, false if not. </returns>
		bool isLoaded(); 

		/// <summary> Get the OpenGL ID of the generated shader. </summary>
		/// <returns> The shader OpenGL ID. </returns>
		UINT getShaderID(); 		

	private: 
		UINT _shaderID;	// ID of shader
		int _type;		// GL_VERTEX_SHADER, GL_FRAGMENT_SHADER...
		bool _isLoaded;	// Whether shader was loaded and compiled	
	};

	/// <summary> Wraps OpenGL shader program. </summary>
	class ShaderProgram_GL
	{
	public:
		ShaderProgram_GL();
		~ShaderProgram_GL();

		/// <summary> Creates a new program. </summary>
		void createProgram();

		/// <summary> Deletes the program and frees memory on GPU. </summary>
		void deleteProgram();

		/// <summary> Adds a shader to the program. </summary>
		/// <param name="shShader"> The shader to add. </param>
		/// <returns> true if it succeeds, false if it fails. </returns>
		bool addShaderToProgram(Shader_GL* glShader);

		/// <summary> Performs final linkage of OpenGL program. </summary>
		/// <returns> true if it succeeds, false if it fails. </returns>
		bool linkProgram();

		/// <summary> Tells OpenGL to use this program. </summary>
		void useProgram();

		/// <summary> Gets the OpenGL ID of this program. </summary>
		/// <returns> The program OpenGL ID. </returns>
		UINT getProgramID();

		/// <summary> Query if this program is linked. </summary>
		/// <returns> true if linked, false if not. </returns>
		bool isLinked();
	private:
		UINT _programID;	// ID of program
		bool _isLinked;	// Whether program was linked and is ready to use

	};

} // namespace SketchyDynamics

#endif // Shader_GL_h__