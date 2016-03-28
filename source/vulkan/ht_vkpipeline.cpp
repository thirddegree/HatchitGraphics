/**
**    Hatchit Engine
**    Copyright(c) 2015 Third-Degree
**
**    GNU Lesser General Public License
**    This file may be used under the terms of the GNU Lesser
**    General Public License version 3 as published by the Free
**    Software Foundation and appearing in the file LICENSE.LGPLv3 included
**    in the packaging of this file. Please review the following information
**    to ensure the GNU Lesser General Public License requirements
**    will be met: https://www.gnu.org/licenses/lgpl.html
**
**/

#include <ht_vkpipeline.h>
#include <ht_vkrenderer.h>

#include <cassert>

namespace Hatchit {

    namespace Graphics {

        namespace Vulkan {

            VKPipeline::VKPipeline(const VkRenderPass* renderPass) { m_renderPass = renderPass; }
            VKPipeline::~VKPipeline() 
            {
                VKRenderer* renderer = VKRenderer::RendererInstance;

                VkDevice device = renderer->GetVKDevice();

                vkDestroyPipeline(device, m_pipeline, nullptr);
                vkDestroyPipelineLayout(device, m_pipelineLayout, nullptr);
                vkDestroyPipelineCache(device, m_pipelineCache, nullptr);

                for (size_t i = 0; i < m_descriptorSetLayouts.size(); i++)
                    vkDestroyDescriptorSetLayout(device, m_descriptorSetLayouts[i], nullptr);
            }

			bool VKPipeline::VInitFromFile(File* file)
			{
				nlohmann::json json;
				std::ifstream jsonStream(file->Path());

				if (jsonStream.is_open())
				{
					jsonStream >> json;
					JsonExtractGuid(json, "GUID", m_guid);

					// TO-DO Add Shader extraction from JSON file

					// Get Shader Variables
					nlohmann::json shaderVariables = json["ShaderVariables"];
					for (int i = 0; i < shaderVariables.size(); i++)
					{
						std::string name;
						std::string type;
						JsonExtractString(shaderVariables[i], "Name", name);
						JsonExtractString(shaderVariables[i], "Type", type);

						if (type == "INT")
						{
							int64_t value;
							JsonExtractInt64(shaderVariables[i], "Value", value);
							m_shaderVariables[name] = new IntVariable(value);
						}
						else if (type == "FLOAT")
						{
							float value = 0.0f;
							JsonExtractFloat(shaderVariables[i], "Value", value);
							m_shaderVariables[name] = new FloatVariable(value);
						}
						else if (type == "DOUBLE")
						{
							double value = 0.0f;
							JsonExtractDouble(shaderVariables[i], "Value", value);
							m_shaderVariables[name] = new DoubleVariable(value);
						}
					}

					// Get Rasterizer state
					nlohmann::json json_rasterState = json["RasterState"];
					RasterizerState rasterState {};
					std::string polygonModeStr;
					std::string cullModeStr;

					JsonExtractString(json_rasterState, "PolygonMode", polygonModeStr);
					JsonExtractString(json_rasterState, "CullMode", cullModeStr);
					JsonExtractBool(json_rasterState, "FrontCounterClockwise", rasterState.frontCounterClockwise);
					JsonExtractBool(json_rasterState, "DepthClampEnable", rasterState.depthClampEnable);

					// Polygon mode
					if (polygonModeStr == "SOLID")
						rasterState.polygonMode = PolygonMode::SOLID;
					else if (polygonModeStr == "LINE")
						rasterState.polygonMode = PolygonMode::LINE;

					// Cull mode
					if (cullModeStr == "NONE")
						rasterState.cullMode = CullMode::NONE;
					else if (cullModeStr == "FRONT")
						rasterState.cullMode = CullMode::FRONT;
					else if (cullModeStr == "BACK")
						rasterState.cullMode = CullMode::BACK;

					// Get Multisampler state
					nlohmann::json json_multisampleState = json["MultisampleState"];
					MultisampleState multisampleState {};
					uint32_t sampleCount;

					JsonExtractUInt32(json_multisampleState, "SampleCount", sampleCount);
					JsonExtractFloat(json_multisampleState, "MinSamples", multisampleState.minSamples);
					JsonExtractBool(json_multisampleState, "PerSampleShading", multisampleState.perSampleShading);

					// Sample count
					switch (sampleCount)
					{
					case 1:
						multisampleState.samples = SAMPLE_1_BIT;
						break;
					case 2:
						multisampleState.samples = SAMPLE_2_BIT;
						break;
					case 4:
						multisampleState.samples = SAMPLE_4_BIT;
						break;
					case 8:
						multisampleState.samples = SAMPLE_8_BIT;
						break;
					case 16:
						multisampleState.samples = SAMPLE_16_BIT;
						break;
					case 32:
						multisampleState.samples = SAMPLE_32_BIT;
						break;
					case 64:
						multisampleState.samples = SAMPLE_64_BIT;
						break;
					}

					VSetRasterState(rasterState);
					VSetMultisampleState(multisampleState);

					jsonStream.close();
					return true;
				}

				DebugPrintF("ERROR: Could not generate stream to JSON file -> %s", file->Path());
				return false;
			}

