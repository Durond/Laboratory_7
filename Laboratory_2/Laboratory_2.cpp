#define GLEW_DLL
#define GLFW_DLL

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "GL/glew.h"
#include "GL/glfw3.h"
#include "Shader.h"
#include "glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "Model.h"

const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float yaw = -90.0f;
float pitch = 0.0f;

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

struct ObjectTransform {
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(1.0f);
    glm::vec3 pivotPoint = glm::vec3(0.0f);

    struct {
        float min = -180.0f;
        float max = 180.0f;
    } rotationLimit;
};

std::vector<ObjectTransform> objectTransforms;

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

void processInput(GLFWwindow* window) {
    float cameraSpeed = 2.5f * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

    float rotateSpeed = 50.0f * deltaTime;


    // первая точка вращения клавиши N M 
    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) {
        objectTransforms[1].rotation.y += rotateSpeed;
        if (objectTransforms[1].rotation.y > objectTransforms[1].rotationLimit.max)
            objectTransforms[1].rotation.y = objectTransforms[1].rotationLimit.max;
    }
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) {
        objectTransforms[1].rotation.y -= rotateSpeed;
        if (objectTransforms[1].rotation.y < objectTransforms[1].rotationLimit.min)
            objectTransforms[1].rotation.y = objectTransforms[1].rotationLimit.min;
    }


    // вторая точка вращения клавиши J K 
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
        objectTransforms[2].rotation.z += rotateSpeed;
        if (objectTransforms[2].rotation.z > objectTransforms[2].rotationLimit.max)
            objectTransforms[2].rotation.z = objectTransforms[2].rotationLimit.max;
    }
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
        objectTransforms[2].rotation.z -= rotateSpeed;
        if (objectTransforms[2].rotation.z < objectTransforms[2].rotationLimit.min)
            objectTransforms[2].rotation.z = objectTransforms[2].rotationLimit.min;
    }


    // третья точка вращения клавиши U I 
    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) {
        objectTransforms[3].rotation.y += rotateSpeed;
        if (objectTransforms[3].rotation.y > objectTransforms[3].rotationLimit.max)
            objectTransforms[3].rotation.y = objectTransforms[3].rotationLimit.max;
    }
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
        objectTransforms[3].rotation.y -= rotateSpeed;
        if (objectTransforms[3].rotation.y < objectTransforms[3].rotationLimit.min)
            objectTransforms[3].rotation.y = objectTransforms[3].rotationLimit.min;
    }
}

glm::mat4 calculateModelMatrix(int index) {
    glm::mat4 model = glm::mat4(1.0f);

    if (index == 0) {
        return model;
    }

    if (index == 1) {
        model = glm::translate(model, objectTransforms[1].pivotPoint);
        model = glm::rotate(model, glm::radians(objectTransforms[1].rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::translate(model, -objectTransforms[1].pivotPoint);
        model = glm::translate(model, objectTransforms[1].position);
    }

    if (index == 2) {
        model = glm::translate(model, objectTransforms[1].pivotPoint);
        model = glm::rotate(model, glm::radians(objectTransforms[1].rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::translate(model, -objectTransforms[1].pivotPoint);
        model = glm::translate(model, objectTransforms[1].position);

        model = glm::translate(model, objectTransforms[2].pivotPoint);
        model = glm::rotate(model, glm::radians(objectTransforms[2].rotation.z), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::translate(model, -objectTransforms[2].pivotPoint);
    }

    if (index == 3) {
        model = glm::translate(model, objectTransforms[1].pivotPoint);
        model = glm::rotate(model, glm::radians(objectTransforms[1].rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::translate(model, -objectTransforms[1].pivotPoint);
        model = glm::translate(model, objectTransforms[1].position);

        model = glm::translate(model, objectTransforms[2].pivotPoint);
        model = glm::rotate(model, glm::radians(objectTransforms[2].rotation.z), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::translate(model, -objectTransforms[2].pivotPoint);

        model = glm::translate(model, objectTransforms[3].pivotPoint);
        model = glm::rotate(model, glm::radians(objectTransforms[3].rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::translate(model, -objectTransforms[3].pivotPoint);
    }

    return model;
}

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "3D Model Preview with Lighting", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);

    Model ourModel("../assets/mymodel.obj");
    Shader shader("vertex_shader.glsl", "fragment_shader.glsl");

    objectTransforms.resize(4);
    objectTransforms[1].pivotPoint = glm::vec3(0.13029f, -0.03f, 0.0f);
    objectTransforms[2].pivotPoint = glm::vec3(0.0f, 0.9638f, -0.333f);
    objectTransforms[3].pivotPoint = glm::vec3(0.1235f, 0.0f, -0.955f);

    objectTransforms[1].rotationLimit = { -90.0f, 90.0f };
    objectTransforms[2].rotationLimit = { -45.0f, 90.0f };

    shader.Use();
    shader.setUniform3f("light.position", 1.2f, 1.0f, 2.0f);
    shader.setUniform3f("light.ambient", 0.2f, 0.2f, 0.2f);
    shader.setUniform3f("light.diffuse", 0.7f, 0.7f, 0.7f);
    shader.setUniform3f("light.specular", 1.0f, 1.0f, 1.0f);

    shader.setUniform3f("material.ambient", 0.6f, 0.9f, 0.8f);
    shader.setUniform3f("material.diffuse", 0.6f, 0.9f, 0.8f);
    shader.setUniform3f("material.specular", 0.8f, 0.9f, 0.85f);
    shader.setUniform1f("material.shininess", 32.0f);

    while (!glfwWindowShouldClose(window)) {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.Use();
        shader.setUniform3f("viewPos", cameraPos.x, cameraPos.y, cameraPos.z);

        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        shader.setUniformMat4("projection", projection);
        shader.setUniformMat4("view", view);

        // обновление модели для каждой сетки 
        for (size_t i = 0; i < ourModel.meshTransforms.size(); ++i) {
            ourModel.meshTransforms[i] = calculateModelMatrix(i);
        }

        ourModel.Draw(shader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}