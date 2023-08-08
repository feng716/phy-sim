#include "glad/glad.h"
#include <cstddef>
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

#include "model.h"
/**
 * @brief a test
 * 
 */
void init(std::vector<model*>&);
static float sWindowW,sWindowH;
void GLAPIENTRY
MessageCallback( GLenum source,
                 GLenum type,
                 GLuint id,
                 GLenum severity,
                 GLsizei length,
                 const GLchar* message,
                 const void* userParam );
void frameBufferCallBack(GLFWwindow* window,int width,int height);

int main(){
    if(!glfwInit()) return -1;
    //imgui init
    GLFWwindow* window = glfwCreateWindow(500, 600, "OpenGL", NULL, NULL);
    if(!window){glfwTerminate();return -1;}
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize OpenGL context" << std::endl;
        return -1;
    }
    glfwSetFramebufferSizeCallback(window, frameBufferCallBack);
    sWindowH=600;
    sWindowW=500;
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
        ofst.windowHeight=sWindowH;
        ofst.windowWidth=sWindowW;

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
  fprintf( stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
           ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
            type, severity, message );
}


void frameBufferCallBack(GLFWwindow* window,int width,int height){
    glViewport(0,0,width,height);
    sWindowH=width;
    sWindowW=height;
}