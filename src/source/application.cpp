#include "basic.h"
#include <GLFW/glfw3.h>
#include <cstddef>
#include <cstdint>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#include <spdlog/spdlog.h>
#include <stdexcept>
#include <vulkan/vulkan_core.h>
#include "sceneTransform.h"
void application::preInit(){
    spdlog::info("preInit");
#ifdef VULKAN_API 
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwInit();

    initVulkan();
#endif
}

void application::initVulkan(){
    spdlog::info("initVulkan");
    VkApplicationInfo appInfo{};
    appInfo.sType=VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName="phy-sim";
    appInfo.applicationVersion=VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion=VK_API_VERSION_1_3;
    VkInstanceCreateInfo createInfo{};
    createInfo.sType=VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo=&appInfo;
    uint32_t glfwExtensionCount=0;
    const char** glfwExtensions=glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    createInfo.enabledExtensionCount=glfwExtensionCount;
    createInfo.ppEnabledExtensionNames=glfwExtensions;
    createInfo.enabledLayerCount=0;
    if(vkCreateInstance(&createInfo, nullptr,&iVKInstance)!=VK_SUCCESS) 
        throw std::runtime_error("failed to create vk instance");
    
}
void application::init(){
    spdlog::info("init");
    preInit();
#ifdef VULKAN_API
    glfwWindow=glfwCreateWindow(sceneTransform::getwindowW(), sceneTransform::getwindowH(), 
        "phy-sim", nullptr, nullptr);
    
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForVulkan(glfwWindow,true);
#endif
}

void application::loop(){
    spdlog::info("loop");
    while(!glfwWindowShouldClose(glfwWindow)){
        glfwPollEvents();
    }
}

void application::cleanup(){
    glfwDestroyWindow(glfwWindow);
    glfwTerminate();
}