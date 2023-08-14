#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>
#ifdef VULKAN_API
#include <vulkan/vulkan.h>
#endif
class application{
private:
#ifdef VULKAN_API
    VkInstance iVKInstance;
    void initVulkan();
    void preInit();
#endif
    GLFWwindow* glfwWindow;
public:
    void init();
    void loop();
    void cleanup();
};