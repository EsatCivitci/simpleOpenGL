#version 330 core

layout (location = 0) in vec4 aPos; // Position (x,y,z,1.0f)
layout (location  = 1) in vec3 aNormal; 
//layout (location = 2) in vec3 aColor; // Color (r,g,b)

// Output to fragment shader
//out vec3 vertexColor; // Passed to Fragment Shader
out vec3 FragPos;
out vec3 Normal;

// Uniform Matrices
uniform mat4 MVP; 
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 normalMatrix;


void main()
{
    FragPos = vec3(model * aPos);
    Normal = mat3(normalMatrix) * aNormal;
    //vertexColor = aColor;

    //gl_Position = MVP * aPos; 
    gl_Position = projection * view * model * aPos;
}