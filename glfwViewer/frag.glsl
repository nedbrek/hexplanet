#version 130

flat in uvec3 fragTerrainData;

vec3 colorForTerrain(uint t)
{
	switch (t)
	{
	case 0: // water
		return vec3(0., 0., 1.);
	case 1: // desert (brown)
		return vec3(0.625, 0.625, 0.);
	case 2: // grass
		return vec3(0., 1., 0.);
	case 3: // forest
		return vec3(0., 0.5, 0.);
	case 4: // mountain
		return vec3(0.25, 0.25, 0.25);
	}

	return vec3(0., 0., 0.);
}

void main()
{
	gl_FragColor.a = 1;

	float d0 = distance(vec2(0, 0.5), gl_TexCoord[0].xy);
	float d1 = distance(vec2(0.5, -0.5), gl_TexCoord[0].xy);
	float d2 = distance(vec2(-0.5, -0.5), gl_TexCoord[0].xy);

	if (d0 < d1)
	{
		if (d0 < d2)
		{
			// d0 is lowest
			gl_FragColor.rgb = colorForTerrain(fragTerrainData[0]);
		}
		else
		{
			// d2 is lowest
			gl_FragColor.rgb = colorForTerrain(fragTerrainData[2]);
		}
	}
	else
	{
		if (d1 < d2)
		{
			// d1 is lowest
			gl_FragColor.rgb = colorForTerrain(fragTerrainData[1]);
		}
		else
		{
			// d2 is lowest
			gl_FragColor.rgb = colorForTerrain(fragTerrainData[2]);
		}
	}
}

