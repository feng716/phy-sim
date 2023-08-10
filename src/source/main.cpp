#include "glad/glad.h"
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <iostream>
//gflw
#include <GLFW/glfw3.h>

#include <assimp/scene.h>
#include <assimp/Importer.hpp>

#include <glm/vec3.hpp>

#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <imgui.h>
#include <vector>

#include <spdlog/spdlog.h>
#include "sceneTransform.h"
#include "model.h"
/**
 * @brief a test
 * 
 */
void init(std::vector<model*>&);

float sceneTransform::windowW=500,sceneTransform::windowH=600;
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

    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

    //list of models
    std::vector<model*> vModels;
    init(vModels);
    while(!glfwWindowShouldClose(window)){
        static float scale=0.1;
        static offset ofst;
        ImGui_ImplGlfw_NewFrame();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("Settings");
        ImGui::SliderFloat("X Axis", &ofst.x, -1,1);

        ImGui::SliderFloat("Z Axis", &ofst.z, 0,100);
        ImGui::SliderFloat("Scale", &scale,0.1,1);

        ImGui::End();

        glClear(GL_COLOR_BUFFER_BIT);
        for (auto i : vModels) i->draw(ofst,scale);
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}
void init(std::vector<model*>& vM){
    system("pwd");
    vM.push_back(new model("3dmodels/cube.fbx"));
    
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
  //fprintf( stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
           //( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
            //type, severity, message );
  spdlog::error("GL CALLBACK:{},type={},severity=0x{},message={}\n",
    ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),type,severity,message);
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
    spdlog::error("GLFWError:{}", errorString);
}