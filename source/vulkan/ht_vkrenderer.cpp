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

#include <ht_vkmesh.h>
#include <ht_vkrenderer.h>
#include <ht_vkshader.h>
#include <ht_vkpipeline.h>
#include <ht_vkmaterial.h>
#include <ht_vksampler.h>
#include <ht_vktexture.h>
#include <ht_vkmeshrenderer.h>
#include <ht_vkrendertarget.h>
#include <ht_debug.h>
#include <ht_scheduler.h>

#include <cassert>

#ifdef HT_SYS_WINDOWS
#include <windows.h>
#endif

#ifdef HT_SYS_LINUX
#include <xcb/xcb.h>
#include <xcb/xproto.h>
#endif

namespace Hatchit {

    namespace Graphics {

        namespace Vulkan {

            VKRenderer* VKRenderer::RendererInstance = nullptr;

            VKRenderer::VKRenderer()
            {
                m_setupCommandBuffer = 0;

                m_swapchain = nullptr;

                m_renderTarget = nullptr;
                m_texture = nullptr;
                m_sampler = nullptr;

                m_instance = VK_NULL_HANDLE;

                m_device = VK_NULL_HANDLE;
                m_commandPool = VK_NULL_HANDLE;
                m_descriptorPool = VK_NULL_HANDLE;

                m_renderSemaphore = VK_NULL_HANDLE;
                m_presentSemaphore = VK_NULL_HANDLE;
            }

            VKRenderer::~VKRenderer()
            {
            }

            bool VKRenderer::VInitialize(const RendererParams & params)
            {
                m_clearColor.color.float32[0] = params.clearColor.r;
                m_clearColor.color.float32[1] = params.clearColor.g;
                m_clearColor.color.float32[2] = params.clearColor.b;
                m_clearColor.color.float32[3] = params.clearColor.a;

                /*
                * Initialize Core Vulkan Systems: Driver layers & extensions 
                */
                if (!initVulkan(params))
                    return false;
                            
                /*
                * Initialize Vulkan swapchain
                */
                if (!initVulkanSwapchain(params))
                    return false;

                //We should be able to use the device and instance wherever we want at this point
                if (RendererInstance == nullptr)
                    RendererInstance = this;

                /*
                * Prepare Vulkan command buffers and memory systems for drawing
                */
                if (!prepareVulkan())
                    return false;

                return true;
            }

            void VKRenderer::VDeInitialize()
            {
                m_queueProps.clear();

                if (m_swapchain != nullptr)
                {
                    delete m_swapchain;
                    vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
                }

                //These should all be deleted elsewhere when resources work properly
                if(m_renderTarget != nullptr)
                    delete m_renderTarget;
                if (m_texture != nullptr)
                    delete m_texture;
                if (m_sampler != nullptr)
                    delete m_sampler;
                
                std::map<IPipeline*, std::vector<Renderable>>::iterator it;
                for (it = m_pipelineList.begin(); it != m_pipelineList.end(); it++)
                {
                    delete it->first;

                    std::vector<Renderable> renderables = it->second;

                    for (uint32_t i = 0; i < renderables.size(); i++)
                    {
                        Renderable r = renderables[i];

                        if (r.material != nullptr)
                        {
                            delete r.material;
                            r.material = nullptr;
                        }

                        if (r.mesh != nullptr)
                        {
                            delete r.mesh;
                            r.mesh = nullptr;
                        }
                    }
                    it->second.clear();
                }
                m_pipelineList.clear();


                for (uint32_t i = 0; i < m_renderPasses.size(); i++)
                    delete m_renderPasses[i];
                
                m_renderPasses.clear();

                if (m_device != VK_NULL_HANDLE)
                {
                    if (m_presentSemaphore != VK_NULL_HANDLE)
                        vkDestroySemaphore(m_device, m_presentSemaphore, nullptr);
                    if (m_renderSemaphore != VK_NULL_HANDLE)
                        vkDestroySemaphore(m_device, m_renderSemaphore, nullptr);

                    if (m_commandPool != VK_NULL_HANDLE)
                        vkDestroyCommandPool(m_device, m_commandPool, nullptr);
                    if (m_descriptorPool != VK_NULL_HANDLE)
                        vkDestroyDescriptorPool(m_device, m_descriptorPool, nullptr);

                    vkDestroyDevice(m_device, nullptr);
                }

                if (m_instance != VK_NULL_HANDLE)
                {
                    if(msg_callback != VK_NULL_HANDLE)
                        m_destroyDebugReportCallback(m_instance, msg_callback, nullptr);

                    vkDestroyInstance(m_instance, nullptr);
                }
            }

            void VKRenderer::VResizeBuffers(uint32_t width, uint32_t height)
            {
                //Recreate the swapchain
                m_width = width;
                m_height = height;
                prepareVulkan();
            }

            void VKRenderer::VSetClearColor(const Color & color)
            {
                
            }

            void VKRenderer::VClearBuffer(ClearArgs args)
            {
                VkResult err;
                
                VkFence nullFence = VK_NULL_HANDLE;

                //Get the next image to draw on
                //TODO: Actually use fences
                err = m_swapchain->VKGetNextImage(m_presentSemaphore);
                if (err == VK_ERROR_OUT_OF_DATE_KHR)
                {
                    //Resize!
                    VResizeBuffers(m_width, m_height); //TODO: find a better way to resize
                    return;
                }
                else if (err == VK_SUBOPTIMAL_KHR) 
                {
                    // demo->swapchain is not as optimal as it could be, but the platform's
                    // presentation engine will still present the image correctly.
                }
                else
                {
                    assert(!err);
                }
            }

            void VKRenderer::VRender(float dt) 
            {
                //TODO: Determine which physical device and thread are best to render with

                m_swapchain->BuildSwapchain(m_clearColor);

                bool success = m_swapchain->VKPostPresentBarrier(m_queue);
                assert(success);

                //Get list of command buffers
                std::vector<VkCommandBuffer> commandBuffers;

                for (size_t i = 0; i < m_renderPasses.size(); i++)
                {
                    VKRenderPass* renderPass = static_cast<VKRenderPass*>(m_renderPasses[i]);

                    renderPass->VBuildCommandList();
                    commandBuffers.push_back(renderPass->GetVkCommandBuffer());
                }

                //Make sure we run the swapchain command
                commandBuffers.push_back(m_swapchain->GetCurrentCommand());

                //Example code for rotation
                Math::Matrix4 scale = Math::MMMatrixScale(Math::Vector3(0.02f, 0.02f, 0.02f));
                Math::Matrix4 rot = Math::MMMatrixRotationXYZ(Math::Vector3(0, m_angle += dt, 0));
                Math::Matrix4 trans = Math::MMMatrixTranslation(Math::Vector3(0, -30, 0));
                Math::Matrix4 mat = Math::MMMatrixTranspose(rot * (scale * trans));

                m_material->VSetMatrix4("object.model", mat);
                m_material->VUpdate();

                VkResult err;

                m_submitInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());
                m_submitInfo.pCommandBuffers = commandBuffers.data();

