#include "sceneTransform.h"
#include <glm/ext/vector_float3.hpp>

float sceneTransform::windowW=500,sceneTransform::windowH=600;
glm::vec3 sceneTransform::LookAt=glm::vec3(0,0,0);
float sceneTransform::Timestep=0.1;
void sceneTransform::changeCursorPos(float xpos,float ypos){
    cursorXPos=xpos;
    cursorYPos=ypos;
}