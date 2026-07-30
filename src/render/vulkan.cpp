#include <QVulkanWindowRenderer>
#include "vulkan.hpp"

QVulkanWindowRenderer* atomizer::createRenderer(){
    return new atomizerer (this);
}

void atomizerer::initResources(){

}

void atomizerer::startNextFrame(){

}