                err = vkQueueSubmit(m_queue, 1, &m_submitInfo, VK_NULL_HANDLE);
                assert(!err);

                success = m_swapchain->VKPrePresentBarrier(m_queue);
                assert(success);
            }

            void VKRenderer::VPresent()
            {
                VkResult err;

                err = m_swapchain->VKPresent(m_queue, m_renderSemaphore);
                if (err == VK_ERROR_OUT_OF_DATE_KHR)
                    VResizeBuffers(m_width, m_height);
                else if (err == VK_SUBOPTIMAL_KHR)
                {
                    //Swapchain is not as optimal as it could be
                }
                else
                    assert(!err);

                err = vkQueueWaitIdle(m_queue);
                assert(!err);

                //Reset command buffer memory back to this command pool
                vkResetCommandPool(m_device, m_commandPool, 0);
            }

            VkPhysicalDevice VKRenderer::GetVKPhysicalDevice() 
            {
                return m_gpu;
            }

            VkDevice VKRenderer::GetVKDevice() 
            {
                return m_device;
            }

            VkInstance VKRenderer::GetVKInstance() 
            {
                return m_instance;
            }

            VkCommandPool VKRenderer::GetVKCommandPool()
            {
                return m_commandPool;
            }

            VkDescriptorPool VKRenderer::GetVKDescriptorPool() 
            {
                return m_descriptorPool;
            }

            VkCommandBuffer VKRenderer::GetSetupCommandBuffer() 
            {
                return m_setupCommandBuffer;
            }

            VkFormat VKRenderer::GetPreferredImageFormat() 
            {
                return m_preferredImageFormat;
            }
            VkFormat VKRenderer::GetPreferredDepthFormat() 
            {
                return VK_FORMAT_D32_SFLOAT_S8_UINT;
            }

            bool VKRenderer::initVulkan(const RendererParams& params) 
            {
                VkResult err;
                bool success = true;
                /*
                * Check Vulkan instance layers
                */
                success = checkInstanceLayers();
                assert(success);
                if (!success)
                    return false;


                /*
                * Check Vulkan instance extensions
                */
                success = checkInstanceExtensions();
                assert(success);
                if (!success)
                    return false;

                /*
                * Setup Vulkan application info structure
                */

                m_appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
                m_appInfo.pNext = nullptr;
                m_appInfo.pApplicationName = params.applicationName.c_str();
                m_appInfo.applicationVersion = 0;
                m_appInfo.pEngineName = "Hatchit";
                m_appInfo.engineVersion = 0;
                m_appInfo.apiVersion = VK_API_VERSION;

                /*
                * Setup Vulkan instance create info
                */
                VkInstanceCreateInfo instanceInfo;
                instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
                instanceInfo.pNext = nullptr;
                instanceInfo.flags = 0;
                instanceInfo.pApplicationInfo = &m_appInfo;
                instanceInfo.enabledLayerCount = static_cast<uint32_t>(m_enabledLayerNames.size());
                instanceInfo.ppEnabledLayerNames = &m_enabledLayerNames[0];
                instanceInfo.enabledExtensionCount = static_cast<uint32_t>(m_enabledExtensionNames.size());
                instanceInfo.ppEnabledExtensionNames = &m_enabledExtensionNames[0];

                /**
                * Create Vulkan instance handle
                */
                err = vkCreateInstance(&instanceInfo, nullptr, &m_instance);
                switch (err)
                {
                case VK_SUCCESS:
                    break;

                case VK_ERROR_INCOMPATIBLE_DRIVER:
                {
#ifdef _DEBUG
                    Core::DebugPrintF("Cannot find a compatible Vulkan installable client driver"
                        "(ICD).\n\nPlease look at the Getting Started guide for "
                        "additional information.\n"
                        "vkCreateInstance Failure\n");
#endif
                } return false;

                case VK_ERROR_EXTENSION_NOT_PRESENT:
                {
                    //TODO: print something
                } return false;

                default:
                    return false;
                }

                /**
                *
                * Enumerate available GPU devices for use with Vulkan
                *
                */
                if (!enumeratePhysicalDevices())
                    return false;

                /*
                * Check layers that we want against the layers available on the device
                */
                if (!checkDeviceLayers())
                    return false;

                /*
                * Check extensions that we want against the extensions supported by the device
                */
                if (!checkDeviceExtensions())
                    return false;

                /*
                * Setup debug callbacks
                */
#ifdef _DEBUG
                if (!setupDebugCallbacks())
                    return false;
#endif

                /*
                * Device should be valid at this point, get device properties
                */
                if (!setupDeviceQueues())
                    return false;

                /*
                * Query the device for advanced feature support
                */
                if (!setupProcAddresses())
                    return false;

                return true;
            }

