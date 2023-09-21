#include "glad/glad.h"
#pragma once
class Shader;
class ShaderProgram{
private:
    GLuint obj;
public:
    ShaderProgram();
    void attach(GLuint shader);
    void attach(Shader& shader);
    void update();
    int getGLProgram();
    ~ShaderProgram();
};
class Shader{
private:
friend ShaderProgram;
    GLuint shaderObj;
public:
    Shader(GLenum type,char* shaderFilePath);
    ~Shader();
};