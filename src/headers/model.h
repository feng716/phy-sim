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
    unsigned int VAO;
    unsigned int vertex_buffer;
    std::vector<Vertex> vVertex;
    virtual void setupMesh()=0;
    const aiScene *scene;
    Shader vert;
    ShaderProgram prog;
    Shader frag;
public:
    virtual void draw(offset,float)=0;
    model(char* modelFilePath,char* vertPath,char* fragPath);
};

class indexModel:public model{
private:
    std::vector<uint3> vFace;
    unsigned int indices_buffer;
    int meshIndex;
    virtual void setupMesh() override;
    
public:
    void draw(offset,float) override;
    ~indexModel();
    indexModel(char* modelFilePath,char* vertPath,char* fragPath,int);
};