            bool VKRenderer::initVulkanSwapchain(const RendererParams& params)
            {
                VkResult err;

                //Hook into the window
#ifdef _WIN32
                //Get HINSTANCE from HWND
                HWND window = (HWND)params.window;
                HINSTANCE instance;
                instance = (HINSTANCE)GetWindowLongPtr(window, GWLP_HINSTANCE);

                VkWin32SurfaceCreateInfoKHR creationInfo;
                creationInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
                creationInfo.pNext = nullptr;
                creationInfo.flags = 0; //Unused in Vulkan 1.0.3;
                creationInfo.hinstance = instance;
                creationInfo.hwnd = window;

                err = vkCreateWin32SurfaceKHR(m_instance, &creationInfo, nullptr, &m_surface);

                if (err != VK_SUCCESS)
                {
#ifdef _DEBUG
                    Core::DebugPrintF("Error creating VkSurface for Win32 window");
#endif
                    return false;
                }
#endif

#ifdef HT_SYS_LINUX
                VkXcbSurfaceCreateInfoKHR creationInfo;
                creationInfo.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
                creationInfo.pNext = nullptr;
                creationInfo.flags = 0;
                creationInfo.connection = (xcb_connection_t*)params.display;
                creationInfo.window = *(uint32_t*)params.window;

                err = vkCreateXcbSurfaceKHR(m_instance, &creationInfo, nullptr, &m_surface);

                if(err != VK_SUCCESS)
                {
                    Core::DebugPrintF("Error creating VkSurface for Xcb window");
        
                    return false;
                }
#endif

                /*
                * Setup the device queues
                */
                if (!setupQueues())
                    return false;

                /*
                * Create the device object that is in charge of allocating memory and making draw calls
                */
                if (!createDevice())
                    return false;                

                //Get Device queue
                vkGetDeviceQueue(m_device, m_graphicsQueueNodeIndex, 0, &m_queue);

                /*
                * Get the supported texture format and color space
                */
                if (!getSupportedFormats())
                    return false;

                //Get memory information
                vkGetPhysicalDeviceMemoryProperties(m_gpu, &m_memoryProps);

                m_swapchain = new VKSwapchain(m_instance, m_gpu, m_device, m_commandPool);

                //Setup semaphores and submission info
                VkSemaphoreCreateInfo semaphoreCreateInfo = {};
                semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
                semaphoreCreateInfo.pNext = nullptr;
                semaphoreCreateInfo.flags = 0;

                err = vkCreateSemaphore(m_device, &semaphoreCreateInfo, nullptr, &m_presentSemaphore);
                assert(!err);

                err = vkCreateSemaphore(m_device, &semaphoreCreateInfo, nullptr, &m_renderSemaphore);
                assert(!err);

                VkPipelineStageFlags stageFlags = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;

                m_submitInfo = {};
                m_submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
                m_submitInfo.pWaitDstStageMask = &stageFlags;
                m_submitInfo.waitSemaphoreCount = 1;
                m_submitInfo.pWaitSemaphores = &m_presentSemaphore;
                m_submitInfo.signalSemaphoreCount = 1;
                m_submitInfo.pSignalSemaphores = &m_renderSemaphore;

                return true;
            }


            bool VKRenderer::checkInstanceLayers()
            {
                VkResult err;

                /**
                * Check the following requested Vulkan layers against available layers
                */
                VkBool32 validationFound = 0;
                uint32_t instanceLayerCount = 0;
                err = vkEnumerateInstanceLayerProperties(&instanceLayerCount, NULL);
                assert(!err);

                m_enabledLayerNames = {
                    "VK_LAYER_GOOGLE_threading",      "VK_LAYER_LUNARG_mem_tracker",
                    "VK_LAYER_LUNARG_object_tracker", "VK_LAYER_LUNARG_draw_state",
                    "VK_LAYER_LUNARG_param_checker",  "VK_LAYER_LUNARG_swapchain",
                    "VK_LAYER_LUNARG_device_limits",  "VK_LAYER_LUNARG_image",
                    "VK_LAYER_GOOGLE_unique_objects",
                };

                if (instanceLayerCount > 0)
                {
                    VkLayerProperties* instanceLayers = new VkLayerProperties[instanceLayerCount];
                    err = vkEnumerateInstanceLayerProperties(&instanceLayerCount, instanceLayers);
                    assert(!err);

                    bool validated = CheckLayers(m_enabledLayerNames, instanceLayers, instanceLayerCount);

                    delete[] instanceLayers;
                    if (!validated)
                        return false;

                    return true;
                }

#ifdef _DEBUG
                Core::DebugPrintF("VKRenderer::checkInstanceLayers(), instanceLayerCount is zero. \n");
#endif
                return false;
            }

            bool VKRenderer::checkInstanceExtensions()
            {
                VkResult err;

                /**
                * Vulkan:
                *
                * Check the for correct Vulkan instance extensions
                *
                */
                VkBool32 surfaceExtFound = 0;
                VkBool32 platformSurfaceExtFound = 0;
                uint32_t instanceExtensionCount = 0;

                err = vkEnumerateInstanceExtensionProperties(NULL, &instanceExtensionCount, NULL);
                assert(!err);

                if (instanceExtensionCount > 0)
                {
                    VkExtensionProperties* instanceExtensions = new VkExtensionProperties[instanceExtensionCount];
                    err = vkEnumerateInstanceExtensionProperties(NULL, &instanceExtensionCount, instanceExtensions);
                    assert(!err);
                    for (uint32_t i = 0; i < instanceExtensionCount; i++)
                    {
                        if (!strcmp(VK_KHR_SURFACE_EXTENSION_NAME, instanceExtensions[i].extensionName))
                        {
                            surfaceExtFound = 1;
                            m_enabledExtensionNames.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
                        }
#ifdef HT_SYS_WINDOWS
                        if (!strcmp(VK_KHR_WIN32_SURFACE_EXTENSION_NAME, instanceExtensions[i].extensionName)) {
                            platformSurfaceExtFound = 1;
                            m_enabledExtensionNames.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
                        }
#elif defined(HT_SYS_LINUX)
                        if (!strcmp(VK_KHR_XCB_SURFACE_EXTENSION_NAME, instanceExtensions[i].extensionName))
                        {
                            platformSurfaceExtFound = 1;
                            m_enabledExtensionNames.push_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
                        }
#endif

                        if (!strcmp(VK_EXT_DEBUG_REPORT_EXTENSION_NAME, instanceExtensions[i].extensionName)) {
                            m_enabledExtensionNames.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
                        }

                        assert(m_enabledExtensionNames.size() < 64);
                    }

                    delete[] instanceExtensions;

                    return true;
                }

#ifdef _DEBUG
                Core::DebugPrintF("VKRenderer::checkInstanceExtensions(), instanceExtensionCount is zero. \n");
#endif
                return false;
            }

            bool VKRenderer::enumeratePhysicalDevices()
            {
                VkResult err;
                uint32_t gpuCount = 0;

                err = vkEnumeratePhysicalDevices(m_instance, &gpuCount, nullptr);
                if (gpuCount <= 0 || err != VK_SUCCESS)
                {
#ifdef _DEBUG
                    Core::DebugPrintF("No compatible devices were found.\n");
#endif
                    return false;
                }

                VkPhysicalDevice* physicalDevices = new VkPhysicalDevice[gpuCount];
                err = vkEnumeratePhysicalDevices(m_instance, &gpuCount, physicalDevices);
                if (err)
                {
#ifdef _DEBUG
                    Core::DebugPrintF("Vulkan encountered error enumerating physical devices.\n");
#endif
                    delete[] physicalDevices;
                    return false;
                }

                /*For now, we store the first device Vulkan finds*/
                m_gpu = physicalDevices[0];
                delete[] physicalDevices;

                return true;
            }

