#include <assimp/LogStream.hpp>
#include <assimp/scene.h>
#include <assimp/vector3.h>
#include <cstdio>
#include <glm/detail/qualifier.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "shader.h"
#include <imgui.h>
#include <ostream>
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
private:
    unsigned int VAO;
    unsigned int vertex_buffer;
    unsigned int indices_buffer;
    std::vector<Vertex> vVertex;
    std::vector<uint3> vFace;
    Shader vert;
    ShaderProgram prog;
    Shader frag;
    void setupMesh(int,const aiScene&);
    
public:
    void draw(offset,float);
    model(char* filePath);
    ~model();
};
