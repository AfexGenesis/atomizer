#include "vulkan.hpp"

QVulkanWindowRenderer* atomizer::createRenderer(){
    return new atomizerer (this);
}