            bool VKRenderer::checkDeviceLayers()
            {
                VkResult err;
                uint32_t deviceLayerCount = 0;
                err = vkEnumerateDeviceLayerProperties(m_gpu, &deviceLayerCount, NULL);
                assert(!err);

                if (deviceLayerCount == 0)
                {
#ifdef _DEBUG
                    Core::DebugPrintF("VKRenderer::checkValidationLayers(): No layers were found on the device.\n");
#endif
                    return false;
                }

                VkLayerProperties* deviceLayers = new VkLayerProperties[deviceLayerCount];
                err = vkEnumerateDeviceLayerProperties(m_gpu, &deviceLayerCount, deviceLayers);
                assert(!err);

                bool validated = CheckLayers(m_enabledLayerNames, deviceLayers, deviceLayerCount);
                delete[] deviceLayers;

                if (!validated)
                {
#ifdef _DEBUG
                    Core::DebugPrintF("VkRenderer::checkValidationLayers(): Could not validate enabled layers against device layers.\n");
#endif
                    return false;
                }

                return true;
            }

            bool VKRenderer::CheckLayers(std::vector<const char*> layerNames, VkLayerProperties * layers, uint32_t layerCount)
            {
                bool validated = true;
                for (size_t i = 0; i < layerNames.size(); i++)
                {
                    VkBool32 found = 0;
                    for (uint32_t j = 0; j < layerCount; j++)
                    {
                        if (!strcmp(layerNames[i], layers[j].layerName))
                        {
                            found = true;
                            break;
                        }
                    }
                    if (!found)
                    {
#ifdef _DEBUG
                        Core::DebugPrintF("VKRenderer::checkLayers(), Cannot find layer: %s\n", layerNames[i]);
#endif
                        validated = false;
                    }

                }

                return validated;
            }

            bool VKRenderer::CreateBuffer(VkDevice device, VkBufferUsageFlagBits usage, size_t dataSize, void* data, UniformBlock* uniformBlock)
            {
                VkResult err;

                //Setup Buffer

                VkBufferCreateInfo bufferCreateInfo = {};
                bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
                bufferCreateInfo.usage = usage;
                bufferCreateInfo.size = dataSize;

                err = vkCreateBuffer(device, &bufferCreateInfo, nullptr, &uniformBlock->buffer);
                assert(!err);
                if (err != VK_SUCCESS)
                {
#ifdef _DEBUG
                    Core::DebugPrintF("VKMesh::createBuffer(): Failed to create buffer\n");
#endif
                    return false;
                }

                //Setup buffer requirements
                VkMemoryRequirements memReqs;
                vkGetBufferMemoryRequirements(device, uniformBlock->buffer, &memReqs);

                VkMemoryAllocateInfo memAllocInfo = {};
                memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
                memAllocInfo.pNext = nullptr;
                memAllocInfo.allocationSize = memReqs.size;
                memAllocInfo.memoryTypeIndex = 0;

                bool okay = VKRenderer::RendererInstance->MemoryTypeFromProperties(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, &memAllocInfo.memoryTypeIndex);
                assert(okay);
                if (!okay)
                {
#ifdef _DEBUG
                    Core::DebugPrintF("VKMesh::createBuffer(): Failed to get memory type\n");
#endif
                    return false;
                }

                //Allocate and fill memory
                void* pData;

                err = vkAllocateMemory(device, &memAllocInfo, nullptr, &uniformBlock->memory);
                assert(!err);
                if (err != VK_SUCCESS)
                {
#ifdef _DEBUG
                    Core::DebugPrintF("VKMesh::createBuffer(): Failed to allocate memory\n");
#endif
                    return false;
                }

                //We may not ask for a buffer that has anything in it
                if (data != nullptr)
                {
                    err = vkMapMemory(device, uniformBlock->memory, 0, dataSize, 0, &pData);
                    assert(!err);
                    if (err != VK_SUCCESS)
                    {
#ifdef _DEBUG
                        Core::DebugPrintF("VKMesh::createBuffer(): Failed to map memory\n");
#endif
                        return false;
                    }

                    //Actually copy data into location
                    memcpy(pData, data, dataSize);

                    //Unmap memory and then bind to the uniform
                    vkUnmapMemory(device, uniformBlock->memory);
                }

                err = vkBindBufferMemory(device, uniformBlock->buffer, uniformBlock->memory, 0);
                assert(!err);
                if (err != VK_SUCCESS)
                {
#ifdef _DEBUG
                    Core::DebugPrintF("VKMesh::VBuffer(): Failed to bind memory\n");
#endif
                    return false;
                }

                uniformBlock->descriptor.buffer = uniformBlock->buffer;

                return true;
            }
            
            bool VKRenderer::checkDeviceExtensions()
            {
                VkResult err;
                uint32_t deviceExtensionCount = 0;
                VkBool32 swapchainExtFound = 0;
                m_enabledExtensionNames.clear();

                //Check how many extensions are on the device
                err = vkEnumerateDeviceExtensionProperties(m_gpu, NULL, &deviceExtensionCount, NULL);
                assert(!err);

                if (deviceExtensionCount == 0)
                {
#ifdef _DEBUG
                    Core::DebugPrintF("VKRenderer::checkDeviceExtensions(): Device reported no available extensions\n");
#endif
                    return false;
                }

                //Get extension properties
                VkExtensionProperties* deviceExtensions = new VkExtensionProperties[deviceExtensionCount];
                err = vkEnumerateDeviceExtensionProperties(m_gpu, NULL, &deviceExtensionCount, deviceExtensions);
                assert(!err);

                for (uint32_t i = 0; i < deviceExtensionCount; i++) {
                    if (!strcmp(VK_KHR_SWAPCHAIN_EXTENSION_NAME,
                        deviceExtensions[i].extensionName)) {
                        swapchainExtFound = 1;
                        m_enabledExtensionNames.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
                    }
                    assert(m_enabledExtensionNames.size() < 64);
                }

                delete[] deviceExtensions;


                if (!swapchainExtFound)
                {
#ifdef _DEBUG
                    Core::DebugPrintF("vkEnumerateDeviceExtensionProperties failed to find "
                        "the " VK_KHR_SWAPCHAIN_EXTENSION_NAME
                        " extension.\n\nDo you have a compatible "
                        "Vulkan installable client driver (ICD) installed?\nPlease "
                        "look at the Getting Started guide for additional "
                        "information.\n");
#endif
                    return false;
                }

                return true;
            }

