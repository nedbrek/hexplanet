#version 130

uniform sampler1D positionData;

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
		texCoord = vec2(.5, 1);
		break;

	case 1:
		texCoord = vec2(1, 0);
		break;

	case 2:
		texCoord = vec2(0, 0);
		break;

	default:
		texCoord = vec2(0, 0);
	}

	vec4 position = texelFetch(positionData, index, 0);
	gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * position;
}

