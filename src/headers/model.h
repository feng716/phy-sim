#pragma once
#include <assimp/LogStream.hpp>
#include <assimp/scene.h>
#include <assimp/vector3.h>
#include <cstdio>
#include <glm/detail/qualifier.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/quaternion_float.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "shader.h"
#include "basic.h"
#include <imgui.h>
#include <list>
#include <vector>
struct uint3{
    unsigned int x;
    unsigned int y;
    unsigned int z;
};

struct offset{
    float x;
    float y;
    float z;
    glm::mat4 perspective;
    glm::mat4 lookat;
};
glm::vec3 convertAiToGlm(const aiVector3D);
glm::vec2 convertAiToGlm2D(const aiVector3D);

class stdAssimpLogStream:public Assimp::LogStream{
public:
    void write(const char* message){
        printf("%s\n",message);
    }
};
struct Vertex{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;
};
class model{
protected:
    static std::list<model*> modelList;
    std::list<model*>::iterator modelListIndex;
    unsigned int vertex_buffer;
    transform tr;
    transform modelTr;
    unsigned int VAO;
    std::vector<Vertex> vVertex;
    virtual void setupMesh();
    const aiScene *scene;
    Shader vert;
    ShaderProgram prog;
    Shader frag;
public:
    virtual ~model();
    static void renderAllModels();
    virtual void draw()=0;
    void setTransform(transform& itr);
    void setPosition(glm::vec3);
    void setScale(const glm::vec3& ss){tr.setScale(ss);}
    model(char* vertPath,char* fragPath);
};

class indexModel:public model{
private:
    std::vector<uint3> vFace;
    unsigned int indices_buffer;
    int meshIndex;
    char* modelFilePath;
    
public:
    virtual void setupMesh() override;
    void draw() override;
    ~indexModel();
    indexModel(char* modelFilePath,char* vertPath,char* fragPath,int);
};

class camera{
private:
    transform tr;
};