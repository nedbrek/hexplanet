#version 130

flat in uvec3 fragTerrainData;
in vec2 texCoord;

vec3 colorForPlate(uint t)
{
	// convert plate number to RGB
	// 0..63 gray
	// 64..127 green
	// 128..191 blue
	// 192..255 red
	uint tone = t & 3u;
	uint hue = t & 0xfcu;

	uint r = 0;
	uint g = 0;
	uint b = 0;

	switch (tone)
	{
	case 0: // dark gray
		r = hue >> 1;
		g = hue >> 1;
		b = hue >> 1;
		break;

	case 1: // green
		g = hue;
		break;

	case 2: // blue
		b = hue;
		break;

	case 3: // red
		r = hue;
		break;

	case 4: // green = blue (aqua)
		g = hue;
		b = hue;
		break;

	case 5: // green = red (orange?)
		r = hue;
		g = hue;
		break;

	case 6: // blue = red (purple?)
		r = hue;
		b = hue;
		break;

	case 7: // light gray
		r = hue;
		g = hue;
		b = hue;
		break;
	}

	return vec3(r, g, b);
}

void main()
{
	gl_FragColor.a = 1;

	float d0 = distance(vec2(0.5, 1.0), texCoord);
	float d1 = distance(vec2(1.0, 0.0), texCoord);
	float d2 = distance(vec2(0.0, 0.0), texCoord);

	int terrainIndex =
	    (d0 < d1) ?
	        (
	            (d0 < d2) ? 0 : 2
	        ) :
	        (
	            (d1 < d2) ? 1 : 2
	        );

	gl_FragColor.rgb = colorForPlate(fragTerrainData[terrainIndex]);
}