            //If we wanted to allow users to control blending states
            //void VSetColorBlendAttachments(ColorBlendState* colorBlendStates) override;

            /* Set the rasterization state for this pipeline
            * \param rasterState A struct containing rasterization options
            */
            void VKPipeline::VSetRasterState(const RasterizerState& rasterState)
            {
                VkPolygonMode polyMode;
                VkCullModeFlagBits cullMode;

                switch (rasterState.polygonMode)
                {
                case PolygonMode::SOLID:
                    polyMode = VK_POLYGON_MODE_FILL;
                    break;
                case PolygonMode::LINE:
                    polyMode = VK_POLYGON_MODE_LINE;
                    break;
                default:
                    polyMode = VK_POLYGON_MODE_FILL;
                    break;
                }

                switch (rasterState.cullMode)
                {
                case NONE:
                    cullMode = VK_CULL_MODE_NONE;
                    break;
                case FRONT:
                    cullMode = VK_CULL_MODE_FRONT_BIT;
                    break;
                case BACK:
                    cullMode = VK_CULL_MODE_BACK_BIT;
                    break;
                }

                m_rasterizationState = {}; //default setup
                m_rasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
                m_rasterizationState.pNext = nullptr;
                m_rasterizationState.polygonMode = polyMode;
                m_rasterizationState.cullMode = cullMode;
                m_rasterizationState.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
                m_rasterizationState.depthClampEnable = rasterState.depthClampEnable;
                m_rasterizationState.rasterizerDiscardEnable = rasterState.discardEnable;
                m_rasterizationState.depthBiasEnable = VK_FALSE;
            }

            /* Set the multisampling state for this pipeline
            * \param multiState A struct containing multisampling options
            */
            void VKPipeline::VSetMultisampleState(const MultisampleState& multiState)
            {
                VkSampleCountFlagBits sampleCount = VK_SAMPLE_COUNT_1_BIT;

                switch (multiState.samples)
                {
                case SAMPLE_1_BIT:
                    sampleCount = VK_SAMPLE_COUNT_1_BIT;
                    break;
                case SAMPLE_2_BIT:
                    sampleCount = VK_SAMPLE_COUNT_2_BIT;
                    break;
                case SAMPLE_4_BIT:
                    sampleCount = VK_SAMPLE_COUNT_4_BIT;
                    break;
                case SAMPLE_8_BIT:
                    sampleCount = VK_SAMPLE_COUNT_8_BIT;
                    break;
                case SAMPLE_16_BIT:
                    sampleCount = VK_SAMPLE_COUNT_16_BIT;
                    break;
                case SAMPLE_32_BIT:
                    sampleCount = VK_SAMPLE_COUNT_32_BIT;
                    break;
                case SAMPLE_64_BIT:
                    sampleCount = VK_SAMPLE_COUNT_64_BIT;
                    break;
                }

                m_multisampleState = {};
                m_multisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
                m_multisampleState.pNext = nullptr;
                m_multisampleState.pSampleMask = nullptr;
                m_multisampleState.rasterizationSamples = sampleCount;
                m_multisampleState.sampleShadingEnable = multiState.perSampleShading;
                m_multisampleState.minSampleShading = multiState.minSamples;
            }

