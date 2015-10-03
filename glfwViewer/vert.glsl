#version 130

uniform vec3 positionData[3];

in int index;
in uvec3 terrainData;

flat out uvec3 fragTerrainData;
out vec2 texCoord;

void main()
{
	// copy terrain data to frag shader
	fragTerrainData = terrainData;

	// calculate uv
	switch (gl_VertexID % 3)
	{
	case 0:
		texCoord = vec2(0, .5);
		break;

	case 1:
		texCoord = vec2(-.5, -.5);
		break;

	case 2:
		texCoord = vec2(.5, -.5);
		break;

	default:
		texCoord = vec2(0, 0);
	}

	vec4 position = vec4(positionData[index], 1);
	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * position;
}

