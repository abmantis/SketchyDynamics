//VERTEX SHADER
#version 110

attribute vec2 position;
attribute vec2 inTexCoord;
uniform mat3 transMat;

varying vec2 texCoordVarying;

void main()
{
	vec3 transformedPos = transMat*vec3(position, 1);
	gl_Position = gl_ModelViewProjectionMatrix * vec4(transformedPos, 1.0);
	//texCoordVarying = inTexCoord;
	texCoordVarying = position;
}