            /* Load a shader into a shader slot for the pipeline
            * \param shaderSlot The slot that you want the shader in; vertex, fragment etc.
            * \param shader A pointer to the shader that you want to load to the given shader slot
            */
            void VKPipeline::VLoadShader(ShaderSlot shaderSlot, IShader* shader)
            {
                VKShader* vkShader = (VKShader*)shader;

                VkShaderStageFlagBits shaderType;

                switch (shaderSlot)
                {
                case ShaderSlot::VERTEX:
                    shaderType = VK_SHADER_STAGE_VERTEX_BIT;
                    break;
                case ShaderSlot::FRAGMENT:
                    shaderType = VK_SHADER_STAGE_FRAGMENT_BIT;
                    break;
                case ShaderSlot::GEOMETRY:
                    shaderType = VK_SHADER_STAGE_GEOMETRY_BIT;
                    break;
                case ShaderSlot::TESS_CONTROL:
                    shaderType = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
                    break;
                case ShaderSlot::TESS_EVAL:
                    shaderType = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
                    break;
                case ShaderSlot::COMPUTE:
                    shaderType = VK_SHADER_STAGE_COMPUTE_BIT;
                    break;
                }

                VkPipelineShaderStageCreateInfo shaderStage = {};
                shaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
                shaderStage.stage = shaderType;
                shaderStage.module = vkShader->GetShaderModule();
                shaderStage.pName = "main";

                m_shaderStages.push_back(shaderStage);
            }

            bool VKPipeline::VSetInt(std::string name, int data)
            {
                //If the variable doesn't exist in the map lets allocate it
                //Otherwise lets just change its data
                std::map<std::string, ShaderVariable*>::iterator it = m_shaderVariables.find(name);
                if (it != m_shaderVariables.end())
                    static_cast<IntVariable*>(m_shaderVariables[name])->SetData(data);
                else
                    m_shaderVariables[name] = new IntVariable(data);

                return true;
            }
            bool VKPipeline::VSetFloat(std::string name, float data)
            {
                //If the variable doesn't exist in the map lets allocate it
                //Otherwise lets just change its data
                std::map<std::string, ShaderVariable*>::iterator it = m_shaderVariables.find(name);
                if (it != m_shaderVariables.end())
                    static_cast<FloatVariable*>(m_shaderVariables[name])->SetData(data);
                else
                    m_shaderVariables[name] = new FloatVariable(data);

                return true;
            }
            bool VKPipeline::VSetFloat3(std::string name, Math::Vector3 data)
            {
                //If the variable doesn't exist in the map lets allocate it
                //Otherwise lets just change its data
                std::map<std::string, ShaderVariable*>::iterator it = m_shaderVariables.find(name);
                if (it != m_shaderVariables.end())
                    static_cast<Float3Variable*>(m_shaderVariables[name])->SetData(data);
                else
                    m_shaderVariables[name] = new Float3Variable(data);

                return true;
            }
            bool VKPipeline::VSetFloat4(std::string name, Math::Vector4 data)
            {
                //If the variable doesn't exist in the map lets allocate it
                //Otherwise lets just change its data
                std::map<std::string, ShaderVariable*>::iterator it = m_shaderVariables.find(name);
                if (it != m_shaderVariables.end())
                    static_cast<Float4Variable*>(m_shaderVariables[name])->SetData(data);
                else
                    m_shaderVariables[name] = new Float4Variable(data);

                return true;
            }
            bool VKPipeline::VSetMatrix4(std::string name, Math::Matrix4 data)
            {
                //If the variable doesn't exist in the map lets allocate it
                //Otherwise lets just change its data
                std::map<std::string, ShaderVariable*>::iterator it = m_shaderVariables.find(name);
                if (it != m_shaderVariables.end())
                    static_cast<Matrix4Variable*>(m_shaderVariables[name])->SetData(data);
                else
                    m_shaderVariables[name] = new Matrix4Variable(data);
                
                return true;
            }

