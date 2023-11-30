#version 330

#define PI 3.145926538

in vec3 fragNormal;
in vec3 fragPos;
in vec2 texCoord;
flat in int fragIsTarget;
flat in int fragIsNeighbour;
in float fragLinearVelocity;


float minWaterHeight = 0.15;
float minWaterOpacity = 0.3;

float maxWaterHeight = 5.0;
float maxWaterOpacity = 0.8;

out vec4 fragColor;

uniform vec3 viewerPosition;
uniform sampler2D texture0;
uniform float deltaTime;
uniform int waterDebugMode;


float maxSpeed = 100;


vec3 lightDirection = normalize(vec3(1.0,10.0, 3.0));

float getGreen(float velocity)
{
	velocity = velocity / maxSpeed;

	float g = 0;
	if(velocity < 0.25)
	{
		g = velocity * 2;
	}
	else if (velocity >= 0.75)
	{
		g = 1 - velocity * 2;
	}

	return g;
}

float getRed(float velocity)
{
	velocity = velocity / maxSpeed;

	
	return (velocity - 0.25) * (4/3);
	
}

float getBlue(float velocity)
{
	velocity = velocity / maxSpeed;

	if(velocity > 0.5)
		return 0;
	else
		return 1 - (3/4) * velocity;

}

void main()
{
	// water velocity
	if(waterDebugMode == 1) {
		vec3 c;
		if (maxSpeed / 2 >= fragLinearVelocity) {
			float a = smoothstep(0.0, maxSpeed / 2, fragLinearVelocity);
			c = mix(vec3(0,0.1,0.9), vec3(0,0.9,0.1), a);
		} else {
			float a = smoothstep(0.0, maxSpeed / 2, fragLinearVelocity - maxSpeed / 2);
			c = mix(vec3(0,0.9,0.1), vec3(0.9,0.1,0), a);
		}
		fragColor = vec4(c, 1);
		return;
	}
	// sediment transportation
	else if (waterDebugMode == 2)
	{
		fragColor = vec4(1,0,1,1);
		return;
	}
	// invisible
	else if( waterDebugMode == 3)
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

	vec3 baseColor = vec3(15.0 / 256, 94.0 / 256, 156.0 / 256.0);

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

