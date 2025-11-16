#version 330 core

// Outputs colors in RGBA
out vec4 FragColor;


// Imports the color from the Vertex Shader
in vec4 color;
// Imports the texture coordinates from the Vertex Shader
in vec2 texCoord;
// Imports the normal from the Vertex Shader
in vec3 Normal;
// Imports the current position from the Vertex Shader
in vec3 crntPos;

// Gets the Texture Unit from the main function
uniform sampler2D tex0;
// Gets the color of the light from the main function
uniform vec3 lightColor;
// Gets the position of the light from the main function
uniform vec3 lightPos;
// Gets the position of the camera from the main function
uniform vec3 camPos;
// Optional explicit light direction (when previewing or using a directional component)
uniform bool u_useLightDir;
uniform vec3 u_lightDir;
// Fog control
uniform bool u_useFog;
uniform float u_fogDensity;
uniform vec3 u_fogColor;
// Shadow pass flag
uniform bool u_isShadow;

void main()
{
	if(u_isShadow) {
		// Render simple dark translucent shadow, ignore lighting/texture
		FragColor = vec4(0.0, 0.0, 0.0, 0.45) * color;
		return;
	}
	// ambient lighting
	float ambient = 0.25f;

	// diffuse lighting
	vec3 normal = normalize(Normal);
	vec3 lightDirection;
	if(u_useLightDir) {
		lightDirection = normalize(u_lightDir);
	} else {
		lightDirection = normalize(lightPos - crntPos);
	}
	float diffuse = max(dot(normal, lightDirection), 0.0f);

	// specular lighting
	float specularLight = 0.50f;
	vec3 viewDirection = normalize(camPos - crntPos);
	vec3 reflectionDirection = reflect(-lightDirection, normal);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 8);
	float specular = specAmount * specularLight;

	// outputs final color
	FragColor = texture(tex0, texCoord) * color * vec4(lightColor * (diffuse + ambient + specular), 1.0f);

	// Apply fog (exponential) if enabled
	if(u_useFog) {
		float dist = length(camPos - crntPos);
		// fogFactor: 0 = no fog (near), 1 = full fog (far)
		float fogFactor = 1.0 - exp(-u_fogDensity * dist);
		fogFactor = clamp(fogFactor, 0.0, 1.0);
		FragColor = mix(FragColor, vec4(u_fogColor, 1.0f), fogFactor);
	}
}