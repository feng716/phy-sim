#pragma once
#include "model.h"
#include "shader.h"
#include <glm/ext/vector_float3.hpp>
class spriteRenderer{
private:
    ShaderProgram prog;
    unsigned int VAO;
    unsigned int buffer;
public:
    static float spriteVertices[24];
    spriteRenderer();
};
class fluid: public model{//fluid renderer
private:
    float lastBurstSpawnTime;
    float lastRateSpawnTime;
    int numParticleLength;
    int numParticleWidth;
    int numParticleHeight;
    float interval;
    
public:
    static float spriteVertices[24];
    void setupMesh() override;
    void draw() override;
    fluid(char*,char*,int l,int w,int h,float);
};
class IFluidParticle;
class meshFluid;
class fluidParticle{
private:
    static float gravityAclr;
    int currentIndex;
    std::vector<IFluidParticle*>* ownerVec;
    
    static float maxLife;
    meshFluid* parent;
    model* particle;
    float spawnRawTime;
    glm::vec3 force;
    glm::vec3 velocity;

    static float mass;
    int shouldBeRemoveFromVec;//1: should;0: live
public:
    int getShouldBeRemoveFromVec(){return shouldBeRemoveFromVec;}
    fluidParticle(model*,int index,std::vector<IFluidParticle*>* iList,meshFluid* iParent);
    virtual ~fluidParticle();
    static void setupImGUI();
    void solveAndApply();
    
    void boxCollision();
    void lifeTick();
    void gravity();
    void setScale(glm::vec3 scalar){particle->setScale(scalar);}
    virtual void update()=0;
};

class IFluidParticle:public fluidParticle{
public:
    void update() override{
        gravity();
        solveAndApply();
        lifeTick();
    };
    IFluidParticle(model* modelPtr,int index,std::vector<IFluidParticle*>* iList,meshFluid* iParent);
};

class meshFluid{
friend class fluidParticle;
private:
    int numParticleLength;
    int numParticleWidth;
    int numParticleHeight;
    std::vector<IFluidParticle*> particle;
public:
    ~meshFluid();
    void setAllParticlesScale(transform& iTr);
    void particleSpawn();
    void update();
    meshFluid(int l,int w,int h,float);
    void configImGUI();
};
