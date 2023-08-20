#include "basic.h"
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <algorithm>
#include <limits>
#include <set>
#include <string>
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_wayland.h>

#define GLFW_INCLUDE_VULKAN
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
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    glfwSetErrorCallback(glfwErrorCallback);
    glfwWindow=glfwCreateWindow(sceneTransform::getwindowW(), sceneTransform::getwindowH(), 
        "phy-sim", nullptr, nullptr);
    
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForVulkan(glfwWindow,true);
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
    setupVulkanSurface();
    pickPhysicalDevice();
    createLogicalDevices();
    createSwapChain();
}
void application::init(){
    spdlog::info("init");
    preInit();
}

void application::loop(){
    spdlog::info("loop");
    while(!glfwWindowShouldClose(glfwWindow)){
        glfwPollEvents();
    }
}

void application::cleanup(){
    vkDestroyDevice(device, nullptr);
    vkDestroySurfaceKHR(iVKInstance, surface, nullptr);
    vkDestroySwapchainKHR(device, swapChain, nullptr);
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
    queueFamilyIndices indices=findQueueFamilies(device);
    
    bool swapChainAdequate=false;
    swapChainSupportDetails detail=querySwapChainSupport(device);
    swapChainAdequate=!detail.formats.empty()&&!detail.presentModes.empty();

    return indices.isComplete()&&checkDeviceExtensionSupport(device)&&swapChainAdequate;
}


auto application::findQueueFamilies(VkPhysicalDevice device)->queueFamilyIndices{
    queueFamilyIndices indices;
    uint32_t queueFamilyCount=0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
    int i=0;
    VkBool32 presentSupport=false;
    for(auto queueFamily:queueFamilies){
        if(queueFamily.queueFlags&VK_QUEUE_GRAPHICS_BIT) indices.graphicsFamily=i;
        vkGetPhysicalDeviceSurfaceSupportKHR(device,i,surface,&presentSupport);
        if(presentSupport) indices.presentFamily=i;
        if(indices.isComplete()) break;
        i++;
    }

    return indices;
}


bool application::checkDeviceExtensionSupport(VkPhysicalDevice device){
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr,&extensionCount,availableExtensions.data());
    std::set<std::string> requiredExtension(deviceExtensions.begin(),deviceExtensions.end());
    for(auto extension:availableExtensions) requiredExtension.erase(extension.extensionName);
    return true;
}
void application::createLogicalDevices(){
    queueFamilyIndices indices=findQueueFamilies(physicalDevice);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

    std::set<uint32_t> uniqueQueueFamilies={indices.graphicsFamily.value(),indices.presentFamily.value()};

    for(auto queueFamily:uniqueQueueFamilies){
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType=VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex=queueFamily;
        queueCreateInfo.queueCount=1;
        float queuePriority = 1.f;
        queueCreateInfo.pQueuePriorities=&queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }
    VkPhysicalDeviceFeatures devicesFeatures{};

    VkDeviceCreateInfo createInfo{};
    createInfo.sType=VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos=queueCreateInfos.data();
    createInfo.queueCreateInfoCount=queueCreateInfos.size();
    createInfo.pEnabledFeatures=&devicesFeatures;
    createInfo.enabledExtensionCount=deviceExtensions.size();
    createInfo.ppEnabledExtensionNames=deviceExtensions.data();
    if(enableValidationTool){
        createInfo.enabledLayerCount=validationLayers.size();
        createInfo.ppEnabledLayerNames=validationLayers.data();
    }else createInfo.enabledLayerCount=0;
    //the queue was created along with logical device
    if(vkCreateDevice(physicalDevice, &createInfo, nullptr, &device)!=VK_SUCCESS) throw std::runtime_error("can't create logical device");
    vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
    vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
}

void application::setupVulkanSurface(){
    auto surfaceCreator=new waylandPlatform(glfwWindow,&iVKInstance);
    surface=surfaceCreator->createSurface();
    if(glfwCreateWindowSurface(iVKInstance,glfwWindow,nullptr,&surface)!=VK_SUCCESS) throw std::runtime_error("failed to create vulkan surface");
}

