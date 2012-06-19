#version 110

attribute vec2 position;
uniform mat3 trans_mat;

void main()
{
	vec3 transformedPos = trans_mat*vec3(position, 1);
	gl_Position = gl_ModelViewProjectionMatrix * vec4(transformedPos, 1.0);
//	gl_Position = vec4(position, 0.0, 1.0);
}