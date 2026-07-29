#include <vulkan/vulkan.h>
#include "vulkan.hpp"

QVulkanWindowRenderer* atomizer::createRenderer(){
    return new atomizerer (this);
}
void atomizerer::initResources(){
    initVulkan();
}
void atomizerer::initVulkan(){
    vulkanInstance();
}

void atomizerer::vulkanInstance(){
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "atomizer";
    appInfo.applicationVersion = VK_MAKE_VERSION(1,0,0);
    appInfo.pEngineName = "afexium";
    appInfo.engineVersion = VK_MAKE_VERSION(1,0,0);
    appInfo.apiVersion = VK_API_VERSION_1_0;
    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    

    createInfo.enabledLayerCount = 0;
    VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);

    if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS){
        throw std::runtime_error("failed to create instance");
    }
}
void atomizerer::startNextFrame(){

}
