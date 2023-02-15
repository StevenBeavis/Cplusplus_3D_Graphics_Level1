// FRAGMENT SHADER

#version 330

in vec4 color;
out vec4 outColor;

// Materials
uniform vec3 materialAmbient;
uniform vec3 materialDiffuse;
uniform vec3 materialSpecular;
uniform float shininess;
uniform sampler2D texture0;

uniform mat4 matrixView;

in vec4 position;
in vec3 normal;
in vec2 texCoord0;

struct POINT
{
	int on;
	vec3 position;
	vec3 diffuse;
	vec3 specular;
};

uniform POINT lightPoint1, lightPoint2;

vec4 PointLight(POINT light)
{
	// Calculate Point Light
	vec4 color = vec4(0, 0, 0, 0);
	vec3 L= normalize(matrixView * vec4(light.position, 1)-position).xyz;
	float NdotL = dot(normal, L);
	if(NdotL > 0)
		color += vec4(materialDiffuse * light.diffuse, 1) * NdotL;

	vec3 V = normalize(-position.xyz);
	vec3 R = reflect(-L, normal);
	float RdotV = dot(R, V);
	if(NdotL > 0 && RdotV > 0)
		color += vec4(materialSpecular * light.specular * pow(RdotV, shininess), 1);

	return color;
}


void main(void) 
{
	outColor = color;

	if(lightPoint1.on == 1) outColor += PointLight(lightPoint1);

	if(lightPoint2.on == 1) outColor += PointLight(lightPoint2);

	outColor *= texture(texture0, texCoord0);
 
}
