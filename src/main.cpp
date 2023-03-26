#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <iostream>

#include "shader.h"
#include "camera.h"
#include "model.h"
#include "mesh.h"
#include "skybox.h"
#include "texture.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;
bool blinn = false;
bool blinnKeyPressed = false;

Camera camera(glm::vec3(1.98434,2.08286,2.12496));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

vector<float> skyboxVertices = {
    // positions          
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f,  1.0f
};
vector<std::string> faces{
    // sky box textures
    "C:/Users/45162.CPHXR9000K/Desktop/EarthRenderer/assets/PurpleNebula2048_right.jpg",
    "C:/Users/45162.CPHXR9000K/Desktop/EarthRenderer/assets/PurpleNebula2048_left.jpg",
    "C:/Users/45162.CPHXR9000K/Desktop/EarthRenderer/assets/PurpleNebula2048_top.jpg",
    "C:/Users/45162.CPHXR9000K/Desktop/EarthRenderer/assets/PurpleNebula2048_bottom.jpg",
    "C:/Users/45162.CPHXR9000K/Desktop/EarthRenderer/assets/PurpleNebula2048_front.jpg",
    "C:/Users/45162.CPHXR9000K/Desktop/EarthRenderer/assets/PurpleNebula2048_back.jpg",
};

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Earth", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

    vector<string> texturePaths = {
        "C:/Users/45162.CPHXR9000K/Desktop/EarthRenderer/assets/8k_earth_daymap.jpg",
        "C:/Users/45162.CPHXR9000K/Desktop/EarthRenderer/assets/8k_earth_normal_map.jpg",
        //"C:/Users/45162.CPHXR9000K/Desktop/EarthRenderer/assets/8k_specular_map.jpg"
    };
    vector<string> textureTyes = {
        "texture_diffuse",
        "texture_normal",
        //"texture_specular"
    };

    //stbi_set_flip_vertically_on_load(true);

	glEnable(GL_DEPTH_TEST);

    Shader alShader("C:/Users/45162.CPHXR9000K/Desktop/EarthRenderer/shader/advanced_lighting.vs",
                    "C:/Users/45162.CPHXR9000K/Desktop/EarthRenderer/shader/advanced_lighting.fs");
    alShader.use();

    Shader ourShader("C:/Users/45162.CPHXR9000K/Desktop/EarthRenderer/shader/1.model_loading.vs", 
                     "C:/Users/45162.CPHXR9000K/Desktop/EarthRenderer/shader/1.model_loading.fs");
    Model ourModel("C:/Users/45162.CPHXR9000K/Desktop/EarthRenderer/assets/earth8K.obj");
    ourModel.loadTextures(texturePaths, textureTyes);
    ourModel.loadModel();
    ourShader.use();

    Shader skyboxShader("C:/Users/45162.CPHXR9000K/Desktop/EarthRenderer/shader/skybox.vs", 
                        "C:/Users/45162.CPHXR9000K/Desktop/EarthRenderer/shader/skybox.fs");
    Skybox ourSkybox(faces, skyboxVertices);
    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);


    glm::vec3 lightPos(1.5f, 1.0f, 1.5f);//! Light Position
    // render loop
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content

        glm::mat4 projection;
        glm::mat4 view;
        glm::mat4 model;

        //- be sure to activate shader when setting uniforms/drawing objects
        // ourShader.use();
        // projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        // view = camera.GetViewMatrix();
        // model = glm::mat4(1.0f);
        // model = glm::translate(model, glm::vec3(0.0f, -1.0f, -1.0f)); 
        // model = glm::rotate(model,glm::radians(180.0f), glm::vec3(0.0, 1.0, 0.0));
        // model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));

        // ourShader.setMat4("projection", projection);
        // ourShader.setMat4("view", view);
        // ourShader.setMat4("model", model);

        // ourModel.Draw(ourShader); //!  here

        alShader.use();
        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        view = camera.GetViewMatrix();
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); 
        model = glm::rotate(model,glm::radians(0.00f), glm::vec3(0.0, 1.0, 0.0));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));

        alShader.setMat4("projection", projection);
        alShader.setMat4("view", view);
        alShader.setMat4("model", model);

        alShader.setVec3("viewPos", camera.Position);
        alShader.setVec3("lightPos", lightPos);
        alShader.setInt("blinn",blinn);

        ourModel.Draw(alShader); //! advanced shader draw here


        //----------------------- draw skybox----------------------------------------------
        skyboxShader.use();

        view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
        skyboxShader.setMat4("view", view);
        skyboxShader.setMat4("projection", projection);

        ourSkybox.Draw(skyboxShader);


        //cout << camera.Position.r << camera.Position.g << camera.Position.b << endl;
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    // glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
    return 0;
}


void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
