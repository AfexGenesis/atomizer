#include <QFile>
#include <QtTypes>
#include <QVulkanDeviceFunctions>
#include <vulkan/vulkan.h>
#include <algorithm>
#include <cmath>
#include <cstring>
#include <limits>
#include <print>
#include <DirectXMath.h>
#include "render/vulkan.hpp"
#include "core/atom/atominfo.hpp"
#include "render/atoms/atomselect.hpp"

using namespace DirectX;

static const int size = 2 * sizeof(DirectX::XMFLOAT4X4);

static inline VkDeviceSize aligned(VkDeviceSize v, VkDeviceSize alignByte){
    return (v + alignByte - 1) & ~(alignByte - 1);
}

atomizerer::atomizerer(QVulkanWindow *window, const std::vector<atom> &atomsis,
                       const std::vector<segment> &segments, const std::vector<bond> &bondsis,
                       bool msaa) : windows(window), atoms(atomsis), bonds(bondsis),
                       model(mmodel(atomsis, segments)),
                       cam(DirectX::XMFLOAT4(0.0f, 0.0f, -5.0f, 1.0f)){
    if (!atoms.empty()){
        DirectX::XMFLOAT3 minimum(
            std::numeric_limits<float>::max(),
            std::numeric_limits<float>::max(),
            std::numeric_limits<float>::max()
        );
        DirectX::XMFLOAT3 maximum(
            std::numeric_limits<float>::lowest(),
            std::numeric_limits<float>::lowest(),
            std::numeric_limits<float>::lowest()
        );
        for (const auto &a : atoms){
            minimum.x = std::min(minimum.x, a.x);
            minimum.y = std::min(minimum.y, a.y);
            minimum.z = std::min(minimum.z, a.z);
            maximum.x = std::max(maximum.x, a.x);
            maximum.y = std::max(maximum.y, a.y);
            maximum.z = std::max(maximum.z, a.z);
        }

        const DirectX::XMFLOAT3 center(
            (minimum.x + maximum.x) * 0.5f,
            (minimum.y + maximum.y) * 0.5f,
            (minimum.z + maximum.z) * 0.5f
        );
        const float halfx = (maximum.x - minimum.x) * 0.5f + 1.0f;
        const float halfy = (maximum.y - minimum.y) * 0.5f + 1.0f;
        const float halfz = (maximum.z - minimum.z) * 0.5f + 1.0f;
        const float sceneradius = std::sqrt(halfx * halfx + halfy * halfy + halfz * halfz);
        const float cameradistance = std::max(sceneradius * 1.6f, 5.0f);

        cam.setPosition(DirectX::XMFLOAT4(center.x, center.y, center.z - cameradistance, 1.0f));
        movspeed = std::max(sceneradius * 0.5f, 2.5f);
        farplane = std::max(sceneradius * 12.0f, 100.0f);
    }

    if (msaa){
        const QList<int> samples = windows->supportedSampleCounts();
        qDebug() << "debug supported samples" << samples;
        for (int s = 4; s >= 2; s /= 2){
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

void atomizerer::createHostBuffer(VkBuffer &target, VkDeviceMemory &memory, VkBufferUsageFlags usage, const void *data, VkDeviceSize bytes){
    VkDevice device = windows->device();
    VkBufferCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    info.size = bytes;
    info.usage = usage;
    VkResult result = devicef->vkCreateBuffer(device, &info, nullptr, &target);
    
    if (result != VK_SUCCESS) qFatal("model buffer creation failed %d", result);
    VkMemoryRequirements requirements;
    devicef->vkGetBufferMemoryRequirements(device, target, &requirements);
    VkMemoryAllocateInfo allocation{VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO, nullptr, requirements.size, windows->hostVisibleMemoryIndex()};

    result = devicef->vkAllocateMemory(device, &allocation, nullptr, &memory);
    if (result != VK_SUCCESS) qFatal("model memory allocation failed %d", result);
    result = devicef->vkBindBufferMemory(device, target, memory, 0);

    if (result != VK_SUCCESS) qFatal("model buffer binding failed %d", result);
    void *mapped = nullptr;
    result = devicef->vkMapMemory(device, memory, 0, bytes, 0, &mapped);

    if (result != VK_SUCCESS) qFatal("model memory mapping failed %d", result);
    if (data) memcpy(mapped, data, bytes);
    devicef->vkUnmapMemory(device, memory);
}

void atomizerer::initResources(){
    qDebug("Debug:");
    VkDevice device = windows->device();
    devicef = windows->vulkanInstance()->deviceFunctions(device);
    const int ccf = windows->concurrentFrameCount();
    const VkPhysicalDeviceLimits *pdevicel = &windows->physicalDeviceProperties()->limits;
    const VkDeviceSize uniAlign = pdevicel->minUniformBufferOffsetAlignment;
    qDebug("Debugging on uniAlign %d", (uint) uniAlign);
    atomized atomized;
    indexc = uint32_t(atomized.indi().size());
    VkBufferCreateInfo bufferinfo;
    memset(&bufferinfo, 0, sizeof(bufferinfo));
    bufferinfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;

    const VkDeviceSize vallosaurs = atomized.verti().size() * sizeof(atomertex);
    const VkDeviceSize uniform = aligned(vallosaurs, uniAlign);
    const VkDeviceSize uallosaurs = aligned(size, uniAlign);
    bufferinfo.size = uniform + ccf * uallosaurs;
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

    memcpy(pointer, atomized.verti().data(), atomized.verti().size() * sizeof(atomertex));
    DirectX::XMMATRIX id = DirectX::XMMatrixIdentity();
    renderuniforms data;
    DirectX::XMStoreFloat4x4(&data.view, id);
    DirectX::XMStoreFloat4x4(&data.projection, id);
    memset(uallosaursi, 0, sizeof(uallosaursi));
    for(int i = 0; i < ccf; ++i){
        const VkDeviceSize offset = uniform + i * uallosaurs;
        memcpy(pointer + offset, &data, sizeof(DirectX::XMFLOAT4X4));
        uallosaursi[i].buffer = buffer;
        uallosaursi[i].offset = offset;
        uallosaursi[i].range = size;
    }
    devicef->vkUnmapMemory(device, bufferm);

    const VkDeviceSize indexs = atomized.indi().size() * sizeof(uint32_t);
    VkBufferCreateInfo indexb;
    memset(&indexb, 0, sizeof(indexb));
    indexb.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    indexb.size = indexs;
    indexb.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;

    result = devicef->vkCreateBuffer(device, &indexb, nullptr, &ibuffer);
    if (result != VK_SUCCESS){
        qFatal("failted at creating the index buffer %d", result);
    }

    VkMemoryRequirements indexmri;
    devicef->vkGetBufferMemoryRequirements(device, ibuffer, &indexmri);
    VkMemoryAllocateInfo iallosaurus ={
        VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO, nullptr, indexmri.size, windows->hostVisibleMemoryIndex()
    };

    result = devicef->vkAllocateMemory(device, &iallosaurus, nullptr, &ibufferm);
    if (result != VK_SUCCESS){
        qFatal("failed to enable index memory %d", result);
    }

    result = devicef->vkBindBufferMemory(device, ibuffer, ibufferm, 0);
    if (result != VK_SUCCESS){
        qFatal("jarvis locate goth mommys in 69km radius %d", result);
    }

    quint8 *ipointer;
    result = devicef->vkMapMemory(device, ibufferm, 0, indexs, 0, reinterpret_cast<void **> (&ipointer));
    if (result != VK_SUCCESS){
        qFatal("can't map memory find god %d", result);
    }
    memcpy(ipointer, atomized.indi().data(), indexs);
    devicef->vkUnmapMemory(device, ibufferm);

    acount = uint32_t(atoms.size());
    if (acount > 0){
        std::vector<insdata> instances = make_atom_instances(atoms, false);
        auto spacefill = make_atom_instances(atoms, true);
        auto links = makebondinstances(atoms, bonds);
        bcount = uint32_t(links.size());
        instances.insert(instances.end(), spacefill.begin(), spacefill.end());
        instances.insert(instances.end(), links.begin(), links.end());
        qDebug("atoms: %u, bond halves: %u", acount, bcount);

        const VkDeviceSize instances_size = instances.size() * sizeof(insdata);
        VkBufferCreateInfo instanceb;
        memset(&instanceb, 0, sizeof(instanceb));
        instanceb.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        instanceb.size = instances_size;
        instanceb.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

        result = devicef->vkCreateBuffer(device, &instanceb, nullptr, &atomb);
        if (result != VK_SUCCESS){
            qFatal("failed at creating the instance buffer %d", result);
        }

        VkMemoryRequirements instancemri;
        devicef->vkGetBufferMemoryRequirements(device, atomb, &instancemri);
        VkMemoryAllocateInfo instancealloc ={
            VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO, nullptr, instancemri.size, windows->hostVisibleMemoryIndex()
        };

        result = devicef->vkAllocateMemory(device, &instancealloc, nullptr, &atomdm);
        if (result != VK_SUCCESS){
            qFatal("failed to allocate instance memory %d", result);
        }

        result = devicef->vkBindBufferMemory(device, atomb, atomdm, 0);
        if (result != VK_SUCCESS){
            qFatal("failed to bind instance buffer %d", result);
        }

        quint8 *instancepointer;
        result = devicef->vkMapMemory(device, atomdm, 0, instances_size, 0, reinterpret_cast<void **> (&instancepointer));
        if (result != VK_SUCCESS){
            qFatal("can't map instance memory %d", result);
        }
        memcpy(instancepointer, instances.data(), instances_size);
        devicef->vkUnmapMemory(device, atomdm);
        overlaystride = aligned(VkDeviceSize(acount + bcount)*sizeof(insdata), 256);
        createHostBuffer(overlayb, overlaym, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                         nullptr, overlaystride*ccf);
    }

    modelcount = static_cast<uint32_t>(model.indices.size());
    if (modelcount > 0){
        createHostBuffer(modelv, modelvm, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                         model.vertices.data(), model.vertices.size() * sizeof(modelvertex));
        createHostBuffer(modeli, modelim, VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                         model.indices.data(), model.indices.size() * sizeof(uint32_t));
        qDebug("model vertices: %zu, triangles: %u", model.vertices.size(), modelcount / 3);
    }

    VkVertexInputBindingDescription vertexb[] ={
        { 0, sizeof(atomertex), VK_VERTEX_INPUT_RATE_VERTEX },
        { 1, sizeof(insdata), VK_VERTEX_INPUT_RATE_INSTANCE }
    };
    VkVertexInputAttributeDescription vertexa[] ={
        {
            0, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(atomertex, corner)
        },
        {
            1, 1, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(insdata, position)
        },
        {
            2, 1, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(insdata, colour)
        },
        {
            3, 1, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(insdata, end)
        }
    };

    VkPipelineVertexInputStateCreateInfo verstappen;
    verstappen.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    verstappen.pNext = nullptr;
    verstappen.flags = 0;
    verstappen.vertexBindingDescriptionCount = 2;
    verstappen.pVertexBindingDescriptions = vertexb;
    verstappen.vertexAttributeDescriptionCount = 4;
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
        0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1,
        VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, nullptr
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
    georgerussel.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
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
    piastri.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
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
    if (modelcount > 0){
        VkShaderModule modelvert = createShader(QStringLiteral("../shaders/modelvertex.spv"));
        VkShaderModule modelfrag = createShader(QStringLiteral("../shaders/modelfragment.spv"));
        VkPipelineShaderStageCreateInfo modelshaders[2] = {
            {VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO, nullptr, 0,
             VK_SHADER_STAGE_VERTEX_BIT, modelvert, "main", nullptr},
            {VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO, nullptr, 0,
             VK_SHADER_STAGE_FRAGMENT_BIT, modelfrag, "main", nullptr}
        };
        VkVertexInputBindingDescription modelbinding{0, sizeof(modelvertex), VK_VERTEX_INPUT_RATE_VERTEX};
        VkVertexInputAttributeDescription modelactoress [] = {
            {0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(modelvertex, position)},
            {1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(modelvertex, normal)},
            {2, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(modelvertex, colour)}
        };
        VkPipelineVertexInputStateCreateInfo modelinput{};
        modelinput.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        modelinput.vertexBindingDescriptionCount = 1;
        modelinput.pVertexBindingDescriptions = &modelbinding;
        modelinput.vertexAttributeDescriptionCount = 3;
        modelinput.pVertexAttributeDescriptions = modelactoress ;
        pipelinec.pStages = modelshaders;
        pipelinec.pVertexInputState = &modelinput;
        result = devicef->vkCreateGraphicsPipelines(device, pipeche, 1, &pipelinec, nullptr, &modelpipe);
        if (result != VK_SUCCESS) qFatal("model graphics pipeline failed %d", result);
        devicef->vkDestroyShaderModule(device, modelvert, nullptr);
        devicef->vkDestroyShaderModule(device, modelfrag, nullptr);
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
    DirectX::XMMATRIX projectile = DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV2, aspect, 0.01f, farplane);
    DirectX::XMFLOAT4X4 projectiled;
    DirectX::XMStoreFloat4x4(&projectiled, projectile);

    projectiled.m[1][1] *= -1.0f;
    projm = projectiled;
}

void atomizerer::releaseResources(){
    qDebug("releaseResources");

    VkDevice device = windows->device();
    if (modelpipe){
        devicef->vkDestroyPipeline(device, modelpipe, nullptr);
        modelpipe = VK_NULL_HANDLE;
    }
    if (modelv){ devicef->vkDestroyBuffer(device, modelv, nullptr); modelv = VK_NULL_HANDLE; }
    if (modelvm){ devicef->vkFreeMemory(device, modelvm, nullptr); modelvm = VK_NULL_HANDLE; }
    if (modeli){ devicef->vkDestroyBuffer(device, modeli, nullptr); modeli = VK_NULL_HANDLE; }
    if (modelim){ devicef->vkFreeMemory(device, modelim, nullptr); modelim = VK_NULL_HANDLE; }
    if (overlayb){ devicef->vkDestroyBuffer(device, overlayb, nullptr); overlayb = VK_NULL_HANDLE; }
    if (overlaym){ devicef->vkFreeMemory(device, overlaym, nullptr); overlaym = VK_NULL_HANDLE; }
    if (pipelane){
        devicef->vkDestroyPipeline(device, pipelane, nullptr);
        pipelane = VK_NULL_HANDLE;
    }
    if (pipeche){
        devicef->vkDestroyPipelineCache(device, pipeche, nullptr);
        pipeche = VK_NULL_HANDLE;
    }
    if (pipeout){
        devicef->vkDestroyPipelineLayout(device, pipeout, nullptr);
        pipeout = VK_NULL_HANDLE;
    }
    if (layer){
        devicef->vkDestroyDescriptorSetLayout(device, layer, nullptr);
        layer = VK_NULL_HANDLE;
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
    if (ibuffer){
        devicef->vkDestroyBuffer(device, ibuffer, nullptr);
        ibuffer = VK_NULL_HANDLE;
    }
    if (ibufferm){
        devicef->vkFreeMemory(device, ibufferm, nullptr);
        ibufferm = VK_NULL_HANDLE;
    }
    if (atomb){
        devicef->vkDestroyBuffer(device, atomb, nullptr);
        atomb = VK_NULL_HANDLE;
    }
    if (atomdm){
        devicef->vkFreeMemory(device, atomdm, nullptr);
        atomdm = VK_NULL_HANDLE;
    }
}

void atomizerer::getUniforms(renderuniforms *uniforms){
    QMutexLocker locker(&mutexgui);
    uniforms->view = cam.matrix();
    uniforms->projection = projm;
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

    renderuniforms uniforms;
    getUniforms(&uniforms);
    memcpy(pointer, &uniforms, sizeof(renderuniforms));

    devicef->vkUnmapMemory(device, bufferm);

    devicef->vkCmdBindPipeline(piastry, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelane);
    devicef->vkCmdBindDescriptorSets(piastry, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeout, 0, 1, 
    &layers[windows->currentFrame()], 0, nullptr);

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

    int cmode;
    int cpiece;
    std::vector<insdata> frameoverlay;
    {
        QMutexLocker locker(&mutexgui);
        cmode = mode;
        cpiece = selected_piece;
        if (cmode == 4) frameoverlay = overlay;
    }
    if (cmode == 4 && modelcount > 0 && modelpipe){
        devicef->vkCmdBindPipeline(piastry, VK_PIPELINE_BIND_POINT_GRAPHICS, modelpipe);
        VkDeviceSize offset = 0;
        devicef->vkCmdBindVertexBuffers(piastry, 0, 1, &modelv, &offset);
        devicef->vkCmdBindIndexBuffer(piastry, modeli, 0, VK_INDEX_TYPE_UINT32);
        if (cpiece >= 0 && cpiece < static_cast<int>(model.pieces.size()) &&
            model.pieces[cpiece].type != shape::coil){
            const auto &piece = model.pieces[cpiece];
            if (piece.firstindex > 0)
                devicef->vkCmdDrawIndexed(piastry, piece.firstindex, 1, 0, 0, 0);
            const uint32_t next = piece.firstindex + piece.countindex;
            if (next < modelcount)
                devicef->vkCmdDrawIndexed(piastry, modelcount-next, 1, next, 0, 0);
        }else{
            devicef->vkCmdDrawIndexed(piastry, modelcount, 1, 0, 0, 0);
        }
        if (!frameoverlay.empty() && overlayb){
            const VkDeviceSize bytes = frameoverlay.size()*sizeof(insdata);
            if (bytes <= overlaystride){
                const VkDeviceSize offset = windows->currentFrame()*overlaystride;
                void *mapped = nullptr;
                VkResult result = devicef->vkMapMemory(device, overlaym, offset, bytes, 0, &mapped);
                if (result != VK_SUCCESS) qFatal("overlay map failed %d", result);
                memcpy(mapped, frameoverlay.data(), bytes);
                devicef->vkUnmapMemory(device, overlaym);
                devicef->vkCmdBindPipeline(piastry, VK_PIPELINE_BIND_POINT_GRAPHICS, pipelane);
                VkBuffer vertexbuffers[] = {buffer, overlayb};
                VkDeviceSize offsets[] = {0, offset};
                devicef->vkCmdBindVertexBuffers(piastry, 0, 2, vertexbuffers, offsets);
                devicef->vkCmdBindIndexBuffer(piastry, ibuffer, 0, VK_INDEX_TYPE_UINT32);
                devicef->vkCmdDrawIndexed(piastry, indexc, static_cast<uint32_t>(frameoverlay.size()), 0, 0, 0);
            }
        }
    }else if (acount > 0 && atomb){
        VkBuffer vertexbuffers[] = { buffer, atomb };
        VkDeviceSize offsets[] = { 0, 0 };
        devicef->vkCmdBindVertexBuffers(piastry, 0, 2, vertexbuffers, offsets);
        devicef->vkCmdBindIndexBuffer(piastry, ibuffer, 0, VK_INDEX_TYPE_UINT32);
        const uint32_t first = cmode == 2 ? acount : 0;
        devicef->vkCmdDrawIndexed(piastry, indexc, acount, 0, 0, first);
        if (cmode == 1 && bcount > 0)
            devicef->vkCmdDrawIndexed(piastry, indexc, bcount, 0, 0, acount * 2);
    }
    devicef->vkCmdEndRenderPass(commandblock);
    windows->frameReady();
}

void atomizerer::look(float ydelta, float pdelta){
    {
        QMutexLocker locker(&mutexgui);
        cam.look(ydelta, pdelta);
    }
    requestFrame();
}

void atomizerer::setMode(int value){
    if (value < 1 || value > 4) return;
    {
        QMutexLocker locker(&mutexgui);
        mode = value;
    }
    requestFrame();
}

void atomizerer::pick(int x, int y, int width, int height){
    if (width <= 0 || height <= 0) return;
    float origin[3], direction[3];
    int cmode;
    {
        QMutexLocker locker(&mutexgui);
        cmode = mode;
        const float nx = 2.0f * (x+0.5f) / width - 1.0f;
        const float ny = 1.0f - 2.0f * (y+0.5f)/height;
        cam.ray(nx, ny, float(width) / height, origin, direction);
    }

    if (cmode != 4){
        const atomhit hit = patom(atoms, origin, direction, cmode == 2);
        if (hit.index >= 0){
            const std::string details = atomdescription(atoms[hit.index]);
            qInfo().noquote() << QString::fromStdString(details);
            windows->setTitle(QString::fromStdString(details));
        }else{
            windows->setTitle(QStringLiteral("Atomizer"));
        }
        return;
    }
    if (model.pieces.empty()) return;

    const modelhit hit = pmodel(model, origin, direction);
    std::vector<insdata> nextoverlay;
    if (hit.piece >= 0){
        const auto &piece = model.pieces[hit.piece];
        int first = piece.firstresidue;
        int last = piece.lastresidue;
        if (piece.type == shape::coil){
            float nearest = std::numeric_limits<float>::max();
            int center = first;
            for (const auto &a : atoms){
                if (std::strcmp(a.chain, piece.chain.c_str()) != 0 ||
                    a.s < first || a.s > last || std::strcmp(a.d, "CA") != 0) continue;
                const float dx = a.x-hit.position[0], dy = a.y-hit.position[1], dz = a.z-hit.position[2];
                const float distance = dx*dx+dy*dy+dz*dz;
                if (distance < nearest){ nearest = distance; center = a.s; }
            }
            first = std::max(first, center-2);
            last = std::min(last, center+2);
        }

        std::vector<atom> nearby;
        std::vector<int> remap(atoms.size(), -1);
        for (size_t i = 0; i < atoms.size(); ++i){
            const atom &a = atoms[i];
            if (std::strcmp(a.chain, piece.chain.c_str()) == 0 && a.s >= first && a.s <= last &&
                std::strcmp(a.t, "ATOM") == 0){
                remap[i] = static_cast<int>(nearby.size());
                nearby.push_back(a);
            }
        }

        std::vector<bond> nearbonds;
        for (const bond &item : bonds){
            if (item.first >= remap.size() || item.second >= remap.size()) continue;
            const int firstatom = remap[item.first];
            const int secondatom = remap[item.second];
            if (firstatom < 0 || secondatom < 0) continue;
            nearbonds.push_back({static_cast<uint32_t>(firstatom),
                                 static_cast<uint32_t>(secondatom), item.order});
        }

        nextoverlay = make_atom_instances(nearby, false);
        auto links = makebondinstances(nearby, nearbonds);
        nextoverlay.insert(nextoverlay.end(), links.begin(), links.end());
    }
    {
        QMutexLocker locker(&mutexgui);
        if (hit.piece == selected_piece){
            selected_piece = -1;
            overlay.clear();
        }else{
            selected_piece = hit.piece;
            overlay = std::move(nextoverlay);
        }
        if (selected_piece >= 0){
            const auto &piece = model.pieces[selected_piece];
            qDebug("selected chain %s, residues %d-%d", piece.chain.c_str(),
                   piece.firstresidue, piece.lastresidue);
        }
    }
    requestFrame();
}

void atomizerer::move(float famount, float samount, float vamount, float seconds, bool fast){
    const float distance = movspeed * seconds * (fast ? 4.0f : 1.0f);
    const float dirlength = std::sqrt(
        famount * famount + samount * samount + vamount * vamount
    );
    if (dirlength > 0.0f){
        famount /= dirlength;
        samount /= dirlength;
        vamount /= dirlength;
    }
    {
        QMutexLocker locker(&mutexgui);
        cam.move(famount * distance, samount * distance, vamount * distance);
    }
    requestFrame();
}