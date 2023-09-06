#pragma once
#include "model.h"
#include "shader.h"
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

class meshFluid{
private:
    int numParticleLength;
    int numParticleWidth;
    int numParticleHeight;
    std::vector<model*> particle;
public:
    ~meshFluid();
    void setAllParticlesScale(transform& iTr);
    void update();
    meshFluid(int l,int w,int h,float);
};