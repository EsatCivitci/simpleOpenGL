// GLEW
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// GLM
#include <glm/glm.hpp> 
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp> // Required for glm::value_ptr

#include <fstream>
#include <iostream>
#include <sstream>

#include <string>
#include <vector>

using namespace std;

// Global Variables
GLFWwindow* window;
int gWidth = 1920, gHeight = 1080;

GLuint shaderProgram;

GLuint VAO, VBO, EBO;

GLuint lightVAO; // Light Object VAO
glm::vec3 lightPos(1.2f, 1.0f, 2.0f); // Light source position

glm::vec3 camPos = {0, 0, 3}; // Camera Position
glm::vec3 camFront = {0.0f, 0.0f, -1.0f}; // Gaze 
glm::vec3 camUp = {0.0f, 1.0f, 0.0f}; // Up  


glm::mat4 model;
glm::mat4 view;
glm::mat4 projection;

float yaw = -90.0f; // Left/Right rotation
float pitch = 0.0f; // Up/Down rotation
float lastX = gWidth / 2.0;
float lastY = gHeight / 2.0;
bool firstMouse = true; 


// Function declerations
void init();
void display();
void framebuffer_size_callback(GLFWwindow* window, int width, int height);


// Function to read shader source from a file
string readShaderFile(const string& filePath)
{
    ifstream shaderFile(filePath);
    if (!shaderFile)
    {
        cerr << "Error: Could not open shader file " << filePath << endl;
        return "";
    }

    stringstream shaderStream;
    shaderStream << shaderFile.rdbuf(); // Read file contents into string stream
    return shaderStream.str(); 
}

// Function to initialize shaders
void initShaders()
{
    string vertexShaderSource = readShaderFile("vertex_shader.glsl");
    string fragmentShaderSource = readShaderFile("fragment_shader.glsl");

    if (vertexShaderSource.empty() || fragmentShaderSource.empty())
    {
        cerr << "Failed to read shader file!" << endl;
        return;
    }

    // Create and compile vertex shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    const char* vShaderCode = vertexShaderSource.c_str();
    glShaderSource(vertexShader, 1, &vShaderCode, NULL);
    glCompileShader(vertexShader);

    // Check vertex shader compilation
    GLint success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        cerr << "ERROR: Vertex Shader Compilation Failed\n" << infoLog << endl;
        return;
    }

    // Create and compile fragment shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    const char* fShaderCode = fragmentShaderSource.c_str();
    glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
    glCompileShader(fragmentShader);

    //Check fragment shader compilation
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        cerr << "ERROR: Fragment Shader Compilation Failed\n" << infoLog << endl;
        return;
    }

    // Create shader program and link shaders
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Check linking errors 
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        cerr << "ERROR: Shader Program Linking Failed\n" << infoLog << endl;
        return;
    }

    // Cleanup shaders after linking 
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}   

