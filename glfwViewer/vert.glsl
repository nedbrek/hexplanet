#version 130

in vec3 position;

void main()
{
	vec4 pos4 = vec4(position, 1);
	gl_TexCoord[0] = pos4;
	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * pos4;
}

