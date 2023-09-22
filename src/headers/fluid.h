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
    std::vector<fluidParticle*>* ownerVec;
    
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
    fluidParticle(model*,int index,std::vector<fluidParticle*>* iList,meshFluid* iParent);
    virtual ~fluidParticle();
    static void setupImGUI();

    //particle update group
    virtual void particleUpdate()=0;
    void solveAndApply();
    void boxCollision();
    void lifeTick();
    void gravity();

    //particle spawn group
    void boxLocation(float width,float height,float length);
    virtual fluidParticle* particleSpawn(){return this;}
    void setScale(glm::vec3 scalar){particle->setScale(scalar);}
};

class IFluidParticle:public fluidParticle{
public:
    void particleUpdate() override{
        gravity();
        solveAndApply();
        lifeTick();
    };
    fluidParticle* particleSpawn() override{
        boxLocation(10, 10, 10);
        return this;
    }
    IFluidParticle(model* modelPtr,int index,std::vector<fluidParticle*>* iList,meshFluid* iParent);
};

class meshFluid{
friend class fluidParticle;
private:
    float numSpawnRate;
    bool spawnInstantOnce;
    float lastRateSpawnTime;
    int numParticleLength;
    int numParticleWidth;
    int numParticleHeight;
    std::vector<fluidParticle*> particle;
protected:
    void spawnBurstInstantaneous(int numParticle);
    void spawnRate(float rate);
public:
    void setupImGUI();
    ~meshFluid();
    void setAllParticlesScale(transform& iTr);
    virtual void emitterUpdate();
    void update();
    meshFluid(int l,int w,int h,float);
};

class IMeshFluid:public meshFluid{
private:
public:
    void emitterUpdate() override;
};