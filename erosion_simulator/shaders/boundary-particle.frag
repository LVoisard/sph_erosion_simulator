#version 330

in vec3 fragNormal;
in vec3 fragPos;
in vec2 texCoord;

flat in int fragIsNeighbour;

out vec4 fragColor;

void main()
{
	if (fragIsNeighbour != 0)
	{
		fragColor = vec4(1,1,0,1);
		return;
	}
	fragColor = vec4(1);
}