            bool VKRenderer::setupDebugCallbacks()
            {
                VkResult err;

                //Get debug callback function pointers
                m_createDebugReportCallback =
                    (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(m_instance, "vkCreateDebugReportCallbackEXT");
                m_destroyDebugReportCallback =
                    (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(m_instance, "vkDestroyDebugReportCallbackEXT");

                if (!m_createDebugReportCallback)
                {
#ifdef _DEBUG
                    Core::DebugPrintF("GetProcAddr: Unable to find vkCreateDebugReportCallbackEXT\n");
#endif
                    return false;
                }
                if (!m_destroyDebugReportCallback) {
#ifdef _DEBUG
                    Core::DebugPrintF("GetProcAddr: Unable to find vkDestroyDebugReportCallbackEXT\n");
#endif
                    return false;
                }

                m_debugReportMessage =
                    (PFN_vkDebugReportMessageEXT)vkGetInstanceProcAddr(m_instance, "vkDebugReportMessageEXT");
                if (!m_debugReportMessage) {
#ifdef _DEBUG
                    Core::DebugPrintF("GetProcAddr: Unable to find vkDebugReportMessageEXT\n");
#endif
                    return false;
                }

                PFN_vkDebugReportCallbackEXT callback;
                callback = VKRenderer::debugFunction;

                VkDebugReportCallbackCreateInfoEXT dbgCreateInfo;
                dbgCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
                dbgCreateInfo.pNext = NULL;
                dbgCreateInfo.pfnCallback = callback;
                dbgCreateInfo.pUserData = NULL;
                dbgCreateInfo.flags =
                    VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
                err = m_createDebugReportCallback(m_instance, &dbgCreateInfo, NULL,
                    &msg_callback);
                switch (err) {
                case VK_SUCCESS:
                    break;
                case VK_ERROR_OUT_OF_HOST_MEMORY:
#ifdef _DEBUG
                    Core::DebugPrintF("ERROR: Out of host memory!\n");
#endif
                    return false;
                default:
#ifdef _DEBUG
                    Core::DebugPrintF("ERROR: An unknown error occured!\n");
#endif
                    return false;
                }

                return true;
            }

            bool VKRenderer::setupDeviceQueues()
            {
                vkGetPhysicalDeviceProperties(m_gpu, &m_gpuProps);

                //Call with NULL data to get count
                uint32_t queueCount;
                vkGetPhysicalDeviceQueueFamilyProperties(m_gpu, &queueCount, NULL);
                assert(queueCount >= 1);

                if (queueCount == 0)
                {
#ifdef _DEBUG
                    Core::DebugPrintF("VKRenderer::setupDeviceQueues: No queues were found on the device\n");
#endif
                    return false;
                }

                m_queueProps = std::vector<VkQueueFamilyProperties>(queueCount);
                vkGetPhysicalDeviceQueueFamilyProperties(m_gpu, &queueCount, &m_queueProps[0]);

                // Find a queue that supports gfx
                uint32_t gfxQueueIdx = 0;
                for (gfxQueueIdx = 0; gfxQueueIdx < queueCount; gfxQueueIdx++) {
                    if (m_queueProps[gfxQueueIdx].queueFlags & VK_QUEUE_GRAPHICS_BIT)
                        break;
                }
                assert(gfxQueueIdx < queueCount);

                if (gfxQueueIdx >= queueCount)
                {
#ifdef _DEBUG
                    Core::DebugPrintF("VKRenderer::setupDeviceQueues: No graphics queue was found on the device\n");
#endif
                    return false;
                }

                return true;
            }

            bool VKRenderer::setupProcAddresses()
            {
                // Query fine-grained feature support for this device.
                //  If app has specific feature requirements it should check supported
                //  features based on this query
                VkPhysicalDeviceFeatures physDevFeatures;
                vkGetPhysicalDeviceFeatures(m_gpu, &physDevFeatures);

                fpGetPhysicalDeviceSurfaceSupportKHR = (PFN_vkGetPhysicalDeviceSurfaceSupportKHR)
                    vkGetInstanceProcAddr(m_instance, "vkGetPhysicalDeviceSurfaceSupportKHR");
                if (fpGetPhysicalDeviceSurfaceSupportKHR == nullptr)
                {
#ifdef _DEBUG
                    Core::DebugPrintF("VKRenderer::setupProcAddresses: vkGetPhysicalDeviceSurfaceSupportKHR not found.\n");
#endif
                    return false;
                }

                fpGetPhysicalDeviceSurfaceCapabilitiesKHR = (PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR)
                    vkGetInstanceProcAddr(m_instance, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR");
                if (fpGetPhysicalDeviceSurfaceCapabilitiesKHR == nullptr)
                {
#ifdef _DEBUG
                    Core::DebugPrintF("VKRenderer::setupProcAddresses: vkGetPhysicalDeviceSurfaceCapabilitiesKHR not found.\n");
#endif
                    return false;
                }

                fpGetPhysicalDeviceSurfaceFormatsKHR = (PFN_vkGetPhysicalDeviceSurfaceFormatsKHR)
                    vkGetInstanceProcAddr(m_instance, "vkGetPhysicalDeviceSurfaceFormatsKHR");
                if (fpGetPhysicalDeviceSurfaceFormatsKHR == nullptr)
                {
#ifdef _DEBUG
                    Core::DebugPrintF("VKRenderer::setupProcAddresses: vkGetPhysicalDeviceSurfaceFormatsKHR not found.\n");
#endif
                    return false;
                }

                fpGetPhysicalDeviceSurfacePresentModesKHR = (PFN_vkGetPhysicalDeviceSurfacePresentModesKHR)
                    vkGetInstanceProcAddr(m_instance, "vkGetPhysicalDeviceSurfacePresentModesKHR");
                if (fpGetPhysicalDeviceSurfacePresentModesKHR == nullptr)
                {
#ifdef _DEBUG
                    Core::DebugPrintF("VKRenderer::setupProcAddresses: vkGetPhysicalDeviceSurfacePresentModesKHR not found.\n");
#endif
                    return false;
                }

                return true;
            }

            bool VKRenderer::setupQueues() 
            {
                uint32_t i; //we reuse this for all the loops

                //Find which queue we can use to present
                VkBool32* supportsPresent = new VkBool32[m_queueProps.size()];
                for (i = 0; i < m_queueProps.size(); i++)
                    fpGetPhysicalDeviceSurfaceSupportKHR(m_gpu, i, m_surface, &supportsPresent[i]);

                //Search for a queue that can both do graphics and presentation
                uint32_t graphicsQueueNodeIndex = UINT32_MAX;
                uint32_t presentQueueNodeIndex = UINT32_MAX;

                for (i = 0; i < m_queueProps.size(); i++) {
                    if ((m_queueProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) {
                        if (graphicsQueueNodeIndex == UINT32_MAX)
                            graphicsQueueNodeIndex = i;

                        if (supportsPresent[i] == VK_TRUE) {
                            graphicsQueueNodeIndex = i;
                            presentQueueNodeIndex = i;
                            break;
                        }
                    }
                }
                if (presentQueueNodeIndex == UINT32_MAX) {
                    // If didn't find a queue that supports both graphics and present, then
                    // find a separate present queue.
                    for (uint32_t i = 0; i < m_queueProps.size(); ++i) {
                        if (supportsPresent[i] == VK_TRUE) {
                            presentQueueNodeIndex = i;
                            break;
                        }
                    }
                }

                delete[] supportsPresent;

                // Generate error if could not find both a graphics and a present queue
                if (graphicsQueueNodeIndex == UINT32_MAX ||
                    presentQueueNodeIndex == UINT32_MAX) {
#ifdef _DEBUG
                    Core::DebugPrintF("Unable to find a graphics and a present queue.\n");
#endif
                    return false;
                }

                //Save the index of the queue we want to use
                m_graphicsQueueNodeIndex = graphicsQueueNodeIndex;

                return true;
            }

            //TODO: Support more than one queue / device?
            bool VKRenderer::createDevice() 
            {
                VkResult err;
                float queuePriorities[1] = { 0.0f };

                VkDeviceQueueCreateInfo queue;
                queue.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
                queue.pNext = nullptr;
                queue.queueFamilyIndex = m_graphicsQueueNodeIndex;
                queue.queueCount = 1;
                queue.pQueuePriorities = queuePriorities;

                VkDeviceCreateInfo device;
                device.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
                device.pNext = nullptr;
                device.queueCreateInfoCount = 1;
                device.pQueueCreateInfos = &queue;
                device.enabledLayerCount = static_cast<uint32_t>(m_enabledLayerNames.size());
                device.ppEnabledLayerNames = &m_enabledLayerNames[0];
                device.enabledExtensionCount = static_cast<uint32_t>(m_enabledExtensionNames.size());
                device.ppEnabledExtensionNames = &m_enabledExtensionNames[0];
                device.pEnabledFeatures = nullptr; //Request specific features here

                err = vkCreateDevice(m_gpu, &device, nullptr, &m_device);
                if (err != VK_SUCCESS)
                {
#ifdef _DEBUG
                    Core::DebugPrintF("Failed to create device. \n");
#endif
                    return false;
                }

                //Pointer to function to get function pointers from device
                PFN_vkGetDeviceProcAddr g_gdpa = (PFN_vkGetDeviceProcAddr)
                    vkGetInstanceProcAddr(m_instance, "vkGetDeviceProcAddr");

                fpCreateSwapchainKHR = (PFN_vkCreateSwapchainKHR)g_gdpa(m_device, "vkCreateSwapchainKHR");
                fpDestroySwapchainKHR = (PFN_vkDestroySwapchainKHR)g_gdpa(m_device, "vkDestroySwapchainKHR");
                fpGetSwapchainImagesKHR = (PFN_vkGetSwapchainImagesKHR)g_gdpa(m_device, "vkGetSwapchainImagesKHR");
                fpAcquireNextImageKHR = (PFN_vkAcquireNextImageKHR)g_gdpa(m_device, "vkAcquireNextImageKHR");
                fpQueuePresentKHR = (PFN_vkQueuePresentKHR)g_gdpa(m_device, "vkQueuePresentKHR");

                return true;
            }

            bool VKRenderer::getSupportedFormats() 
            {
                VkResult err;

                //Get list of supported VkFormats
                uint32_t formatCount;
                err = fpGetPhysicalDeviceSurfaceFormatsKHR(m_gpu, m_surface, &formatCount, nullptr);

                if (err != VK_SUCCESS)
                {
#ifdef _DEBUG
                    Core::DebugPrintF("VkRenderer::getSupportedFormats(): Error getting number of formats from device.\n");
#endif
                    return false;
                }

                //Get format list
                VkSurfaceFormatKHR* surfaceFormats = new VkSurfaceFormatKHR[formatCount];
                err = fpGetPhysicalDeviceSurfaceFormatsKHR(m_gpu, m_surface, &formatCount, surfaceFormats);
                if (err != VK_SUCCESS || formatCount <= 0)
                {
#ifdef _DEBUG
                    Core::DebugPrintF("VkRenderer::getSupportedFormats(): Error getting VkSurfaceFormats from device.\n");
#endif
                    return false;
                }

                // If the format list includes just one entry of VK_FORMAT_UNDEFINED,
                // the surface has no preferred format.  Otherwise, at least one
                // supported format will be returned.
                if (formatCount == 1 && surfaceFormats[0].format == VK_FORMAT_UNDEFINED)
                    m_preferredImageFormat = VK_FORMAT_B8G8R8A8_UNORM;
                else
                    m_preferredImageFormat = surfaceFormats[0].format;

                m_colorSpace = surfaceFormats[0].colorSpace;

                return true;
            }

            bool VKRenderer::setupCommandPool() 
            {
                VkResult err;

                //Create the command pool
                VkCommandPoolCreateInfo commandPoolInfo;
                commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
                commandPoolInfo.pNext = nullptr;
                commandPoolInfo.queueFamilyIndex = m_graphicsQueueNodeIndex;
                commandPoolInfo.flags = 0;

                err = vkCreateCommandPool(m_device, &commandPoolInfo, nullptr, &m_commandPool);

                if (err != VK_SUCCESS)
                {
#ifdef _DEBUG
                    Core::DebugPrintF("VKRenderer::setupCommandPool: Error creating command pool.\n");
#endif
                    return false;
                }

                return true;
            }

            bool VKRenderer::setupDescriptorPool() 
            {
                VkResult err;

                //Setup the descriptor pool
                std::vector<VkDescriptorPoolSize> poolSizes;

                VkDescriptorPoolSize uniformSize = {};
                uniformSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                uniformSize.descriptorCount = 4;

                VkDescriptorPoolSize samplerSize = {};
                samplerSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                samplerSize.descriptorCount = 4;

                poolSizes.push_back(uniformSize);
                poolSizes.push_back(samplerSize);

                VkDescriptorPoolCreateInfo poolCreateInfo = {};
                poolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
                poolCreateInfo.pPoolSizes = poolSizes.data();
                poolCreateInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
                poolCreateInfo.maxSets = 8;
                poolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

                err = vkCreateDescriptorPool(m_device, &poolCreateInfo, nullptr, &m_descriptorPool);
                assert(!err);
                if (err != VK_SUCCESS)
                {
#ifdef _DEBUG
                    Core::DebugPrintF("VKRenderer::setupDescriptorPool: Failed to create descriptor pool\n");
#endif
                    return false;
                }

                return true;
            }

            //TODO: Move this functionality to other subclasses
            bool VKRenderer::prepareVulkan()
            {
                if (!setupCommandPool())
                    return false;

                if (!setupDescriptorPool())
                    return false;

                CreateSetupCommandBuffer();

                m_swapchain->VKPrepare(m_surface, m_colorSpace);

                m_width = m_swapchain->GetWidth();
                m_height = m_swapchain->GetHeight();

                FlushSetupCommandBuffer();

                //TODO: remove this test code
                VKRenderPass* renderPass = new VKRenderPass(m_device, m_commandPool);
                renderPass->SetWidth(m_width);
                renderPass->SetHeight(m_height);
                renderPass->VSetClearColor(Color(m_clearColor.color.float32[0], m_clearColor.color.float32[1], m_clearColor.color.float32[2], m_clearColor.color.float32[3]));

                m_renderTarget = new VKRenderTarget(m_width, m_height);
                m_renderTarget->SetRenderPass(renderPass);

                renderPass->VPrepare();
                m_renderTarget->VPrepare();

                renderPass->SetRenderTarget(m_renderTarget);

                m_swapchain->SetIncomingRenderTarget(m_renderTarget);

                Core::File meshFile;
                meshFile.Open(Core::os_exec_dir() + "Raptor.obj", Core::FileMode::ReadBinary);

                Core::File textureFile;
                textureFile.Open(Core::os_exec_dir() + "raptor.png", Core::FileMode::ReadBinary);

                Resource::ModelHandle model = Resource::Model::GetResourceHandle("Raptor.obj");
                //model.VInitFromFile(&meshFile);

                CreateSetupCommandBuffer();

                //TODO: Once JSON file is found, insert name here
                m_sampler = new VKSampler(m_device, "");
                m_sampler->SetColorSpace(Resource::Sampler::ColorSpace::GAMMA);
                m_sampler->VPrepare();

                m_texture = new VKTexture(m_device);
                m_texture->SetSampler(m_sampler);
                m_texture->VInitFromFile(&textureFile);


				VKShaderHandle vsShader = VKShader::GetResourceHandle("raptor_VS.spv");
				VKShaderHandle fsShader = VKShader::GetResourceHandle("raptor_FS.spv");


                RasterizerState rasterState = {};
                rasterState.cullMode = CullMode::NONE;
                rasterState.polygonMode = PolygonMode::SOLID;
                rasterState.depthClampEnable = true;

                MultisampleState multisampleState = {};
                multisampleState.minSamples = 0;
                multisampleState.samples = SAMPLE_1_BIT;

                Math::Matrix4 view = Math::MMMatrixTranspose(Math::MMMatrixLookAt(Math::Vector3(0, 0, -5), Math::Vector3(0, 0, 0), Math::Vector3(0, 1, 0)));

                Math::Matrix4 proj = Math::MMMatrixTranspose(Math::MMMatrixPerspProj(3.14f * 0.25f, static_cast<float>(m_width), static_cast<float>(m_height), 0.1f, 1000.0f));

                IPipeline* pipeline = new VKPipeline(renderPass->GetVkRenderPass());
				
                pipeline->VLoadShader(ShaderSlot::VERTEX, vsShader->GetRawPointer());
                pipeline->VLoadShader(ShaderSlot::FRAGMENT, fsShader->GetRawPointer());
                pipeline->VSetRasterState(rasterState);
                pipeline->VSetMultisampleState(multisampleState);
                pipeline->VPrepare();

                m_material = new VKMaterial();

                m_material->VSetMatrix4("object.model", Math::Matrix4());
                m_material->VBindTexture("color", m_texture);
                m_material->VPrepare(pipeline);

                std::vector<Resource::Mesh*> meshes = model->GetMeshes();
                IMesh* mesh = new VKMesh();
                mesh->VBuffer(meshes[0]);

                renderPass->ScheduleRenderRequest(pipeline, m_material, mesh);
                 
                Renderable renderable;
                renderable.material = m_material;
                renderable.mesh = mesh;
                m_pipelineList[pipeline].push_back(renderable);

                m_renderPasses.push_back(renderPass);

                pipeline->VUpdate();
                m_material->VUpdate();

                renderPass->SetView(view);
                renderPass->SetProj(proj);

                m_swapchain->VKPrepareResources();
                
                FlushSetupCommandBuffer();

                return true;
            }
        
            void VKRenderer::CreateSetupCommandBuffer() 
            {
                if (m_setupCommandBuffer != VK_NULL_HANDLE)
                {
                    vkFreeCommandBuffers(m_device, m_commandPool, 1, &m_setupCommandBuffer);
                    m_setupCommandBuffer = VK_NULL_HANDLE;
                }

                VkResult err;

                //Start up a basic command buffer if we don't have one already
                if (m_setupCommandBuffer == VK_NULL_HANDLE)
                {
                    VkCommandBufferAllocateInfo command;
                    command.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
                    command.pNext = nullptr;
                    command.commandPool = m_commandPool;
                    command.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
                    command.commandBufferCount = 1;

                    err = vkAllocateCommandBuffers(m_device, &command, &m_setupCommandBuffer);
                    if (err != VK_SUCCESS)
                    {
#ifdef _DEBUG
                        Core::DebugPrintF("VKRenderer::CreateSetupCommandBuffer(): Failed to allocate command buffer.\n");
#endif
                    }
                }

                VkCommandBufferBeginInfo cmdBufInfo = {};
                cmdBufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

                err = vkBeginCommandBuffer(m_setupCommandBuffer, &cmdBufInfo);
                assert(!err);
            }

            void VKRenderer::FlushSetupCommandBuffer()
            {
                VkResult err;
                if (m_setupCommandBuffer == VK_NULL_HANDLE)
                    return;

                err = vkEndCommandBuffer(m_setupCommandBuffer);
                assert(!err);

                const VkCommandBuffer commands[] = { m_setupCommandBuffer };
                VkFence nullFence = VK_NULL_HANDLE;

                VkSubmitInfo submitInfo = {};
                submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
                submitInfo.pNext = nullptr;
                submitInfo.waitSemaphoreCount = 0;
                submitInfo.pWaitSemaphores = nullptr;
                submitInfo.pWaitDstStageMask = nullptr;
                submitInfo.commandBufferCount = 1;
                submitInfo.pCommandBuffers = commands;
                submitInfo.signalSemaphoreCount = 0;
                submitInfo.pSignalSemaphores = nullptr;

                err = vkQueueSubmit(m_queue, 1, &submitInfo, nullFence);
                assert(!err);

                err = vkQueueWaitIdle(m_queue);
                assert(!err);

                vkFreeCommandBuffers(m_device, m_commandPool, 1, commands);
                m_setupCommandBuffer = VK_NULL_HANDLE;
            }

            bool VKRenderer::SetImageLayout(VkCommandBuffer commandBuffer, VkImage image, VkImageAspectFlags aspectMask,
                VkImageLayout oldImageLayout, VkImageLayout newImageLayout)
            {
                VkImageMemoryBarrier imageMemoryBarrier;
                imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
                imageMemoryBarrier.pNext = nullptr;
                imageMemoryBarrier.srcAccessMask = 0;
                imageMemoryBarrier.dstAccessMask = 0;
                imageMemoryBarrier.oldLayout = oldImageLayout;
                imageMemoryBarrier.newLayout = newImageLayout;
                imageMemoryBarrier.image = image;
                imageMemoryBarrier.subresourceRange.aspectMask = aspectMask;
                imageMemoryBarrier.subresourceRange.baseMipLevel = 0;
                imageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
                imageMemoryBarrier.subresourceRange.layerCount = 1;
                imageMemoryBarrier.subresourceRange.levelCount = 1;
                imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

                // Undefined layout
                // Only allowed as initial layout!
                // Make sure any writes to the image have been finished
                if (oldImageLayout == VK_IMAGE_LAYOUT_UNDEFINED)
                {
                    //imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
                }

                // Old layout is color attachment
                // Make sure any writes to the color buffer have been finished
                if (oldImageLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
                {
                    imageMemoryBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
                }

                // Old layout is transfer source
                // Make sure any reads from the image have been finished
                if (oldImageLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
                {
                    imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
                }

                if (oldImageLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
                {
                    imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                }

                // Old layout is shader read (sampler, input attachment)
                // Make sure any shader reads from the image have been finished
                if (oldImageLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
                {
                    imageMemoryBarrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
                }

                // Target layouts (new)

                // New layout is transfer destination (copy, blit)
                // Make sure any copyies to the image have been finished
                if (newImageLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
                {
                    imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                }

                // New layout is transfer source (copy, blit)
                // Make sure any reads from and writes to the image have been finished
                if (newImageLayout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
                {
                    imageMemoryBarrier.srcAccessMask = imageMemoryBarrier.srcAccessMask | VK_ACCESS_TRANSFER_READ_BIT;
                    imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
                }

                // New layout is color attachment
                // Make sure any writes to the color buffer hav been finished
                if (newImageLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)
                {
                    imageMemoryBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
                    //imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
                }

                // New layout is depth attachment
                // Make sure any writes to depth/stencil buffer have been finished
                if (newImageLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
                {
                    imageMemoryBarrier.dstAccessMask = imageMemoryBarrier.dstAccessMask | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
                }

                // New layout is shader read (sampler, input attachment)
                // Make sure any writes to the image have been finished
                if (newImageLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
                {
                    //imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
                    imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
                }

                if (newImageLayout == VK_IMAGE_LAYOUT_PREINITIALIZED)
                {
                    imageMemoryBarrier.dstAccessMask = VK_ACCESS_HOST_WRITE_BIT;
                }


                // Put barrier on top
                VkPipelineStageFlags srcStageFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
                VkPipelineStageFlags destStageFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

                vkCmdPipelineBarrier(commandBuffer, srcStageFlags, destStageFlags, 0, 0, nullptr, 0,
                    nullptr, 1, &imageMemoryBarrier);

                return true;
            }

            bool VKRenderer::MemoryTypeFromProperties(uint32_t typeBits, VkFlags requirementsMask, uint32_t* typeIndex)
            {
                if (RendererInstance == nullptr)
                {
#ifdef _DEBUG
                    Core::DebugPrintF("VKRenderer::MemoryTypeFromProperties(): Tried to call static before the renderer instance was set.\n");
#endif
                    return false;
                }

                VkPhysicalDeviceMemoryProperties memoryProps = RendererInstance->m_memoryProps;

                //Search mem types to find the first index with those properties
                for (uint32_t i = 0; i < 32; i++)
                {
                    if ((typeBits & 1) == 1)
                    {
                        //Type exists; does it match properties we're looking for?
                        if ((memoryProps.memoryTypes[i].propertyFlags & requirementsMask) == requirementsMask)
                        {
                            *typeIndex = i;
                            return true;
                        }
                    }
                    typeBits >>= 1;
                }
                
                return false; //nothing found
            }

            VKAPI_ATTR VkBool32 VKAPI_CALL VKRenderer::debugFunction(VkFlags msgFlags, VkDebugReportObjectTypeEXT objType,
                uint64_t srcObject, size_t location, int32_t msgCode,
                const char *pLayerPrefix, const char *pMsg, void *pUserData)
            {
                if (msgFlags & VK_DEBUG_REPORT_ERROR_BIT_EXT)
                {
#ifdef _DEBUG
                    Core::DebugPrintF("ERROR: [%s] Code %d : %s\n", pLayerPrefix, msgCode,pMsg);
#endif
                }
                else if (msgFlags & VK_DEBUG_REPORT_WARNING_BIT_EXT)
                {
                    // We know that we're submitting queues without fences, ignore this
                    // warning
                    if (strstr(pMsg,
                        "vkQueueSubmit parameter, VkFence fence, is null pointer"))
                    {
                        return false;
                    }

#ifdef _DEBUG
                    Core::DebugPrintF("WARNING: [%s] Code %d : %s\n", pLayerPrefix, msgCode, pMsg);
#endif
                }
                else {
                    return false;
                }

                /*
                * false indicates that layer should not bail-out of an
                * API call that had validation failures. This may mean that the
                * app dies inside the driver due to invalid parameter(s).
                * That's what would happen without validation layers, so we'll
                * keep that behavior here.
                */
                return false;
            }
        }
    }
}
