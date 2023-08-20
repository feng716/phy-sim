#version 450 core
layout(location=0) in vec4 vPosition;
layout(location=1) in vec4 vNormal;
layout(location=2) in vec2 vUV;
uniform mat4 matrix;
void main(){
    gl_Position=matrix*vPosition;
}
