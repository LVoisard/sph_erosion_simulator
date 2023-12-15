#version 330

#define PI 3.145926538

in vec3 fragNormal;
in vec3 fragPos;
in vec2 texCoord;
flat in int fragIsTarget;
flat in int fragIsNeighbour;
in float fragLinearVelocity;
in float fragSediment;


float minWaterHeight = 0.15;
float minWaterOpacity = 0.3;

float maxWaterHeight = 5.0;
float maxWaterOpacity = 0.8;

out vec4 fragColor;

uniform vec3 viewerPosition;
uniform sampler2D texture0;
uniform float deltaTime;
uniform int waterDebugMode;


float maxSpeed = 64;
float maxSediment = 0.01;


vec3 lightDirection = normalize(vec3(1.0,10.0, 3.0));

void main()
{
	// water velocity
	vec3 baseColor = vec3(15.0 / 256, 94.0 / 256, 156.0 / 256.0);
	if(waterDebugMode == 1) {
		float a = smoothstep(0.0, maxSpeed, fragLinearVelocity);
		baseColor = mix( vec3(15.0 / 256, 94.0 / 256, 156.0 / 256.0), vec3(189.0 /256.0 , 196.0 / 256.0, 197.0 / 256.0), a);
	}
	else if(waterDebugMode == 2) {
		vec3 c;
		if (maxSpeed / 3 >= fragLinearVelocity) {
			float a = smoothstep(0.0, maxSpeed / 3, fragLinearVelocity);
			c = mix(vec3(0,0.1,0.8), vec3(0,0.8,0.1), a);
		} else if (maxSpeed / 3 >= fragLinearVelocity - maxSpeed / 3) {
			float a = smoothstep(0.0, maxSpeed / 3, fragLinearVelocity - maxSpeed / 3);
			c = mix(vec3(0,0.8,0.1), vec3(0.5,0.5,0), a);
		} else {
			float a = smoothstep(0.0, maxSpeed / 3, fragLinearVelocity - 2 * maxSpeed / 3);
			c = mix(vec3(0.5,0.5,0), vec3(0.8,0.1,0), a);
		}
		baseColor = c;
	}
	// sediment transportation
	else if (waterDebugMode == 3)
	{
		// bias the value to make the sediment more obvious, but 33% and 100% sediment will look the same.
		float a = smoothstep(0.0, maxSediment, fragSediment);
		vec3 c = mix(baseColor, vec3(112.0 / 255.0, 84.0 / 255.0, 62.0 / 255.0), a);	
		
		baseColor = c;
	}
	// invisible
	else if( waterDebugMode == 4)
	{
		fragColor = vec4(0);
		return;
	}

	if(fragIsTarget != 0)
	{
		fragColor = vec4(1, 0, 0, 1);
		return;
	}

	if(fragIsNeighbour != 0)
	{
		fragColor = vec4(0, 1, 0, 1);
		return;
	}


	vec3 normalMap = texture(texture0, texCoord).rbg * 2.0 - 1.0;
	vec3 normal = normalize(fragNormal + normalMap);

	float diffuseFactor = max(dot(normal, lightDirection), 0.0);
	vec4 diffuseColor = vec4(vec3(diffuseFactor), 1.0);
	
	vec3 viewingRay = normalize(viewerPosition - fragPos);
	vec3 reflectedvertex = reflect(-lightDirection, normal);		
	float specularFactor = pow(max(dot(viewingRay, reflectedvertex),0), 256);
	vec4 specularColor = vec4(0.5 * vec3(specularFactor), 1.0);
	

	
	vec4 base = vec4(baseColor, 1.0);

	fragColor = clamp(base *(diffuseColor + specularColor), 0, 1);
	fragColor.a = 1;
	
}

