#version 330 core
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

struct Material{
	vec3 ambient;
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};
struct LightDirectional{
	vec3 pos;
	vec3 color;
	vec3 dirToLight;
};
struct LightPoint{
	vec3 pos;
	vec3 color;
	vec3 dirToLight;
	float constant;
	float linear;
	float quadratic;
};
struct LightSpot{
	vec3 pos;
	vec3 color;
	vec3 dirToLight;
	float constant;
	float linear;
	float quadratic;
	float cosPhyInner;
	float cosPhyOuter;
};
uniform Material material;
uniform int NR_DIRECTIONAL_LIGHTS;
uniform int NR_POINT_LIGHTS;
uniform int NR_SPOT_LIGHTS;
uniform LightDirectional lightD[32];
uniform LightPoint lightP[32];
uniform LightSpot lightS[32];
uniform vec3 objColor;
uniform vec3 ambientColor;
uniform vec3 lightPos;
uniform vec3 lightDirUniform;
uniform vec3 lightColor;
uniform vec3 cameraPos;

out vec4 FragColor;

vec3 CalcLightDirectional(LightDirectional light, vec3 uNormal, vec3 dirToCamera);
vec3 CalcLightPoint(LightPoint light, vec3 uNormal, vec3 dirToCamera);
vec3 CalcLightSpot(LightSpot light, vec3 uNormal, vec3 dirToCamera);

void main(){

	vec3 finalResult = vec3(0,0,0);
	vec3 uNormal = normalize(Normal);
	vec3 dirToCamera = normalize(cameraPos - FragPos);

	// DirectionalLight
	for(int i=0; i<NR_DIRECTIONAL_LIGHTS; i++)
		finalResult += CalcLightDirectional(lightD[i], uNormal, dirToCamera);
	
	// PointLight
	for(int i=0; i<NR_POINT_LIGHTS; i++)
		finalResult += CalcLightPoint(lightP[i], uNormal, dirToCamera);

	// SpotLight
	for(int i=0; i<NR_SPOT_LIGHTS; i++)
		finalResult += CalcLightSpot(lightS[i], uNormal, dirToCamera);
	
	FragColor = vec4(finalResult, 1.0);
	
}

vec3 CalcLightDirectional(LightDirectional light, vec3 uNormal, vec3 dirToCamera){
	vec3 ambient = ambientColor * material.ambient * texture(material.diffuse, TexCoord).rgb;

	float diffIntensity = max(dot(light.dirToLight, uNormal), 0);
	vec3 diffColor = diffIntensity * light.color * texture(material.diffuse, TexCoord).rgb;
	
	vec3 R = normalize(reflect(-light.dirToLight, uNormal));
	float specIntensity = pow( max( dot(R, dirToCamera) , 0), material.shininess);
	vec3 specColor = specIntensity * light.color * texture(material.specular, TexCoord).rgb;

	vec3 result = ambient + diffColor + specColor;
	return result;
}

vec3 CalcLightPoint(LightPoint light, vec3 uNormal, vec3 dirToCamera){
	vec3 ambient = ambientColor * material.ambient * texture(material.diffuse, TexCoord).rgb;

	float dist = length(light.pos - FragPos);
	float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * (dist*dist));

	float diffIntensity = max(dot(normalize(light.pos - FragPos), uNormal), 0);
	vec3 diffColor = diffIntensity * light.color * texture(material.diffuse, TexCoord).rgb;

	vec3 R = normalize(reflect(-normalize(light.pos - FragPos), uNormal));
	float specIntensity = pow( max( dot(R, dirToCamera) , 0), material.shininess);
	vec3 specColor = specIntensity * light.color * texture(material.specular, TexCoord).rgb;

	vec3 result = ambient + (diffColor + specColor) * attenuation;
	return result;
}

vec3 CalcLightSpot(LightSpot light, vec3 uNormal, vec3 dirToCamera){
	vec3 ambient = ambientColor * material.ambient * texture(material.diffuse, TexCoord).rgb;

	float dist = length(light.pos - FragPos);
	float attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * (dist*dist));
	float spotRatio;
	float cosTheta = dot(normalize(FragPos - light.pos), -light.dirToLight);
	spotRatio = clamp((cosTheta - light.cosPhyOuter) / (light.cosPhyInner - light.cosPhyOuter), 0.0, 1.0);

	vec3 dirToLight = normalize(light.pos - FragPos);

	float diffIntensity = max(dot(light.dirToLight, uNormal), 0);
	vec3 diffColor = diffIntensity * light.color * texture(material.diffuse, TexCoord).rgb;

	vec3 R = normalize(reflect(-light.dirToLight, uNormal));
	float specIntensity = pow( max( dot(R, dirToCamera) , 0), material.shininess);
	vec3 specColor = specIntensity * light.color * texture(material.specular, TexCoord).rgb;

	vec3 result = ambient + (diffColor + specColor) * attenuation * spotRatio;
	return result;
}