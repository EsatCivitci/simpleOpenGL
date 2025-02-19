#version 330 core

// Interpolated values from the vertex Shader
in vec3 FragPos;      // Fragment position in world space
flat in vec3 Normal;       // Normal vector from vertex shader    
//in vec3 vertexColor;  // Interpolated vertex color

// Final color output 
out vec4 FragColor;

// Point light properties
uniform vec3 lightPos;    // Light position in world space
uniform vec3 lightColor;  // Light Color
uniform vec3 viewPos;     // Camera position (for specular lighting)     

// Material properties
uniform float ambientStrength;
uniform float diffuseStrength;
uniform float specularStrength;
uniform float shininess;

void main()
{
    // Ambient Lighting
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse Lighting
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos); // Direction to the light
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diffuseStrength * diff * lightColor;

    // Specular Lighting (Blinn-Phong)
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), shininess);
    vec3 specular = specularStrength * spec * lightColor;

    // Combine lighting components
    vec3 resultColor = clamp(ambient + diffuse + specular, 0.0, 1.0);

    FragColor = vec4(resultColor, 1.0);
}
