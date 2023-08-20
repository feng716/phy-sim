#include "model.h"
#include "glad/glad.h"
#include <assimp/Importer.hpp>
#include <assimp/Logger.hpp>
#include <assimp/mesh.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/DefaultLogger.hpp>
#include <assimp/vector3.h>
#include <cstdio>
#include <fstream>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/matrix_projection.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/trigonometric.hpp>
#include <iostream>
#include <linux/limits.h>
#include <sstream>
#include <sys/types.h>
#include "sceneTransform.h"

glm::vec3 convertAiToGlm(const aiVector3D aivec){
    glm::vec3 temp(aivec.x,aivec.y,aivec.z);
    return temp;
}

glm::vec2 convertAiToGlm2D(const aiVector3D aivec){
    glm::vec2 temp(aivec.x,aivec.y);
    return temp;
}


void model::setupMesh(int index,const aiScene& scene){
    aiMesh* mesh=scene.mMeshes[index];
    for(uint i=0;i<mesh->mNumVertices;i++){
        Vertex iVertex;
        iVertex.position=convertAiToGlm(mesh->mVertices[i]);
        iVertex.normal=convertAiToGlm(mesh->mNormals[i]);
        iVertex.uv=convertAiToGlm2D(mesh->mTextureCoords[0][i]);
        vVertex.push_back(iVertex);
    }
    for(uint i=0;i<mesh->mNumFaces;i++){
        uint3 iFace;
        iFace.x=mesh->mFaces[i].mIndices[0];
        iFace.y=mesh->mFaces[i].mIndices[1];
        iFace.z=mesh->mFaces[i].mIndices[2];
        vFace.push_back(iFace);
    }
}

model::model(char* filePath):
    vert(GL_VERTEX_SHADER,"3dmodels/cube.vert"),
    frag(GL_FRAGMENT_SHADER,"3dmodels/cube.frag"){
    Assimp::DefaultLogger::create("",Assimp::Logger::VERBOSE);
    Assimp::DefaultLogger::get()->info("begin load modes");
    Assimp::DefaultLogger::get()->attachStream(new stdAssimpLogStream, Assimp::Logger::Err);
    
    Assimp::Importer impter;
    const aiScene *scene=impter.ReadFile(filePath,
        aiProcess_Triangulate | aiProcess_GenNormals );
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
        Assimp::DefaultLogger::get()->error("assimp import failed");
        return;
    }//thanks,learnopengl!

    setupMesh(0, *scene);
#ifndef VULKAN_API
    glCreateBuffers(1,&vertex_buffer);
    glCreateBuffers(1,&indices_buffer);
    glNamedBufferStorage(vertex_buffer,scene->mMeshes[0]->mNumVertices*sizeof(Vertex),
        vVertex.data(),0);
    glCreateVertexArrays(1,&VAO);
    glNamedBufferStorage(indices_buffer,sizeof(glm::vec3)*vFace.size(),vFace.data(),0);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER,vertex_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,indices_buffer);
    //
    glVertexAttribPointer(0,3,GL_FLOAT,false,sizeof(glm::vec2)+2*sizeof(glm::vec3),0);
    glVertexAttribPointer(1,3,GL_FLOAT,false,sizeof(glm::vec2)+2*sizeof(glm::vec3),(void*)sizeof(glm::vec3));
    glVertexAttribPointer(2,2,GL_FLOAT,false,2*sizeof(glm::vec3)+sizeof(glm::vec2),(void*)(2*sizeof(glm::vec3)));
    /*
    layout 0 vec3 postion
    layout 1 vec3 normal
    layout 2 vec2 uv 
    */
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
#endif
    prog.attach(vert);
    prog.attach(frag);
    prog.update();
}

model::~model(){
    Assimp::DefaultLogger::kill();

}

Shader::Shader(GLenum type,char* shaderFilePath){
    std::ifstream file;
    file.open(shaderFilePath);
    std::stringstream ss;
    ss<<file.rdbuf();
    file.close();
    const char* code;
    std::string t=ss.str();
    code =t.c_str();
#ifndef VULKAN_API
    shaderObj=glCreateShader(type);
    glShaderSource(shaderObj,1,&code,NULL);
    glCompileShader(shaderObj);
    char *log=new char[1000];
    int *length=new int;
    glGetShaderInfoLog(shaderObj,1000*sizeof(char),length,log);
    printf(log);
    delete [] log;
    delete length;
#endif
}
ShaderProgram::ShaderProgram(){
#ifndef VULKAN_API

    obj=glCreateProgram();
#endif
}

void ShaderProgram::update(){
#ifndef VULKAN_API
    glLinkProgram(obj);
    int success;
    glGetProgramiv(obj,GL_LINK_STATUS,&success);
    if(success) printf("link success");
    else{
        char* log=new char[1000];
        int* length=new int;
        glGetProgramInfoLog(obj,1000,length,log);
        printf(log);
        delete [] log;
        delete length;
    }
    glUseProgram(obj);
#endif
}

void ShaderProgram::attach(uint shader){
#ifndef VULKAN_API
    glAttachShader(obj,shader);
#endif
}

void ShaderProgram::attach(Shader& shader){
#ifndef VULKAN_API
    glAttachShader(obj,shader.shaderObj);
#endif
}
Shader::~Shader(){
#ifndef VULKAN_API
    glDeleteShader(shaderObj);
#endif
}

ShaderProgram::~ShaderProgram(){
#ifndef VULKAN_API
    glDeleteProgram(obj);
#endif
}
int ShaderProgram::getGLProgram(){
    return obj;
}
void model::draw(offset iOffset,float sscale){
#ifndef VULKAN_API
    glBindVertexArray(VAO);
    glm::mat4 proj=glm::perspective(glm::radians(45.f),sceneTransform::getwindowW()/sceneTransform::getwindowH(),0.1f,150.f);
    glm::mat4 modeltrans(1.0f);
    glm::mat4 trans=glm::translate(modeltrans,glm::vec3(iOffset.x,0,-iOffset.z));
    
    glm::mat4 scale=glm::scale(glm::mat4(1.), glm::vec3(sscale));
    glm::mat4 mat=proj*trans*scale;
    glUniformMatrix4fv(glGetUniformLocation(prog.getGLProgram(),"matrix"),1,GL_FALSE,glm::value_ptr(mat));
    glDrawElements(GL_TRIANGLES,vFace.size()*3,GL_UNSIGNED_INT,0);
#endif
}
