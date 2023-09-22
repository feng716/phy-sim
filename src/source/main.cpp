#include "fluid.h"
#include "glad/glad.h"
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <glm/ext/vector_float3.hpp>
#include <iostream>
//glfw
#include <GLFW/glfw3.h>

#include <assimp/scene.h>
#include <assimp/Importer.hpp>

#include <glm/vec3.hpp>

#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui.h>
#include <strings.h>
#include <vector>

#include <spdlog/spdlog.h>
#include "sceneTransform.h"
#include "model.h"
/**
 * @brief a test
 * 
 */
void init();

float spriteRenderer::spriteVertices[24]={
    0.0f, 1.0f, 0.0f, 1.0f,
    1.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f, 

    0.0f, 1.0f, 0.0f, 1.0f,
    1.0f, 1.0f, 1.0f, 1.0f,
    1.0f, 0.0f, 1.0f, 0.0f
};
float fluid::spriteVertices[24]={
    0.0f, 1.0f, 0.0f, 1.0f,
    1.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f, 

    0.0f, 1.0f, 0.0f, 1.0f,
    1.0f, 1.0f, 1.0f, 1.0f,
    1.0f, 0.0f, 1.0f, 0.0f
};

float sceneTransform::cursorXPos=0,sceneTransform::cursorYPos=0;
void GLAPIENTRY
MessageCallback( GLenum source,
                 GLenum type,
                 GLuint id,
                 GLenum severity,
                 GLsizei length,
                 const GLchar* message,
                 const void* userParam );//opengl error callback
void glfwErrorCallBack(int,const char*);
void frameBufferCallBack(GLFWwindow* window,int width,int height);
void keyCallBack(GLFWwindow* window,int key,int sancode,int action,int mods);
void cursorCallBack(GLFWwindow* window,double xpos,double ypos);
void mouseButtonCallBack(GLFWwindow* window,int button,int action,int mods);
int main(){
    spdlog::info("Init GLFW");
    if(!glfwInit()) {spdlog::error("FAILED to load GLFW");return -1;}
    //imgui init
    spdlog::info("Create glfw window");
    GLFWwindow* window = glfwCreateWindow(500, 600, "OpenGL", NULL, NULL);
    if(!window){glfwTerminate();return -1;}
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize OpenGL context" << std::endl;
        return -1;
    }
    glfwSetFramebufferSizeCallback(window, frameBufferCallBack);
    glfwSetKeyCallback(window, keyCallBack);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    glfwSetMouseButtonCallback(window, mouseButtonCallBack);
    glfwSetCursorPosCallback(window, cursorCallBack);
    //gl error callback
    glEnable( GL_DEBUG_OUTPUT );
    glDebugMessageCallback( MessageCallback, 0 );
    float length,height,width;

    meshFluid fl(3,3,3,10);

    ImGui::CreateContext();
    ImGuiIO& io=ImGui::GetIO();
    ImFontConfig fontcfg;
    fontcfg.OversampleH=3;
    io.Fonts->AddFontFromFileTTF("/usr/share/fonts/TTF/FiraCode-Retina.ttf",14,&fontcfg);

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();
    //indexModel cube("3dmodels/cube.fbx","3dmodels/cube.vert","3dmodels/cube.frag",0);

    static offset ofst;
    ofst.z=50;
    bool test=false;
    while(!glfwWindowShouldClose(window)){
        static float lastUpdateTime=0;
        lastUpdateTime=glfwGetTime();
        static float scale=1;
        ImGui_ImplGlfw_NewFrame();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("Settings");
        if(ImGui::CollapsingHeader("Configuration")){
            if(ImGui::TreeNode("General Settings")){
                ImGui::SliderFloat("X Axis", &ofst.x, -10,10);
                ImGui::SliderFloat("Z Axis", &ofst.z, 50,120);
                ImGui::SliderFloat("Y Axis", &ofst.y, -50, 50);
                ImGui::SliderFloat("Scale", &scale,0.1,10);
                ImGui::TreePop();
            }
        }
        fl.setupImGUI();
        fluidParticle::setupImGUI();

        ImGui::End();
        
        ImGui::ShowDemoWindow();
        //temp_tr.setPosition(glm::vec3(ofst.x,ofst.y,-ofst.z));

        sceneTransform::change_LookAt(glm::vec3(ofst.x,ofst.y,-ofst.z));
        //cube.setTransform(temp_tr);m::getdeltaTime());

        fl.update();
        transform temp_tr;
        temp_tr.setScale(glm::vec3(scale));
        fl.setAllParticlesScale(temp_tr);

        glClear(GL_COLOR_BUFFER_BIT);
        model::renderAllModels();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
        sceneTransform::change_deltaTime(glfwGetTime()-lastUpdateTime);
    }
    glfwTerminate();
    return 0;
}
void GLAPIENTRY
MessageCallback( GLenum source,
                 GLenum type,
                 GLuint id,
                 GLenum severity,
                 GLsizei length,
                 const GLchar* message,
                 const void* userParam )
{
  fprintf( stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
           ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
            type, severity, message );
  //spdlog::error("GL CALLBACK:{},type={},severity=0x{},message={}\n",
    //( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),type,severity,message);
    exit(-1);
}


void frameBufferCallBack(GLFWwindow* window,int width,int height){
    glViewport(0,0,width,height);
    sceneTransform::change_windowW(width);
    sceneTransform::change_windowH(height);
}
void keyCallBack(GLFWwindow* window,int key,int sancode,int action,int mods){
    
}
void cursorCallBack(GLFWwindow* window,double xpos,double ypos){
   printf("\rxpos:%f,ypos:%f",xpos,ypos);
}
void mouseButtonCallBack(GLFWwindow* window,int button,int action,int mods){
    static int viewState=0;
    if(button==GLFW_MOUSE_BUTTON_RIGHT&&action==GLFW_PRESS) {viewState++;printf("\n\r%d\n",viewState);}
    if(viewState%2==1) {glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);}
    else glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}
void glfwErrorCallBack(int errorCode,const char* errorString){
    //spdlog::error("GLFWError:{}", errorString);
}