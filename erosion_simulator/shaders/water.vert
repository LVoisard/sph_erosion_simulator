#version 330

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;

out vec3 fragNormal;
out vec3 fragPos;
out vec2 texCoord;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;

uniform vec3 offset;

void main()
{
	gl_Position = projection * view * model * vec4(pos + offset, 1.0);
	
	fragNormal = mat3(transpose(inverse(model))) * normal;
	
	fragPos = pos;
	
	texCoord = uv;
}