#include <QFile>
#include <QtTypes>
#include <QVulkanDeviceFunctions>
#include <vulkan/vulkan.h>
#include <print>
#include <DirectXMath.h>
#include "vulkan.hpp"

using namespace DirectX;

struct vertex {
    DirectX::XMFLOAT2 position;
    DirectX::XMFLOAT3 colour;
};

static const vertex vertexd[] = {
    {{-0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}},
    {{-0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
    {{0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}}
};

static const int size = sizeof(DirectX::XMFLOAT4X4);

static inline VkDeviceSize aligned(VkDeviceSize v, VkDeviceSize alignByte){
    return (v + alignByte - 1) & ~(alignByte - 1);
}

atomizerer::atomizerer(QVulkanWindow *window, bool msaa) : windows(window), cam(DirectX::XMFLOAT4(0.0f, 0.0f, -5.0f, 1.0f)){
    if (msaa){
        const QList<int> samples = windows->supportedSampleCounts();
        qDebug() << "debug supported samples" << samples;
        for (int s = 16; s >= 4; s /= 2){
            if(samples.contains(s)){
                qDebug("debug samples %d", s);
                windows->setSampleCount(s);
                break;
            }
        }
    }
}

VkShaderModule atomizerer::createShader(const QString &sildursshader){
    QFile file(sildursshader);
    if(!file.open(QIODevice::ReadOnly)){
        qWarning("failed %s", qPrintable(sildursshader));
        throw std::runtime_error("no shader");
        return VK_NULL_HANDLE;
    }

    QByteArray blob = file.readAll();
    file.close();

    VkShaderModuleCreateInfo superman;
    memset(&superman, 0, sizeof(superman));
    superman.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    superman.codeSize = blob.size();
    superman.pCode = reinterpret_cast<const uint32_t *>(blob.constData());
    VkShaderModule shadurModule;

    VkResult result = devicef->vkCreateShaderModule(windows->device(), &superman, nullptr, &shadurModule);
    if (result != VK_SUCCESS){
        qWarning("did not create a shader module %d", result);
        qFatal("did not ran");
        return VK_NULL_HANDLE;
    }
    return shadurModule;
    
}

void atomizerer::initResources(){
    qDebug("Debug:");
    VkDevice device = windows->device();
    devicef = windows->vulkanInstance()->deviceFunctions(device);
    const int ccf = windows->concurrentFrameCount();
    const VkPhysicalDeviceLimits *pdevicel = &windows->physicalDeviceProperties()->limits;
    const VkDeviceSize uniAlign = pdevicel->minUniformBufferOffsetAlignment;
    qDebug("Debugging on uniAlign %d", (uint) uniAlign);
    VkBufferCreateInfo bufferinfo;
    memset(&bufferinfo, 0, sizeof(bufferinfo));
    bufferinfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;

    const VkDeviceSize vallosaurs = aligned(sizeof(vertexd), uniAlign);
    const VkDeviceSize uallosaurs = aligned(size, uniAlign);
    bufferinfo.size = vallosaurs + ccf * uallosaurs;
    bufferinfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

    VkResult result = devicef->vkCreateBuffer(device, &bufferinfo, nullptr, &buffer);
    if (result != VK_SUCCESS){
        qFatal("device function fatal %d", result);
    }

    VkMemoryRequirements onlyfans;
    devicef->vkGetBufferMemoryRequirements(device, buffer, &onlyfans);
    VkMemoryAllocateInfo mallosaurs ={
        VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO, nullptr, onlyfans.size,
        windows->hostVisibleMemoryIndex()
    };

    result = devicef->vkAllocateMemory(device, &mallosaurs, nullptr, &bufferm);
    if (result != VK_SUCCESS){
        qFatal("failed to give allosaurs memory %d", result);
    }

    result = devicef->vkBindBufferMemory(device, buffer, bufferm, 0);
    if (result != VK_SUCCESS){
        qFatal("failed to give allosaurs buffer %d", result);
    }
        
    quint8 *pointer;
    result = devicef->vkMapMemory(device, bufferm, 0, onlyfans.size, 0, reinterpret_cast<void **> (&pointer));
    if (result != VK_SUCCESS){
        qFatal("pointers did not work %d", result);
    }

    memcpy(pointer, vertexd, sizeof(vertexd));
    DirectX::XMMATRIX id = DirectX::XMMatrixIdentity();
    DirectX::XMFLOAT4X4 data;
    DirectX::XMStoreFloat4x4(&data, id);
    memset(uallosaursi, 0, sizeof(uallosaursi));
    for(int i = 0; i < ccf; ++i){
        const VkDeviceSize offset = vallosaurs + i * uallosaurs;
        memcpy(pointer + offset, &data, sizeof(DirectX::XMFLOAT4X4));
        uallosaursi[i].buffer = buffer;
        uallosaursi[i].offset = offset;
        uallosaursi[i].range = uallosaurs;
    }
    devicef->vkUnmapMemory(device, bufferm);

    VkVertexInputBindingDescription vertexb ={
        0, sizeof(vertex), VK_VERTEX_INPUT_RATE_VERTEX
    };
    VkVertexInputAttributeDescription vertexa[] ={
        {
            0, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(vertex, position)
        },
        {
            1, 0, VK_FORMAT_R32G32B32_SFLOAT, sizeof(DirectX::XMFLOAT2)
        }
    };
    VkPipelineVertexInputStateCreateInfo verstappen;
    verstappen.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    verstappen.pNext = nullptr;
    verstappen.flags = 0;
    verstappen.vertexBindingDescriptionCount = 1;
    verstappen.pVertexBindingDescriptions = &vertexb;
    verstappen.vertexAttributeDescriptionCount = 2;
    verstappen.pVertexAttributeDescriptions = vertexa;

    VkDescriptorPoolSize pools = {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, uint32_t(ccf)};
    VkDescriptorPoolCreateInfo poolc;
    memset(&poolc, 0, sizeof(poolc));
    poolc.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolc.maxSets = ccf;
    poolc.poolSizeCount = 1;
    poolc.pPoolSizes = &pools;
    result = devicef->vkCreateDescriptorPool(device, &poolc, nullptr, &pooler);
    if (result != VK_SUCCESS){
        qFatal("Q FATAL idfk %d", result);
    }

    VkDescriptorSetLayoutBinding layout ={
        0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT, nullptr
    };
    VkDescriptorSetLayoutCreateInfo layoutb ={
        VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO, nullptr, 0, 1,  &layout
    };
    result = devicef->vkCreateDescriptorSetLayout(device, &layoutb, nullptr, &layer);
    if (result != VK_SUCCESS){
        qFatal("failed at creation of layout %d", result);
    }

    for (int i = 0; i < ccf; i++){
        VkDescriptorSetAllocateInfo layouta ={
            VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO, nullptr, pooler, 1, &layer
        };
        result = devicef->vkAllocateDescriptorSets(device, &layouta, &layers[i]);
        if (result != VK_SUCCESS){
            qFatal("qFatal n Allocating layout %d", result);
        }
        VkWriteDescriptorSet write;
        memset(&write, 0, sizeof(write));
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.dstSet = layers[i];
        write.descriptorCount = 1;
        write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        write.pBufferInfo = &uallosaursi[i];
        devicef->vkUpdateDescriptorSets(device, 1, &write, 0, nullptr);
    }

    VkPipelineCacheCreateInfo ass;
    memset(&ass, 0, sizeof(ass));
    ass.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
    result = devicef->vkCreatePipelineCache(device, &ass, nullptr, &pipeche);
    if (result != VK_SUCCESS){
        qFatal("pipe got stuck in ass %d", result);
    }

    VkPipelineLayoutCreateInfo pipeline;
    memset(&pipeline, 0, sizeof(pipeline));
    pipeline.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline.setLayoutCount = 1;
    pipeline.pSetLayouts = &layer;
    result = devicef->vkCreatePipelineLayout(device, &pipeline, nullptr, &pipeout);
    if (result != VK_SUCCESS){
        qFatal("couldn't create the pipeline %d", result);
    }
    
    VkShaderModule fortnite = createShader(QStringLiteral("../shaders/fragment.spv"));
    VkShaderModule minecraft = createShader(QStringLiteral("../shaders/vertex.spv"));

    VkGraphicsPipelineCreateInfo pipelinec;
    memset(&pipelinec, 0, sizeof(pipelinec));
    pipelinec.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;

    VkPipelineShaderStageCreateInfo shader[2]={
        {
            VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO, nullptr, 0,
            VK_SHADER_STAGE_VERTEX_BIT, minecraft, "main", nullptr
        },
        {
            VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO, nullptr, 0,
            VK_SHADER_STAGE_FRAGMENT_BIT, fortnite, "main", nullptr
        }
    };

    pipelinec.stageCount = 2;
    pipelinec.pStages = shader;
    pipelinec.pVertexInputState = &verstappen;

    VkPipelineInputAssemblyStateCreateInfo inputa;
    memset(&inputa, 0, sizeof(inputa));
    inputa.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputa.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    pipelinec.pInputAssemblyState = &inputa;

    VkPipelineViewportStateCreateInfo viewp;
    memset(&viewp, 0, sizeof(viewp));
    viewp.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewp.viewportCount = 1;
    viewp.scissorCount = 1;
    pipelinec.pViewportState = &viewp;

    VkPipelineRasterizationStateCreateInfo georgerussel;
    memset(&georgerussel, 0 , sizeof(georgerussel));
    georgerussel.polygonMode = VK_POLYGON_MODE_FILL;
    georgerussel.cullMode = VK_CULL_MODE_NONE;
    georgerussel.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    georgerussel.lineWidth = 1.0f;
    pipelinec.pRasterizationState = &georgerussel;

    VkPipelineMultisampleStateCreateInfo alonso;
    memset(&alonso, 0, sizeof(alonso));
    alonso.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    alonso.rasterizationSamples = windows->sampleCountFlagBits();
    pipelinec.pMultisampleState = &alonso;

    VkPipelineDepthStencilStateCreateInfo leclerc;
    memset(&leclerc, 0, sizeof(leclerc));
    leclerc.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    leclerc.depthTestEnable = VK_TRUE;
    leclerc.depthWriteEnable = VK_TRUE;
    leclerc.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
    pipelinec.pDepthStencilState = &leclerc;

    VkPipelineColorBlendStateCreateInfo piastri;
    VkPipelineColorBlendAttachmentState lando;
    memset(&piastri, 0, sizeof(piastri));
    memset(&lando, 0, sizeof(lando));
    lando.colorWriteMask = 0xF;
    piastri.attachmentCount = 1;
    piastri.pAttachments = &lando;
    pipelinec.pColorBlendState = &piastri;

    VkDynamicState aerodynamic[]={ VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
    VkPipelineDynamicStateCreateInfo aerodynamics;
    memset(&aerodynamics, 0, sizeof(aerodynamics));
    aerodynamics.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    aerodynamics.dynamicStateCount = sizeof(aerodynamic) / sizeof(VkDynamicState);
    aerodynamics.pDynamicStates = aerodynamic;
    pipelinec.pDynamicState = &aerodynamics;
    pipelinec.layout = pipeout;
    pipelinec.renderPass = windows->defaultRenderPass();

    result = devicef->vkCreateGraphicsPipelines(device, pipeche, 1, &pipelinec, nullptr, &pipelane);
    if (result != VK_SUCCESS){
        qFatal("no graphic pipeline for u %d", result);
    }
    if (minecraft){
        devicef->vkDestroyShaderModule(device, minecraft, nullptr);
    }
    if (fortnite){
        devicef->vkDestroyShaderModule(device, fortnite, nullptr);
    }
    //throw std::runtime_error("resources runtime");
}

void atomizerer::initSwapChainResources(){
    const QSize qsize = windows->swapChainImageSize();
    const float aspect = qsize.height() ? qsize.width() / (float) qsize.height() : 1.0f;
    DirectX::XMMATRIX projectile = DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV2, aspect, 0.01f, 100.0f);
    DirectX::XMFLOAT4X4 projectiled;
    DirectX::XMStoreFloat4x4(&projectiled, projectile);

    projectiled.m[1][1] *= -1.0f;
    projm = projectiled;
    markViewProjDirty();
}

void atomizerer::releaseResources(){
    qDebug("releaseResources");

    VkDevice device = windows->device();
    if (pipelane){
        devicef->vkDestroyPipeline(device, pipelane, nullptr);
        pipelane = VK_NULL_HANDLE;
    }
    if (pipeche){
        devicef->vkDestroyPipelineCache(device, pipeche, nullptr);
        pipelane = VK_NULL_HANDLE;
    }
    if (pipeout){
        devicef->vkDestroyPipelineLayout(device, pipeout, nullptr);
        pipeout = VK_NULL_HANDLE;
    }
    if (layout){
        devicef->vkDestroyDescriptorSetLayout(device, layout, nullptr);
        layout = VK_NULL_HANDLE;
    }
    if (pooler){
        devicef->vkDestroyDescriptorPool(device, pooler, nullptr);
        pooler = VK_NULL_HANDLE;
    }
    if (buffer){
        devicef->vkDestroyBuffer(device, buffer, nullptr);
        buffer = VK_NULL_HANDLE;
    }
    if (bufferm){
        devicef->vkFreeMemory(device, bufferm, nullptr);
        bufferm = VK_NULL_HANDLE;
    }
}

void atomizerer::getMatrices(DirectX::XMFLOAT4X4 *mvp, DirectX::XMFLOAT4X4 *model, DirectX::XMFLOAT4X4 *normalmode, DirectX::XMFLOAT4 *eyep){
    DirectX::XMMATRIX m = DirectX::XMMatrixIdentity();
    DirectX::XMStoreFloat4x4(model, m);
    DirectX::XMMATRIX normalm = DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(nullptr, m));
    DirectX::XMStoreFloat4x4(normalmode, normalm);
    DirectX::XMFLOAT4X4 viewf = cam.matrix();
    DirectX::XMMATRIX view = DirectX::XMLoadFloat4x4(&viewf);
    DirectX::XMMATRIX proj = DirectX::XMLoadFloat4x4(&projm);
    DirectX::XMMATRIX mvpm = DirectX::XMMatrixMultiply(DirectX::XMMatrixMultiply(m, view), proj);
    DirectX::XMStoreFloat4x4(mvp, mvpm);
    DirectX::XMMATRIX inview = DirectX::XMMatrixInverse(nullptr, view);
    DirectX::XMStoreFloat4(eyep, inview.r[3]);
}

void atomizerer::startNextFrame(){
    VkDevice device = windows->device();
    VkCommandBuffer piastry = windows->currentCommandBuffer();
    const QSize qsize = windows->swapChainImageSize();
    VkClearColorValue cc = {{0, 0, 0, 1}};
    VkClearDepthStencilValue cd = {1, 0};
    VkClearValue c[3];
    memset(&c, 0, sizeof(c));
    c[0].color = c[2].color = cc;
    c[1].depthStencil = cd;
    
    VkRenderPassBeginInfo pass;
    memset(&pass, 0, sizeof(pass));
    pass.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    pass.renderPass = windows->defaultRenderPass();
    pass.framebuffer = windows->currentFramebuffer();
    pass.renderArea.extent.width = qsize.width();
    pass.renderArea.extent.height = qsize.height();
    pass.clearValueCount = windows->sampleCountFlagBits() > VK_SAMPLE_COUNT_1_BIT? 3 : 2;
    pass.pClearValues = c;
    VkCommandBuffer commandblock = windows->currentCommandBuffer();
    devicef->vkCmdBeginRenderPass(commandblock, &pass, VK_SUBPASS_CONTENTS_INLINE);

    quint8 *pointer;
    VkResult result = devicef->vkMapMemory(device, bufferm, uallosaursi[windows->currentFrame()]
    .offset, size, 0, reinterpret_cast<void **> (&pointer));
    if (result != VK_SUCCESS){
        qFatal("no pointer of the quint8 secondary weapon %d", result);
    }

    DirectX::XMFLOAT4X4 mvp, model, normalmode;
    DirectX::XMFLOAT4 eyep;
    getMatrices(&mvp, &model, &normalmode, &eyep);
    memcpy(pointer, &mvp, sizeof(DirectX::XMFLOAT4X4));

    devicef->vkUnmapMemory(device, bufferm);

    devicef->vkCmdBindPipeline(piastry, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelane);
    devicef->vkCmdBindDescriptorSets(piastry, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeout, 0, 1, 
    &layers[windows->currentFrame()], 0, nullptr);
    VkDeviceSize size = 0;
    devicef->vkCmdBindVertexBuffers(piastry, 0, 1, &buffer, &size);
    VkViewport vp;

    vp.x = vp.y = 0;
    vp.width = qsize.width();
    vp.height = qsize.height();
    vp.minDepth = 0;
    vp.maxDepth = 1;
    devicef->vkCmdSetViewport(piastry, 0, 1, &vp);

    VkRect2D rs;
    rs.offset.x = rs.offset.y = 0;
    rs.extent.width = vp.width;
    rs.extent.height = vp.height;
    devicef->vkCmdSetScissor(piastry, 0, 1, &rs);

    devicef->vkCmdDraw(piastry, 3, 1, 0, 0);
    devicef->vkCmdEndRenderPass(commandblock);
    windows->frameReady();
    windows->requestUpdate();
}

void atomizerer::yaw(float degrees){
    QMutexLocker locker(&mutexgui);
    cam.yaw(degrees);
    markViewProjDirty();
}

void atomizerer::pitch(float degrees){
    QMutexLocker locker(&mutexgui);
    cam.pitch(degrees);
    markViewProjDirty();
}

void atomizerer::walk(float amount){
    QMutexLocker locker(&mutexgui);
    cam.walk(amount);
    markViewProjDirty();
}

void atomizerer::strafe(float amount){
    QMutexLocker locker(&mutexgui);
    cam.strafe(amount);
    markViewProjDirty();
}