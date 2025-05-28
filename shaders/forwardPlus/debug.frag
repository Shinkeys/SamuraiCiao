#version 460 core

// To debug tile clustered render
layout(rg32ui, binding = 1) uniform uimage2D lightsGrid;

uniform int maxLightsPerCluster;

out vec4 FragColor;

void main()
{
	ivec2 location = ivec2(gl_FragCoord.xy);
	ivec2 tileInd = location / ivec2(16,16);

    uint lightsCount = imageLoad(lightsGrid, tileInd).y;

	if(lightsCount == 0)
	{
		FragColor = vec4(0.0, 0.0, 1.0, 1.0);
	}
	else
	{

		FragColor = vec4(lightsCount / float(maxLightsPerCluster), 0.0, 0.0, 0.0);
	}
}