waylandPlatform::waylandPlatform(GLFWwindow* windowInit,VkInstance* iInstance){
    window=windowInit;
    pInstance=iInstance;
}

VkSurfaceKHR waylandPlatform::createSurface(){
    VkSurfaceKHR surface;
    //auto fpCreateWaylandSurfaceKHR=(PFN_vkCreateWaylandSurfaceKHR)vkGetInstanceProcAddr(*pInstance, "vkCreateWaylandSurfaceKHR");
    //if(!fpCreateWaylandSurfaceKHR) throw std::runtime_error("can't get function address(wayland surface)");
    VkWaylandSurfaceCreateInfoKHR info{VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR};
    info.display=glfwGetWaylandDisplay();
    info.surface=glfwGetWaylandWindow(window);
    
    if(vkCreateWaylandSurfaceKHR(*pInstance,&info,nullptr,&surface)!=VK_SUCCESS) throw std::runtime_error("can't create wayland surface");
    return surface;
}


auto application::querySwapChainSupport(VkPhysicalDevice device)->swapChainSupportDetails{
    swapChainSupportDetails details;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);
    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount,nullptr);
    if(formatCount){
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, 
            &formatCount,details.formats.data());
    }
    
    uint32_t presentModesCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModesCount,nullptr);
    if(presentModesCount){
        details.presentModes.resize(presentModesCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, 
            &presentModesCount,details.presentModes.data());
    }


    return details;
}


VkSurfaceFormatKHR application::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats){
    for(const auto& availableFormat:availableFormats){
        if(availableFormat.format==VK_FORMAT_B8G8R8A8_SRGB&&VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) return availableFormat;
    }
    return availableFormats[0];
}

VkPresentModeKHR application::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes){
    for(const auto& availablePresentModes:availablePresentModes){
        if(availablePresentModes==VK_PRESENT_MODE_MAILBOX_KHR) return availablePresentModes;
    }
    return VK_PRESENT_MODE_FIFO_KHR;
}
VkExtent2D application::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities){
    if(capabilities.currentExtent.width!=std::numeric_limits<uint32_t>::max()) return capabilities.currentExtent;
    else{
        int width,height;
        glfwGetFramebufferSize(glfwWindow, &width, &height);

        VkExtent2D actualExtent={static_cast<uint32_t>(width),static_cast<uint32_t>(height)};
        actualExtent.width=std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height=std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
        return actualExtent;
    }
}
void application::createSwapChain(){
    swapChainSupportDetails swapChainSupport=querySwapChainSupport(physicalDevice);
    VkSurfaceFormatKHR surfaceFormat=chooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode=chooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent=chooseSwapExtent(swapChainSupport.capabilities);
    uint32_t imageCount=swapChainSupport.capabilities.minImageCount+1;
    if(swapChainSupport.capabilities.maxImageCount>0&&imageCount>swapChainSupport.capabilities.maxImageCount) imageCount=swapChainSupport.capabilities.maxImageCount;

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType=VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface=surface;
    createInfo.minImageCount=imageCount;
    createInfo.imageFormat=surfaceFormat.format;
    createInfo.imageColorSpace=surfaceFormat.colorSpace;
    createInfo.imageExtent=extent;
    createInfo.imageArrayLayers=1;
    createInfo.imageUsage=VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    queueFamilyIndices indices=findQueueFamilies(physicalDevice);
    uint32_t queueFamilyArray[]={indices.graphicsFamily.value(),indices.presentFamily.value()};
    if(indices.graphicsFamily!=indices.presentFamily){
        createInfo.imageSharingMode=VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount=2;
        createInfo.pQueueFamilyIndices=queueFamilyArray;
    }else createInfo.imageSharingMode=VK_SHARING_MODE_EXCLUSIVE;

    createInfo.preTransform=swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha=VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode=presentMode;
    createInfo.clipped=VK_TRUE;
    createInfo.oldSwapchain=VK_NULL_HANDLE;

    if(vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain)!=VK_SUCCESS) throw std::runtime_error("failed to create swapchain");
    spdlog::info("create swapchain");


}