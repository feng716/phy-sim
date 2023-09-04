
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
                vVertex.push_back(v);
            }
        }
    }
}

void fluid::draw(){

}
spriteRenderer::spriteRenderer(){
    
    
}