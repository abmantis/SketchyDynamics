//FRAGMENT SHADER
#version 110

varying vec2 texCoordVarying;
uniform vec4 color;
uniform sampler2D texture;
uniform float textureFlag;

void main()
{
    // Set the output color of our current pixel
    //gl_FragColor = color;
	gl_FragColor = mix(
                    color,
                    texture2D(texture, texCoordVarying),
                    textureFlag
                  );
}