            ///Have Vulkan create a pipeline with these settings
            bool VKPipeline::VPrepare()
            {
                //Get the renderer for use later
                VKRenderer* renderer = VKRenderer::RendererInstance;

                //Get objects from renderer
                VkDevice device = renderer->GetVKDevice();
                VkDescriptorPool descriptorPool = renderer->GetVKDescriptorPool();

                if (!prepareLayouts(device))
                    return false;


                if (!preparePipeline(device))
                    return false;

                return true;
            }

            bool VKPipeline::VUpdate()
            {
                //TODO: Organize push constant data other than just matricies
                if (m_shaderVariables.size() == 0)
                    return true;

                std::vector<Math::Matrix4> variableList;

                std::map <std::string, ShaderVariable*>::iterator it;
                for (it = m_shaderVariables.begin(); it != m_shaderVariables.end(); it++)
                    variableList.push_back(*(Math::Matrix4*)(it->second->GetData()));

                m_matrixPushData.resize(variableList.size() * 16);

                for (size_t i = 0; i < variableList.size(); i++)
                {
                    Math::Matrix4 mat = variableList[i];
                    memcpy(m_matrixPushData.data() + (16 * i), static_cast<void*>(variableList[i].data), sizeof(float) * 16);
                }

                return true;
            }

            void VKPipeline::SetVKDescriptorSetLayout(VkDescriptorSetLayout descriptorSetLayout)
            {
                useGivenLayout = true;
                m_descriptorSetLayouts.push_back(descriptorSetLayout);
            }

            bool VKPipeline::prepareLayouts(VkDevice device)
            {
                VkResult err;

                if (!useGivenLayout)
                {
                    //TODO: Properly detect and setup layout bindings

                    std::vector<VkDescriptorSetLayoutBinding> perPassBindings;
                    std::vector<VkDescriptorSetLayoutBinding> perObjectBindings;

                    //Per pass binding point
                    VkDescriptorSetLayoutBinding perPassBinding = {};
                    perPassBinding.binding = 0;
                    perPassBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                    perPassBinding.descriptorCount = 1;
                    perPassBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
                    perPassBinding.pImmutableSamplers = nullptr;

                    perPassBindings.push_back(perPassBinding);

                    //Per model binding points
                    VkDescriptorSetLayoutBinding perObjectVSBinding = {};
                    perObjectVSBinding.binding = 0;
                    perObjectVSBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                    perObjectVSBinding.descriptorCount = 1;
                    perObjectVSBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
                    perObjectVSBinding.pImmutableSamplers = nullptr;

                    VkDescriptorSetLayoutBinding perObjectFSBinding = {};
                    perObjectFSBinding.binding = 1;
                    perObjectFSBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                    perObjectFSBinding.descriptorCount = 1;
                    perObjectFSBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
                    perObjectFSBinding.pImmutableSamplers = nullptr;

                    perObjectBindings.push_back(perObjectVSBinding);
                    perObjectBindings.push_back(perObjectFSBinding);

                    VkDescriptorSetLayoutCreateInfo perPassDescriptorLayoutInfo = {};
                    perPassDescriptorLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
                    perPassDescriptorLayoutInfo.pNext = NULL;
                    perPassDescriptorLayoutInfo.bindingCount = static_cast<uint32_t>(perPassBindings.size());
                    perPassDescriptorLayoutInfo.pBindings = perPassBindings.data();

                    VkDescriptorSetLayoutCreateInfo perObjectDescriptorLayoutInfo = {};
                    perObjectDescriptorLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
                    perObjectDescriptorLayoutInfo.pNext = NULL;
                    perObjectDescriptorLayoutInfo.bindingCount = static_cast<uint32_t>(perObjectBindings.size());
                    perObjectDescriptorLayoutInfo.pBindings = perObjectBindings.data();

                    m_descriptorSetLayouts.resize(2);

                    err = vkCreateDescriptorSetLayout(device, &perPassDescriptorLayoutInfo, nullptr, &m_descriptorSetLayouts[0]);
                    assert(!err);
                    if (err != VK_SUCCESS)
                    {
#ifdef _DEBUG
                        Core::DebugPrintF("VKRenderer::preparePipeline(): Failed to create descriptor layout\n");
#endif
                        return false;
                    }

                    err = vkCreateDescriptorSetLayout(device, &perObjectDescriptorLayoutInfo, nullptr, &m_descriptorSetLayouts[1]);
                    assert(!err);
                    if (err != VK_SUCCESS)
                    {
#ifdef _DEBUG
                        Core::DebugPrintF("VKRenderer::preparePipeline(): Failed to create descriptor layout\n");
#endif
                        return false;
                    }
                }

                uint32_t matSize = 16 * sizeof(float);

                std::vector<VkPushConstantRange> pushConstantRanges;
                pushConstantRanges.resize(2);

                pushConstantRanges[0].offset = 0;
                pushConstantRanges[0].size = matSize;
                pushConstantRanges[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

                pushConstantRanges[1].offset = matSize;
                pushConstantRanges[1].size = matSize;
                pushConstantRanges[1].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

                //Pipeline layout 
                VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
                pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
                pipelineLayoutInfo.pNext = nullptr;
                pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(m_descriptorSetLayouts.size());
                pipelineLayoutInfo.pSetLayouts = m_descriptorSetLayouts.data();
                pipelineLayoutInfo.pushConstantRangeCount = static_cast<uint32_t>(pushConstantRanges.size());
                pipelineLayoutInfo.pPushConstantRanges = pushConstantRanges.data();

                err = vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &m_pipelineLayout);
                assert(!err);
                if (err != VK_SUCCESS)
                {
#ifdef _DEBUG
                    Core::DebugPrintF("VKRenderer::preparePipeline(): Failed to create pipeline layout\n");
#endif
                    return false;
                }

                return true;
            }

