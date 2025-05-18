#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "GL/glew.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

class Shader {
public:
    GLuint program;

    Shader(const std::string& vertexPath, const std::string& fragmentPath) {
        std::string vertexCode = loadShaderFromFile(vertexPath);
        GLuint vertexShader = compileShader(vertexCode.c_str(), GL_VERTEX_SHADER);

        std::string fragmentCode = loadShaderFromFile(fragmentPath);
        GLuint fragmentShader = compileShader(fragmentCode.c_str(), GL_FRAGMENT_SHADER);

        program = glCreateProgram();
        glAttachShader(program, vertexShader);
        glAttachShader(program, fragmentShader);
        glLinkProgram(program);
        checkCompileErrors(program, "PROGRAM");

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }

    void Use() {
        glUseProgram(program);
    }

    void setUniform1f(const std::string& name, float value) {
        GLint location = glGetUniformLocation(program, name.c_str());
        if (location != -1) {
            glUniform1f(location, value);
        }
    }

    void setUniform3f(const std::string& name, float x, float y, float z) {
        GLint location = glGetUniformLocation(program, name.c_str());
        if (location != -1) {
            glUniform3f(location, x, y, z);
        }
    }

    void setUniform3f(const std::string& name, const std::string& subName, float x, float y, float z) {
        std::string fullName = name + "." + subName;
        setUniform3f(fullName, x, y, z);
    }

    void setUniformMat4(const std::string& name, const glm::mat4& mat) {
        GLint location = glGetUniformLocation(program, name.c_str());
        if (location != -1) {
            glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat));
        }
    }

private:
    std::string loadShaderFromFile(const std::string& path) {
        std::ifstream shaderFile(path);
        std::stringstream shaderStream;
        shaderStream << shaderFile.rdbuf();
        shaderFile.close();
        return shaderStream.str();
    }

    GLuint compileShader(const char* shaderCode, GLenum shaderType) {
        GLuint shader = glCreateShader(shaderType);
        glShaderSource(shader, 1, &shaderCode, nullptr);
        glCompileShader(shader);
        checkCompileErrors(shader, shaderType == GL_VERTEX_SHADER ? "VERTEX" : "FRAGMENT");
        return shader;
    }

    void checkCompileErrors(GLuint shader, const std::string& type) {
        GLint success;
        GLchar infoLog[1024];
        if (type != "PROGRAM") {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success) {
                glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
                std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << std::endl;
            }
        }
        else {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success) {
                glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
                std::cerr << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << std::endl;
            }
        }
    }
};