// Create Vertex Buffer Object
void createVBO()
{
    
    // For Smooth Shading
    GLfloat positions[] = {
    // Positions       
        -0.5f, -0.5f, 0.5f, //1.0f,  // Bottom-left-front 
        0.5f, -0.5f, 0.5f, //1.0f,   // Bottom-right-front 
        0.5f,  0.5f, 0.5f, //1.0f,   // Top-right-front 
        -0.5f,  0.5f, 0.5f, //1.0f,  // Top-left-front 
        -0.5f, -0.5f, -0.5f, //1.0f, // Bottom-left-back 
        0.5f, -0.5f, -0.5f, //1.0f,  // Bottom-right-back 
        0.5f,  0.5f, -0.5f, //1.0f,  // Top-right-back 
        -0.5f,  0.5f, -0.5f, //1.0f  // Top-left-back 
    };

    GLfloat normals[] = {
        // Averaged normals for each unique vertex
        0.0f, -1.0f, 0.0f,   // Bottom
        1.0f, 0.0f, 0.0f,    // Right
        0.0f, 0.0f, 1.0f,    // Front
        -1.0f, 0.0f, 0.0f,   // Left
        -1.0f, -1.0f, -1.0f, // Unused
        1.0f, -1.0f, -1.0f,  // Unused
        0.0f, 0.0f, -1.0f,   // Back
        0.0f, 1.0f, 0.0f     // Top
    };

    // Define indices for two triangles
    GLuint indices[] = {
        0, 1, 2,  // Front Face 1
        3, 0, 2,  // Front Face 2
        4, 7, 6,  // Back Face 1
        5, 4, 6,  // Back Face 2
        5, 6, 1,  // Right Face 1
        6, 2, 1,  // Right Face 2
        4, 0, 3,  // Left Face 1
        7, 4, 3,  // Left Face 2
        3, 2, 7,  // Top Face 1
        2, 6, 7,  // Top Face 2
        5, 1, 0,  // Bottom Face 1
        4, 1, 0   // Bottom Face 2
    }; 

/*
    // For Flat Shading
    GLfloat positions[] = {
        // Front face (z = 0.5)
        -0.5f, -0.5f,  0.5f, 1.0f,  
        0.5f, -0.5f,  0.5f, 1.0f,  
        0.5f,  0.5f,  0.5f, 1.0f,  
        -0.5f,  0.5f,  0.5f, 1.0f,  

        // Back face (z = -0.5)
        -0.5f, -0.5f, -0.5f, 1.0f,  
        0.5f, -0.5f, -0.5f, 1.0f,  
        0.5f,  0.5f, -0.5f, 1.0f,  
        -0.5f,  0.5f, -0.5f, 1.0f,  

        // Right face (x = 0.5)
        0.5f, -0.5f, -0.5f, 1.0f,  
        0.5f, -0.5f,  0.5f, 1.0f,  
        0.5f,  0.5f,  0.5f, 1.0f,  
        0.5f,  0.5f, -0.5f, 1.0f,  

        // Left face (x = -0.5)
        -0.5f, -0.5f, -0.5f, 1.0f,  
        -0.5f, -0.5f,  0.5f, 1.0f,  
        -0.5f,  0.5f,  0.5f, 1.0f,  
        -0.5f,  0.5f, -0.5f, 1.0f,  

        // Top face (y = 0.5)
        -0.5f,  0.5f, -0.5f, 1.0f,  
        0.5f,  0.5f, -0.5f, 1.0f,  
        0.5f,  0.5f,  0.5f, 1.0f,  
        -0.5f,  0.5f,  0.5f, 1.0f,  

        // Bottom face (y = -0.5)
        -0.5f, -0.5f, -0.5f, 1.0f,  
        0.5f, -0.5f, -0.5f, 1.0f,  
        0.5f, -0.5f,  0.5f, 1.0f,  
        -0.5f, -0.5f,  0.5f, 1.0f   
    };
    
    GLfloat normals[] = {
        // Front face
        0.0f,  0.0f,  1.0f, // 0
        0.0f,  0.0f,  1.0f, // 1
        0.0f,  0.0f,  1.0f, // 2
        0.0f,  0.0f,  1.0f, // 3

        // Back face
        0.0f,  0.0f, -1.0f, // 4
        0.0f,  0.0f, -1.0f, // 5
        0.0f,  0.0f, -1.0f, // 6
        0.0f,  0.0f, -1.0f, // 7

        // Right face
        1.0f,  0.0f,  0.0f, // 8
        1.0f,  0.0f,  0.0f, // 9
        1.0f,  0.0f,  0.0f, //10
        1.0f,  0.0f,  0.0f, //11

        // Left face
        -1.0f,  0.0f,  0.0f, //12
        -1.0f,  0.0f,  0.0f, //13
        -1.0f,  0.0f,  0.0f, //14
        -1.0f,  0.0f,  0.0f, //15

        // Top face
        0.0f,  1.0f,  0.0f, //16
        0.0f,  1.0f,  0.0f, //17
        0.0f,  1.0f,  0.0f, //18
        0.0f,  1.0f,  0.0f, //19

        // Bottom face
        0.0f, -1.0f,  0.0f, //20
        0.0f, -1.0f,  0.0f, //21
        0.0f, -1.0f,  0.0f, //22
        0.0f, -1.0f,  0.0f, //23
    }; 

    GLuint indices[] = {
        0, 1, 2,  2, 3, 0,  // Front face
        6, 5, 4,  4, 7, 6,  // Back face
        10, 9, 8, 8, 11, 10, // Right face
        12, 13, 14, 14, 15, 12, // Left face
        18, 17, 16, 16, 19, 18, // Top face
        20, 21, 22, 22, 23, 20  // Bottom face
    };
*/

    // Create a VAO (Vertex Array Object)
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Create a VBO (Vertex Buffer Object)
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(positions) + sizeof(normals), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(positions), positions);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(positions), sizeof(normals), normals);
    //glBufferSubData(GL_ARRAY_BUFFER, sizeof(positions), sizeof(colors), colors);

    // Create a EBO (Element Buffer Object)
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Position attribute (location 0 / Position)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)(sizeof(positions)));
    glEnableVertexAttribArray(1);

    // Position attribute (location 1 / Color)
    // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)(sizeof(positions)));
    // glEnableVertexAttribArray(1);

    // Unbind the VertexArray (not obligatory)
    glBindVertexArray(0);
}