            bool VKPipeline::preparePipeline(VkDevice device)
            {
                VkResult err;

                //Vertex info state
                VkVertexInputBindingDescription vertexBindingDescriptions[1] = {};

                vertexBindingDescriptions[0] = {};
                vertexBindingDescriptions[0].binding = 0;
                vertexBindingDescriptions[0].stride = sizeof(Vertex);
                vertexBindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

                VkVertexInputAttributeDescription vertexAttributeDescriptions[3] = {};

                vertexAttributeDescriptions[0] = {};
                vertexAttributeDescriptions[0].binding = 0;
                vertexAttributeDescriptions[0].location = 0;
                vertexAttributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
                vertexAttributeDescriptions[0].offset = 0;

                vertexAttributeDescriptions[1] = {};
                vertexAttributeDescriptions[1].binding = 0;
                vertexAttributeDescriptions[1].location = 1;
                vertexAttributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
                vertexAttributeDescriptions[1].offset = sizeof(float) * 3;

                vertexAttributeDescriptions[2] = {};
                vertexAttributeDescriptions[2].binding = 0;
                vertexAttributeDescriptions[2].location = 2;
                vertexAttributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
                vertexAttributeDescriptions[2].offset = sizeof(float) * 6;

                VkPipelineVertexInputStateCreateInfo vertexInputState = {};
                vertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
                vertexInputState.pNext = nullptr;
                vertexInputState.vertexBindingDescriptionCount = 1;
                vertexInputState.vertexAttributeDescriptionCount = 3;
                vertexInputState.pVertexBindingDescriptions = vertexBindingDescriptions;
                vertexInputState.pVertexAttributeDescriptions = vertexAttributeDescriptions;

                //Topology
                VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = {};
                inputAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
                inputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

                //Only one blend attachment state; not using blend right now
                VkPipelineColorBlendAttachmentState blendAttachmentState[1] = {};
                blendAttachmentState[0].colorWriteMask = 0xf;
                blendAttachmentState[0].blendEnable = VK_FALSE;

                //Color blends and masks
                VkPipelineColorBlendStateCreateInfo colorBlendState = {};
                colorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
                colorBlendState.pAttachments = blendAttachmentState;
                colorBlendState.attachmentCount = 1;

                //Viewport
                VkPipelineViewportStateCreateInfo viewportState = {};
                viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
                viewportState.viewportCount = 1;
                viewportState.scissorCount = 1;

                //Enable dynamic states
                VkDynamicState dynamicStateEnables[] = { VK_DYNAMIC_STATE_VIEWPORT , VK_DYNAMIC_STATE_SCISSOR };

                VkPipelineDynamicStateCreateInfo dynamicState = {};
                dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
                dynamicState.pNext = nullptr;
                dynamicState.pDynamicStates = dynamicStateEnables;
                dynamicState.dynamicStateCount = 2;

                //Depth and stencil states
                VkPipelineDepthStencilStateCreateInfo depthStencilState = {};
                depthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
                depthStencilState.pNext = nullptr;
                depthStencilState.depthTestEnable = VK_TRUE;
                depthStencilState.depthWriteEnable = VK_TRUE;
                depthStencilState.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
                depthStencilState.depthBoundsTestEnable = VK_FALSE;
                depthStencilState.back.failOp = VK_STENCIL_OP_KEEP;
                depthStencilState.back.passOp = VK_STENCIL_OP_KEEP;
                depthStencilState.back.compareOp = VK_COMPARE_OP_ALWAYS;
                depthStencilState.stencilTestEnable = VK_FALSE;
                depthStencilState.front = depthStencilState.back;
                depthStencilState.front.compareOp = VK_COMPARE_OP_NEVER;

                //Finalize pipeline
                VkGraphicsPipelineCreateInfo pipelineInfo = {};
                pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
                pipelineInfo.layout = m_pipelineLayout;
                pipelineInfo.renderPass = *m_renderPass;
                pipelineInfo.stageCount = static_cast<uint32_t>(m_shaderStages.size());
                pipelineInfo.pVertexInputState = &vertexInputState;
                pipelineInfo.pInputAssemblyState = &inputAssemblyState;
                pipelineInfo.pRasterizationState = &m_rasterizationState;
                pipelineInfo.pColorBlendState = &colorBlendState;
                pipelineInfo.pMultisampleState = &m_multisampleState;
                pipelineInfo.pViewportState = &viewportState;
                pipelineInfo.pDepthStencilState = &depthStencilState;
                pipelineInfo.pStages = m_shaderStages.data();
                pipelineInfo.pDynamicState = &dynamicState;

                VkPipelineCacheCreateInfo pipelineCacheInfo = {};
                pipelineCacheInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;

                err = vkCreatePipelineCache(device, &pipelineCacheInfo, nullptr, &m_pipelineCache);
                assert(!err);
                if (err != VK_SUCCESS)
                {
#ifdef _DEBUG
                    Core::DebugPrintF("VKRenderer::preparePipeline(): Failed to create pipeline cache\n");
#endif
                    return false;
                }

                err = vkCreateGraphicsPipelines(device, m_pipelineCache, 1, &pipelineInfo, nullptr, &m_pipeline);
                assert(!err);
                if (err != VK_SUCCESS)
                {
#ifdef _DEBUG
                    Core::DebugPrintF("VKRenderer::preparePipeline(): Failed to create pipeline\n");
#endif
                    return false;
                }

                //At this point we can unload shader modules that don't need to be sent to anymore pipelines

                return true;
            }

            VkPipeline                          VKPipeline::GetVKPipeline()             { return m_pipeline; }
            VkPipelineLayout                    VKPipeline::GetVKPipelineLayout()       { return m_pipelineLayout; }
            std::vector<VkDescriptorSetLayout>  VKPipeline::GetVKDescriptorSetLayouts() { return m_descriptorSetLayouts; }

            void VKPipeline::SendPushConstants(VkCommandBuffer commandBuffer)
            {
                //Send a push for each type of data to send; vectors, matricies, ints etc.
                uint32_t dataSize = static_cast<uint32_t>(m_matrixPushData.size() * sizeof(float));
                vkCmdPushConstants(commandBuffer, m_pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, dataSize, m_matrixPushData.data());
            }
        }
    }
}
