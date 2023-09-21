#version 450
layout(location = 0) in vec4 vert;
out vec2 tex;
uniform mat4 model;
uniform mat4 projection;
void main(){
    tex=vert.zw;
    gl_Position=projection*model*vec4(vert.xy,.0,.1);
}
    