// Rendering a cube
void renderCube()
{
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

// Rendering a ground
void renderGround()
{
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);

    glm::mat4 groundModel = glm::mat4(1.0f);
    groundModel = glm::translate(groundModel, glm::vec3(0.0f, -0.6f, 0.0f));
    groundModel = glm::scale(groundModel, glm::vec3(10.0f, 0.2f, 10.0f));

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(groundModel));

    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

}

// Keyboard inputs
void processInput(GLFWwindow *window)
{
    float camSpeed = 0.05f;

    // Move forward
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
        camPos += camSpeed * camFront;
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
        camPos -= camSpeed * camFront;
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
        camPos -= glm::normalize(glm::cross(camFront,camUp)) * camSpeed;
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
        camPos += glm::normalize(glm::cross(camFront,camUp)) * camSpeed;
    }
}

// Mouse input
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse){
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = ypos - lastY;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.05f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // Prevent screen flip
    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    camFront = glm::normalize(front);
}

// Display/Render Function 
void display()
{
    // Clear the screen (include depth buffer)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Compute matrices
    model = glm::mat4(1.0f);
    view = glm::lookAt(camPos, camPos + camFront, camUp);
    projection = glm::perspective(glm::radians(45.0f), float(gWidth) / float(gHeight), 0.1f, 100.0f);



    // Fix normalMatrix calculation (bottom one in view space, top one does not depend on the camera view)
    glm::mat4 normalMatrix = glm::transpose(glm::inverse(model));
    //glm::mat4 normalMatrix = glm::transpose(glm::inverse(view * model));

    // Use shader before setting uniforms
    glUseProgram(shaderProgram);

    // Sending matrices to shader
    //glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "MVP"), 1, GL_FALSE, glm::value_ptr(projection * view * model));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "normalMatrix"), 1, GL_FALSE, glm::value_ptr(normalMatrix));

    // Render the main object
    renderCube();

    // Render ground object
    renderGround();

}

// Callback Function for Resizing the Window
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    gWidth = width;
    gHeight = height;
    glViewport(0, 0, width, height);
}

// Init function
void init()
{
    //Set the clear color
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

    initShaders();
    if (shaderProgram == 0)
    {
        cerr << "Failed to initialize shaders!" << endl;
        return;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    createVBO();

    // Light properties
    glm::vec3 lightPos(2.0f, 3.0f, 5.0f);
    glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
    glm::vec3 viewPos = camPos; // Use camera position

    // Material properties
    float ambientStrength = 0.1f;
    float diffuseStrength = 0.6f;
    float specularStrength = 0.2f;
    float shininess = 32.0f;

    // Set the uniforms
    GLuint lightPosLoc = glGetUniformLocation(shaderProgram, "lightPos");
    GLuint lightColorLoc = glGetUniformLocation(shaderProgram, "lightColor");
    GLuint viewPosLoc = glGetUniformLocation(shaderProgram, "viewPos");

    GLuint ambientLoc = glGetUniformLocation(shaderProgram, "ambientStrength");
    GLuint diffuseLoc = glGetUniformLocation(shaderProgram, "diffuseStrength");
    GLuint specularLoc = glGetUniformLocation(shaderProgram, "specularStrength");
    GLuint shininessLoc = glGetUniformLocation(shaderProgram, "shininess");

    glUseProgram(shaderProgram);
    glUniform3fv(lightPosLoc, 1, glm::value_ptr(lightPos));
    glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));
    glUniform3fv(viewPosLoc, 1, glm::value_ptr(viewPos));

    glUniform1f(ambientLoc, ambientStrength);
    glUniform1f(diffuseLoc, diffuseStrength);
    glUniform1f(specularLoc, specularStrength);
    glUniform1f(shininessLoc, shininess);


}

int main()
{
    // Initialize GLFW
    if (!glfwInit()){
        cerr << "Failed to initialize GLFW" << endl;
        return -1;
    }

    // Configure GlFW (3.3)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3); 
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // For modern OpemGL
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE); // For legacy OpenGL

    // Create window
    window  = glfwCreateWindow(gWidth, gHeight, "OpenGl Example", NULL, NULL);
    if (!window){
        cerr << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window); // If you have more than one window you can switch the windows dynammically by using this
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); // Test this after adding drawing elements

    // Initialize GLEW
    if (glewInit() != GLEW_OK){
        cerr << "Failed to initialize GLEW" << endl;
        return -1;
    }

    // OpenGL and Application Initialization
    init();

    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // Lock cursor to window
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN); // Partially lock cursor to window

    // Main loop
    
    while(!glfwWindowShouldClose(window)){
        processInput(window);

        // Render a frame
        display();

        // Swap fromt and back buffers
        glfwSwapBuffers(window); 

        // Process user input
        glfwPollEvents();
    }

    // Clean up and terminate
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0; 
}

