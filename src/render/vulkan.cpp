#include <vulkan/vulkan.h>
#include <print>
#include "vulkan.hpp"

QVulkanWindowRenderer* atomizer::createRenderer(){
    return new atomizerer (this);
}

void atomizerer::initResources(){
    //throw std::runtime_error("resources runtime");
}

void atomizerer::startNextFrame(){
    VkCommandBuffer vk = window->currentCommandBuffer();
    window->frameReady();
    //window->requestUpdate();
}
