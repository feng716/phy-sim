#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>
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
    const std::vector<const char*> validationLayers{
        "VK_LAYER_KHRONOS_validation",
    };
    std::vector<const char*> getDebugExtension();
    void checkValidationLayers();
    void initVulkan();
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT severity,
        VkDebugUtilsMessageTypeFlagsEXT type,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallBackData,
        void* pUserData
    );
    void setupDebugCallback();
    VkResult CreateDebugUtilsMessengerEXT(VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo);
    void DestroyDebugUtilsMessengerEXT();
    void pickPhysicalDevice();
    VkPhysicalDevice physicalDevice=VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT debugMessenger;
    bool isDeviceSuitable(VkPhysicalDevice device);

    void preInit();
#endif
    GLFWwindow* glfwWindow;
public:
    void init();
    void loop();
    ~application();
    void cleanup();
};