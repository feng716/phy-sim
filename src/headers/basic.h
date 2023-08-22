#include <memory>
#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#define GLFW_EXPOSE_NATIVE_WAYLAND
#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>
#include <vulkan/vulkan_wayland.h>
#include <vulkan/vulkan.h>
#include <cstdint>
#include <spdlog/spdlog.h>
#include <optional>
#include <vector>
#include <vulkan/vk_platform.h>
#include <vulkan/vulkan_core.h>
#ifdef VULKAN_API
#include <vulkan/vulkan.h>
#endif
class application{
private:
#ifdef VULKAN_API
    VkInstance iVKInstance;
#ifdef NDEBUG
const bool enableValidationTool=false;
#else
const bool enableValidationTool=true;
#endif
    struct queueFamilyIndices{
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;
        bool isComplete(){return graphicsFamily.has_value()&&presentFamily.has_value();}
    };
    struct swapChainSupportDetails{
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };
    const std::vector<const char*> validationLayers{
        "VK_LAYER_KHRONOS_validation",
    };
    const std::vector<const char*> deviceExtensions{
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    std::vector<const char*> getDebugExtension();
    void checkValidationLayers();
    void createGraphicsPipeline();
    std::vector<char> loadShaders(const std::string& fileName);
    VkShaderModule createShaderModule(const std::vector<char>&);
    void initVulkan();
    VkDevice device;
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    VkSurfaceKHR surface;
    
    void setupVulkanSurface();
    swapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
    std::vector<VkImageView> swapChainImageViews;
    std::vector<VkImage> swapChainImages;

    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
    void createImageViews();

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT severity,
        VkDebugUtilsMessageTypeFlagsEXT type,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallBackData,
        void* pUserData
    );
    static void glfwErrorCallback(int error,const char* dscrp){
        spdlog::error("glfw:error:{}:{}",error,dscrp);
    }
    void setupDebugCallback();
    VkResult CreateDebugUtilsMessengerEXT(VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo);
    void DestroyDebugUtilsMessengerEXT();
    void pickPhysicalDevice();
    VkPhysicalDevice physicalDevice=VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT debugMessenger;
    bool isDeviceSuitable(VkPhysicalDevice device);
    queueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
    void createSwapChain();
    VkSwapchainKHR swapChain;
    void preInit();
    void createLogicalDevices();
#endif/*VULKAN_API*/
    GLFWwindow* glfwWindow;
public:
    void init();
    void loop();
    ~application();
    void cleanup();
};

/***/
class waylandPlatform{
private:
    GLFWwindow* window;
    VkInstance* pInstance;
    
public:
    waylandPlatform(GLFWwindow* windowInit,VkInstance* iInstance);
    VkSurfaceKHR createSurface();
    
};