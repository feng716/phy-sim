
#include "fluid.h"
#include "model.h"
#include <cstddef>
#include <glm/ext/vector_float3.hpp>
fluid::fluid(char* iVertPath,char* iFragPath,int l,int w,int h,float iInterval):
    model(iVertPath,iFragPath),numParticleLength(l),numParticleWidth(w),numParticleHeight(h),
    interval(iInterval)
{
    glGenVertexArrays(1,&VAO);
    glGenBuffers(1,&vertex_buffer);
    
    glBindBuffer(GL_ARRAY_BUFFER,vertex_buffer);
    glNamedBufferStorage(vertex_buffer,sizeof(float)*24,spriteVertices,NULL);
    glBindVertexArray(VAO);
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(glm::vec2)+2*sizeof(glm::vec3),0);
    prog.attach(vert);
    prog.attach(frag);
    prog.update();
}

void fluid::setupMesh(){
    for(int i=0;i<numParticleLength;i++){
        for(int j=0;j<numParticleHeight;j++){
            for(int k=0;k<numParticleHeight;k++){
                Vertex v;
                v.position=glm::vec3((i-1)*interval,(j-1)*interval,(k-1)*interval);
                vVertex.push_back(v);//use vertex as point to indicate the position of thr particles
            }
        }
    }
}

void fluid::draw(){
    
}
spriteRenderer::spriteRenderer(){
    
    
}
meshFluid::meshFluid(int l,int w,int h,float interval){
    numParticleHeight=h;
    numParticleLength=l;
    numParticleWidth=w;
    for(int i=0;i<numParticleLength;i++){
        for(int j=0;j<numParticleHeight;j++){
            for(int k=0;k<numParticleHeight;k++){
                model* iModel=new indexModel("3dmodels/sphere.fbx","3dmodels/cube.vert","3dmodels/cube.frag",0);
                iModel->setPosition(glm::vec3(i*interval,j*interval,k*interval));
                particle.push_back(iModel);
            }
        }
    }
}

meshFluid::~meshFluid(){
    for(auto i:particle) delete i;
}

void meshFluid::setAllParticlesScale(transform& iTr){
    for(auto i:particle) i->setScale(iTr.getScale());
}

void meshFluid::update(){
    
}