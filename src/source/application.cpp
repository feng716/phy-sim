#include "basic.h"
#include <GLFW/glfw3.h>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#include <spdlog/spdlog.h>
#include <stdexcept>
#include <sys/types.h>
#include <vector>
#include <vulkan/vulkan_core.h>
#include "sceneTransform.h"
void application::checkValidationLayers(){
    if(enableValidationTool){
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, NULL);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
        bool isFound=false;
        for (auto ValidLayer:validationLayers) {
            for (auto avalLayer:availableLayers)
                if((int)strcmp(ValidLayer, avalLayer.layerName)==0){ isFound=true; break;}else isFound=false;
            if(!isFound) throw std::runtime_error("error check validation layers");
        }
    }
}
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
    checkValidationLayers();
    spdlog::info("initVulkan");

    //create vkInstance
    VkApplicationInfo appInfo{};
    appInfo.sType=VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName="phy-sim";
    appInfo.applicationVersion=VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion=VK_API_VERSION_1_3;
    VkInstanceCreateInfo createInfo{};
    createInfo.sType=VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo=&appInfo;
    auto extensions=getDebugExtension();
    createInfo.enabledExtensionCount=extensions.size();
    createInfo.ppEnabledExtensionNames=extensions.data();
    if(enableValidationTool){
        createInfo.enabledLayerCount=static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames=validationLayers.data();
    }else createInfo.enabledLayerCount=0;
    
    if(vkCreateInstance(&createInfo, nullptr,&iVKInstance)!=VK_SUCCESS) 
        throw std::runtime_error("failed to create vk instance");
    
    setupDebugCallback();
    pickPhysicalDevice();

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
    DestroyDebugUtilsMessengerEXT();
    vkDestroyInstance(iVKInstance, NULL);
    glfwDestroyWindow(glfwWindow);
    glfwTerminate();
}

application::~application(){
}
std::vector<const char*> application::getDebugExtension(){
    uint32_t glfwExtensionsCount=0;
    const char** glfwExtensions;
    glfwExtensions=glfwGetRequiredInstanceExtensions(&glfwExtensionsCount);
    std::vector<const char*> extensions(glfwExtensions,glfwExtensions+glfwExtensionsCount);
    if(enableValidationTool) extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    return extensions;    
}
VKAPI_ATTR VkBool32 VKAPI_CALL application::debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT severity,
    VkDebugUtilsMessageTypeFlagsEXT type,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallBackData,
    void* pUserData
)
{
    switch (severity) {
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
        spdlog::info("validationLayer:{}",pCallBackData->pMessage);
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
        spdlog::warn("validationLayer:{}",pCallBackData->pMessage);
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
        spdlog::error("validationLayer:{}",pCallBackData->pMessage);
        break;
    }
    return VK_FALSE;
};

void application::setupDebugCallback(){
    if(!enableValidationTool) return;
    VkDebugUtilsMessengerCreateInfoEXT createInfo{};
    createInfo.sType=VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity=VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT|
    VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT|VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT|
    VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
    createInfo.messageType=VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT|
    VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT|VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback=debugCallback;
    createInfo.pUserData=nullptr;
    CreateDebugUtilsMessengerEXT(&createInfo);
}

VkResult application::CreateDebugUtilsMessengerEXT(VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo){
    auto func=(PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(iVKInstance,"vkCreateDebugUtilsMessengerEXT");
    if(func != nullptr) return func(iVKInstance,pCreateInfo,nullptr,&debugMessenger);
    else throw std::runtime_error("can't find the address of vkCreateDebugUtilsMessengerEXT");
}
void application::DestroyDebugUtilsMessengerEXT(){
    auto func=(PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(iVKInstance,"vkDestroyDebugUtilsMessengerEXT");
    if(func != nullptr) return func(iVKInstance,debugMessenger,nullptr);
    else throw std::runtime_error("can't find the address of vkDestroyDebugUtilsMessengerEXT");
}


void application::pickPhysicalDevice(){
    uint32_t physicalDeviceCount=0;
    vkEnumeratePhysicalDevices(iVKInstance, &physicalDeviceCount, nullptr);
    if(physicalDeviceCount==0) throw std::runtime_error("failed to find GPUs with vulkan support");
    std::vector<VkPhysicalDevice> devices(physicalDeviceCount);
    vkEnumeratePhysicalDevices(iVKInstance, &physicalDeviceCount, devices.data());
    for(auto device:devices){
        if(isDeviceSuitable(device)){
            physicalDevice=device;
            break;
        }
    }
    if(physicalDevice==nullptr) throw std::runtime_error("failed to find a suitable device");

}


bool application::isDeviceSuitable(VkPhysicalDevice device){
    return findQueueFamilies(device).graphicsFamily.has_value();
}


auto application::findQueueFamilies(VkPhysicalDevice device)->queueFamilyDevices{
    queueFamilyDevices indices;
    uint32_t queueFamilyCount=0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
    int i=0;
    for(auto queueFamily:queueFamilies){
        if(queueFamily.queueFlags&VK_QUEUE_GRAPHICS_BIT) indices.graphicsFamily=i;
        if(indices.graphicsFamily.has_value()) break;
        i++;
    }
    return indices;
}