
#include "fluid.h"
#include "model.h"
#include <GLFW/glfw3.h>
#include <cstddef>
#include <fmt/format.h>
#include <glm/detail/qualifier.hpp>
#include <glm/ext/vector_float3.hpp>
#include <imgui.h>
#include <spdlog/spdlog.h>
#include "sceneTransform.h"
float fluidParticle::gravityAclr=-980;
float fluidParticle::mass=10;
float fluidParticle::maxLife=5;
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
                particle.push_back(new IFluidParticle(iModel,particle.size(),&particle,this));
            }
        }
    }
}
void meshFluid::configImGUI(){
    if(ImGui::CollapsingHeader("Fluid")){
        ImGui::Text("meshFluid");
    }
}

meshFluid::~meshFluid(){
    for(auto i:particle) delete i;
}

void meshFluid::setAllParticlesScale(transform& iTr){
    for(auto i:particle) i->setScale(iTr.getScale()); 
}

void meshFluid::update(){
    
    for(auto i:particle) i->update();
    for(int i=0;i<particle.size();++i){
        if(particle[i]->getShouldBeRemoveFromVec()) {
            delete particle[i];
            particle.erase(particle.begin()+i);
            i--;
        }
    }
}

void meshFluid::particleSpawn(){
    
}
fluidParticle::fluidParticle(model* modelPtr,int index,std::vector<IFluidParticle*>* iList,meshFluid* iParent){//will take the ownership
    particle=modelPtr;
    force=glm::vec3(0,0,0);
    velocity=glm::vec3(0,0,0);
    spawnRawTime=glfwGetTime();
    currentIndex=index;
    ownerVec=iList;
    shouldBeRemoveFromVec=0;
    parent=iParent;
}

void fluidParticle::gravity(){
    force+=glm::vec3(0,-mass*gravityAclr,0);
}

void fluidParticle::lifeTick(){
    if(glfwGetTime()-spawnRawTime>maxLife) {
        //ownerVec->erase(ownerVec->begin()+currentIndex);
        shouldBeRemoveFromVec=1;
        spdlog::info("particle{}:become death",currentIndex);
    }
}
void fluidParticle::boxCollision(){
}
void fluidParticle::solveAndApply(){
    velocity+=sceneTransform::getTimestep()*force/mass;
    particle->addPosition(velocity*sceneTransform::getTimestep());
    //spdlog::info("pos.y:{}", particle->getPosition().y);
}
fluidParticle::~fluidParticle(){
    delete particle;
}

void fluidParticle::setupImGUI(){
    if(ImGui::CollapsingHeader("Particle Attribute")){
        ImGui::InputFloat("MaxLife",&maxLife);
    }
}
IFluidParticle::IFluidParticle(model* modelPtr,int index,std::vector<IFluidParticle*>* iList,meshFluid* iParent)
    :fluidParticle(modelPtr,index,iList,iParent){}