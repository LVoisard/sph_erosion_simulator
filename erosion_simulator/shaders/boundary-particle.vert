#version 330

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;
layout (location = 3) in mat4 aInstanceMatrix;

out vec3 fragNormal;
out vec3 fragPos;
out vec2 texCoord;

uniform mat4 projection;
uniform mat4 view;

void main()
{
	gl_Position = projection * view * aInstanceMatrix * vec4(pos, 1.0);
	
	fragNormal = mat3(transpose(inverse(aInstanceMatrix))) * normal;
	
	fragPos = pos;
	
	texCoord = uv;	
}