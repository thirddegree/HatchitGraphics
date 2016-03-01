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
#include <ht_vkmeshrenderer.h>
#include <ht_vkrendertarget.h>
#include <ht_debug.h>

#include <cassert>

#ifdef _WIN32
#include <windows.h>
#endif

namespace Hatchit {

    namespace Graphics {

        namespace Vulkan {

            VKRenderer* VKRenderer::RendererInstance = nullptr;

            VKRenderer::VKRenderer()
            {
                m_swapchain = 0;
                m_setupCommandBuffer = 0;
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
                uint32_t i;

                for (i = 0; i < m_swapchainBuffers.size(); i++) 
                    vkDestroyFramebuffer(m_device, m_framebuffers[i], nullptr);
                
                m_framebuffers.clear();

                vkDestroyRenderPass(m_device, m_renderPass, nullptr);

                fpDestroySwapchainKHR(m_device, m_swapchain, nullptr);

                vkDestroyImageView(m_device, m_depthBuffer.view, nullptr);
                vkDestroyImage(m_device, m_depthBuffer.image, nullptr);
                vkFreeMemory(m_device, m_depthBuffer.memory, nullptr);

                for (i = 0; i < m_swapchainBuffers.size(); i++) {
                    vkDestroyImageView(m_device, m_swapchainBuffers[i].view, nullptr);
                    vkFreeCommandBuffers(m_device, m_commandPool, 1,
                        &m_swapchainBuffers[i].command);
                }
                m_swapchainBuffers.clear();

                m_queueProps.clear();

                vkDestroyCommandPool(m_device, m_commandPool, nullptr);
                vkDestroyDevice(m_device, nullptr);

                m_destroyDebugReportCallback(m_instance, msg_callback, nullptr);
                
                //vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
                vkDestroyInstance(m_instance, nullptr);
            }

            void VKRenderer::VResizeBuffers(uint32_t width, uint32_t height)
            {
                uint32_t i;

                for (i = 0; i < m_swapchainBuffers.size(); i++)
                    vkDestroyFramebuffer(m_device, m_framebuffers[i], nullptr);
                m_framebuffers.clear();

                vkDestroyPipeline(m_device, m_pipeline, nullptr);
                vkDestroyPipelineCache(m_device, m_pipelineCache, nullptr);
                vkDestroyRenderPass(m_device, m_renderPass, nullptr);
                vkDestroyDescriptorSetLayout(m_device, m_descriptorLayout, nullptr);

                //TODO: Destroy textures

                vkDestroyImageView(m_device, m_depthBuffer.view, nullptr);
                vkDestroyImage(m_device, m_depthBuffer.image, nullptr);
                vkFreeMemory(m_device, m_depthBuffer.memory, nullptr);

                //TODO: Destroy uniform info

                for (i = 0; i < m_swapchainBuffers.size(); i++)
                {
                    vkDestroyImageView(m_device, m_swapchainBuffers[i].view, nullptr);
                    vkFreeCommandBuffers(m_device, m_commandPool, 1, &m_swapchainBuffers[i].command);
                }
                vkDestroyCommandPool(m_device, m_commandPool, nullptr);
                m_swapchainBuffers.clear();

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
                
                VkSemaphoreCreateInfo presentCompleteSemaphoreInfo = {};
                presentCompleteSemaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
                presentCompleteSemaphoreInfo.pNext = nullptr;
                presentCompleteSemaphoreInfo.flags = 0;

                VkFence nullFence = VK_NULL_HANDLE;

                err = vkCreateSemaphore(m_device, &presentCompleteSemaphoreInfo, nullptr, &m_presentSemaphore);
                assert(!err);

                //Get the next image to draw on
                //TODO: Actually use fences
                err = fpAcquireNextImageKHR(m_device, m_swapchain, UINT64_MAX, m_presentSemaphore, nullFence, &m_currentBuffer);
                if (err == VK_ERROR_OUT_OF_DATE_KHR)
                {
                    //Resize!
                    VResizeBuffers(m_width, m_height); //TODO: find a better way to resize
                    vkDestroySemaphore(m_device, m_presentSemaphore, nullptr);
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

            void VKRenderer::VRender() 
            {
                //TODO: Determine which physical device and thread are best to render with

                //Get list of command buffers
                std::vector<VkCommandBuffer> commandBuffers;

                for (size_t i = 0; i < m_renderPasses.size(); i++)
                {
                    VKRenderPass* renderPass = static_cast<VKRenderPass*>(m_renderPasses[i]);

                    commandBuffers.push_back(renderPass->GetVkCommandBuffer());
                }

                //Make sure we run the swapchain command
                commandBuffers.push_back(m_swapchainBuffers[m_currentBuffer].command);

                VkResult err;

                VkPipelineStageFlags pipelineStageFlags = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
                VkSubmitInfo submitInfo = {};
                submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
                submitInfo.pNext = nullptr;
                submitInfo.waitSemaphoreCount = 1;
                submitInfo.pWaitSemaphores = &m_presentSemaphore;
                submitInfo.pWaitDstStageMask = &pipelineStageFlags;
                submitInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());
                submitInfo.pCommandBuffers = commandBuffers.data();
                submitInfo.signalSemaphoreCount = 0;
                submitInfo.pSignalSemaphores = nullptr;

                err = vkQueueSubmit(m_queue, 1, &submitInfo, VK_NULL_HANDLE);
                assert(!err);
            }

            void VKRenderer::VPresent()
            {
                VkResult err;

                VkPresentInfoKHR present = {};
                present.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
                present.pNext = nullptr;
                present.swapchainCount = 1;
                present.pSwapchains = &m_swapchain;
                present.pImageIndices = &m_currentBuffer;

                err = fpQueuePresentKHR(m_queue, &present);
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

                vkDestroySemaphore(m_device, m_presentSemaphore, nullptr);
            }

            VkPhysicalDevice VKRenderer::GetVKPhysicalDevice() 
            {
                return m_gpu;
            }

            VkDevice VKRenderer::GetVKDevice() 
            {
                return m_device;
            }

            VkCommandPool VKRenderer::GetVKCommandPool()
            {
                return m_commandPool;
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
                return m_depthBuffer.format;
            }

            bool VKRenderer::initVulkan(const RendererParams& params) 
            {
                VkResult err;
                /*
                * Check Vulkan instance layers
                */
                if (!checkInstanceLayers())
                    return false;

                /*
                * Check Vulkan instance extensions
                */
                if (!checkInstanceExtensions())
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

                //Setup some function pointers from the device

                //Pointer to function to get function pointers from device
                PFN_vkGetDeviceProcAddr g_gdpa = (PFN_vkGetDeviceProcAddr)
                    vkGetInstanceProcAddr(m_instance, "vkGetDeviceProcAddr");

                //Get other function pointers
                fpCreateSwapchainKHR = (PFN_vkCreateSwapchainKHR)g_gdpa(m_device, "vkCreateSwapchainKHR");
                fpDestroySwapchainKHR = (PFN_vkDestroySwapchainKHR)g_gdpa(m_device, "vkDestroySwapchainKHR");
                fpGetSwapchainImagesKHR = (PFN_vkGetSwapchainImagesKHR)g_gdpa(m_device, "vkGetSwapchainImagesKHR");
                fpAcquireNextImageKHR = (PFN_vkAcquireNextImageKHR)g_gdpa(m_device, "vkAcquireNextImageKHR");
                fpQueuePresentKHR = (PFN_vkQueuePresentKHR)g_gdpa(m_device, "vkQueuePresentKHR");

                //Get Device queue
                vkGetDeviceQueue(m_device, m_graphicsQueueNodeIndex, 0, &m_queue);

                /*
                * Get the supported texture format and color space
                */
                if (!getSupportedFormats())
                    return false;

                //Get memory information
                vkGetPhysicalDeviceMemoryProperties(m_gpu, &m_memoryProps);

                return true;
            }


            bool VKRenderer::checkInstanceLayers()
            {
                VkResult err;

                /**
                * Vulkan:
                *
                * Check the following requested Vulkan layers against available layers
                *
                */
                VkBool32 validationFound = 0;
                uint32_t instanceLayerCount = 0;
                err = vkEnumerateInstanceLayerProperties(&instanceLayerCount, NULL);
                assert(!err);

                m_enabledLayerNames = {
                    "VK_LAYER_LUNARG_threading",      "VK_LAYER_LUNARG_mem_tracker",
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
#ifdef _WIN32
                        if (!strcmp(VK_KHR_WIN32_SURFACE_EXTENSION_NAME, instanceExtensions[i].extensionName)) {
                            platformSurfaceExtFound = 1;
                            m_enabledExtensionNames.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
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

                fpGetSwapchainImagesKHR = (PFN_vkGetSwapchainImagesKHR)
                    vkGetInstanceProcAddr(m_instance, "vkGetSwapchainImagesKHR");
                if (fpGetSwapchainImagesKHR == nullptr)
                {
#ifdef _DEBUG
                    Core::DebugPrintF("VKRenderer::setupProcAddresses: vkGetSwapchainImagesKHR not found.\n");
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

            //TODO: Move this functionality to other subclasses
            bool VKRenderer::prepareVulkan()
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
                    Core::DebugPrintF("VKRenderer::prepareVulkan(): Error creating command pool.\n");
#endif
                    return false;
                }

                CreateSetupCommandBuffer();

                /*
                * Prepare the swapchain buffers
                */
                if (!prepareSwapchainBuffers())
                    return false;

                /*
                * Prepare the swapchain depth buffer
                */
                if (!prepareSwapchainDepth())
                    return false;
                
                /*
                * Prepare the render pass; describes which color and depth formats the command buffers will write to
                */
                if (!prepareRenderPass())
                    return false;

                /*
                * Allocate memory for the command buffers
                */
                if (!allocateCommandBuffers())
                    return false;
                
                /*
                * Prepare frame buffers to draw on; one for each image in the swapchain
                */
                if (!prepareFrambuffers())
                    return false;
                
                FlushSetupCommandBuffer();

                //TODO: remove this test code
                IRenderPass* renderPass = new VKRenderPass();
                renderPass->SetWidth(m_width);
                renderPass->SetHeight(m_height);

                m_renderTarget = new VKRenderTarget(m_width, m_height);
                m_renderTarget->SetRenderPass(renderPass);

                renderPass->VPrepare();
                m_renderTarget->VPrepare();

                renderPass->SetRenderTarget(m_renderTarget);

                Core::File meshFile;
                meshFile.Open(Core::os_exec_dir() + "monkey.obj", Core::FileMode::ReadBinary);

                Core::File vsFile;
                vsFile.Open(Core::os_exec_dir() + "monkey_VS.spv", Core::FileMode::ReadBinary);

                Core::File fsFile;
                fsFile.Open(Core::os_exec_dir() + "monkey_FS.spv", Core::FileMode::ReadBinary);

                Resource::Model model;
                model.VInitFromFile(&meshFile);

                VKShader vsShader;
                vsShader.VInitFromFile(&vsFile);

                VKShader fsShader;
                fsShader.VInitFromFile(&fsFile);

                RasterizerState rasterState = {};
                rasterState.cullMode = CullMode::NONE;
                rasterState.polygonMode = PolygonMode::SOLID;

                MultisampleState multisampleState = {};
                multisampleState.minSamples = 0;
                multisampleState.samples = SAMPLE_1_BIT;

                IPipeline* pipeline = new VKPipeline(m_renderPass);
                pipeline->VLoadShader(ShaderSlot::VERTEX, &vsShader);
                pipeline->VLoadShader(ShaderSlot::FRAGMENT, &fsShader);
                pipeline->VSetRasterState(rasterState);
                pipeline->VSetMultisampleState(multisampleState);
                pipeline->VPrepare();

                IMaterial* material = new VKMaterial();

                Math::Matrix4 mat = Math::MMMatrixTranslation(Math::Vector3(0, 0, 0));

                Math::Matrix4 view = Math::MMMatrixLookAt(Math::Vector3(0, 0, 5), Math::Vector3(0, 0, 0), Math::Vector3(0, 1, 0));

                Math::Matrix4 proj = Math::MMMatrixPerspProj(45.0f, (float)m_width / (float)m_height, 0.1f, 1000.0f);

                Math::Matrix4 mvp = mat * (view * proj);

                material->VSetMatrix4("matricies.model", mvp);
                material->VPrepare();
                material->VUpdate();

                std::vector<Resource::Mesh*> meshes = model.GetMeshes();
                IMesh* mesh = new VKMesh();
                mesh->VBuffer(meshes[0]);

                renderPass->ScheduleRenderRequest(pipeline, material, mesh);
                 
                Renderable renderable;
                renderable.material = material;
                renderable.mesh = mesh;
                m_pipelineList[pipeline].push_back(renderable);

                renderPass->VBuildCommandList();

                m_renderPasses.push_back(renderPass);

                /*
                * Build all the command buffers for the swapchain
                */
                for (uint32_t i = 0; i < m_swapchainBuffers.size(); i++) {
                    m_currentBuffer = i;
                    buildCommandBuffer(m_swapchainBuffers[i].command);
                }
                
                //Flush the command buffer once
                FlushSetupCommandBuffer();

                m_currentBuffer = 0;

                return true;
            }

            bool VKRenderer::prepareSwapchainBuffers() 
            {
                VkResult err;
                VkSwapchainKHR oldSwapchain = m_swapchain;

                //Check surface capabilities and formats
                
                VkSurfaceCapabilitiesKHR surfaceCapabilities;
                err = fpGetPhysicalDeviceSurfaceCapabilitiesKHR(m_gpu, m_surface, &surfaceCapabilities);
                if (err != VK_SUCCESS)
                {
#ifdef _DEBUG
                    Core::DebugPrintF("VKRenderer::preapreSwapchainBuffers(): Failed to get surface capabilities.\n");
#endif
                    return false;
                }

                //Get present modes
                
                uint32_t presentModeCount;
                err = fpGetPhysicalDeviceSurfacePresentModesKHR(m_gpu, m_surface, &presentModeCount, nullptr);
                if (err != VK_SUCCESS)
                {
#ifdef _DEBUG
                    Core::DebugPrintF("VKRenderer::preapreSwapchainBuffers(): Failed to get number of present modes.\n");
#endif
                    return false;
                }

                VkPresentModeKHR* presentModes = new VkPresentModeKHR[presentModeCount];
                err = fpGetPhysicalDeviceSurfacePresentModesKHR(m_gpu, m_surface, &presentModeCount, presentModes);
                if (err != VK_SUCCESS)
                {
#ifdef _DEBUG
                    Core::DebugPrintF("VKRenderer::preapreSwapchainBuffers(): Failed to get present modes from device.\n");
#endif
                    return false;
                }

                //Get the extent to match the current recorded width and height
                //Or set the width and height to the bounds of the current extent
                VkExtent2D swapchainExtent;
                if (surfaceCapabilities.currentExtent.width == (uint32_t)-1)
                {
                    swapchainExtent.width = m_width;
                    swapchainExtent.height = m_height;
                }
                else
                {
                    swapchainExtent = surfaceCapabilities.currentExtent;
                    m_width = swapchainExtent.width;
                    m_height = swapchainExtent.height;
                }

                //Use mailbox mode if available as it's the lowest-latency non-tearing mode
                //If that's not available try immediate mode which SHOULD be available and is fast but tears
                //Fall back to FIFO which is always available
                VkPresentModeKHR swapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;
                for (size_t i = 0; i < presentModeCount; i++) {
                    if (presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
                        swapchainPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
                        break;
                    }
                    if ((swapchainPresentMode != VK_PRESENT_MODE_MAILBOX_KHR) &&
                        (presentModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR)) {
                        swapchainPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
                    }
                }

                //Determine how many VkImages to use in the swap chain
                //We only own one at a time besides the image being displayed
                uint32_t desiredNumberOfSwapchainImages = surfaceCapabilities.minImageCount + 1;
                if ((surfaceCapabilities.maxImageCount > 0) &
                    (desiredNumberOfSwapchainImages > surfaceCapabilities.maxImageCount))
                    desiredNumberOfSwapchainImages = surfaceCapabilities.maxImageCount;

                //Setup transform flags
                VkSurfaceTransformFlagBitsKHR preTransform;
                if (surfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
                    preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
                else
                    preTransform = surfaceCapabilities.currentTransform;

                //Create swapchain

                VkSwapchainCreateInfoKHR swapchainInfo;
                swapchainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
                swapchainInfo.pNext = nullptr;
                swapchainInfo.surface = m_surface;
                swapchainInfo.minImageCount = desiredNumberOfSwapchainImages;
                swapchainInfo.imageFormat = m_preferredImageFormat;
                swapchainInfo.imageColorSpace = m_colorSpace;
                swapchainInfo.imageExtent = swapchainExtent;
                swapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
                swapchainInfo.preTransform = preTransform;
                swapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
                swapchainInfo.imageArrayLayers = 1;
                swapchainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
                swapchainInfo.queueFamilyIndexCount = 0;
                swapchainInfo.pQueueFamilyIndices = nullptr;
                swapchainInfo.presentMode = swapchainPresentMode;
                swapchainInfo.oldSwapchain = oldSwapchain;
                swapchainInfo.clipped = true;
                swapchainInfo.flags = 0;

                uint32_t i; // About to be used for a bunch of loops

                err = fpCreateSwapchainKHR(m_device, &swapchainInfo, nullptr, &m_swapchain);
                if (err != VK_SUCCESS)
                {
#ifdef _DEBUG
                    Core::DebugPrintF("VKRenderer::prepareSwapchainBuffers(): Failed to create Swapchain.\n");
#endif
                    return false;
                }

                //Destroy old swapchain
                if (oldSwapchain != VK_NULL_HANDLE)
                    fpDestroySwapchainKHR(m_device, oldSwapchain, nullptr);

                //Get the swapchain images
                uint32_t swapchainImageCount;
                err = fpGetSwapchainImagesKHR(m_device, m_swapchain, &swapchainImageCount, nullptr);
                if(err != VK_SUCCESS)
                {
#ifdef _DEBUG
                    Core::DebugPrintF("VKRenderer::prepareSwapchainBuffers(): Failed to get swapchain image count.\n");
#endif
                    return false;
                }

                VkImage* swapchainImages = new VkImage[swapchainImageCount];
                err = fpGetSwapchainImagesKHR(m_device, m_swapchain, &swapchainImageCount, swapchainImages);
                if (err != VK_SUCCESS)
                {
#ifdef _DEBUG
                    Core::DebugPrintF("VKRenderer::prepareSwapchainBuffers(): Failed to get swapchain images.\n");
#endif
                    return false;
                }

                for (i = 0; i < swapchainImageCount; i++)
                {
                    VkComponentMapping components;
                    components.r = VK_COMPONENT_SWIZZLE_R;
                    components.g = VK_COMPONENT_SWIZZLE_G;
                    components.b = VK_COMPONENT_SWIZZLE_B;
                    components.a = VK_COMPONENT_SWIZZLE_A;

                    VkImageSubresourceRange subresourceRange;
                    subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                    subresourceRange.baseMipLevel = 0;
                    subresourceRange.levelCount = 1;
                    subresourceRange.baseArrayLayer = 0;
                    subresourceRange.layerCount = 1;

                    VkImageViewCreateInfo colorImageView;
                    colorImageView.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
                    colorImageView.pNext = nullptr;
                    colorImageView.format = m_preferredImageFormat;
                    colorImageView.components = components;
                    colorImageView.subresourceRange = subresourceRange;
                    colorImageView.viewType = VK_IMAGE_VIEW_TYPE_2D;
                    colorImageView.flags = 0;

                    SwapchainBuffers buffer;
                    buffer.image = swapchainImages[i];

                    //Render loop will expect image to have been used before
                    //Init image ot the VK_IMAGE_ASPECT_COLOR_BIT state
                    SetImageLayout(m_setupCommandBuffer, buffer.image, VK_IMAGE_ASPECT_COLOR_BIT,
                        VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

                    colorImageView.image = buffer.image;

                    //Attempt to create the image view
                    err = vkCreateImageView(m_device, &colorImageView, nullptr, &buffer.view);

                    if (err != VK_SUCCESS)
                    {
#ifdef _DEBUG
                        Core::DebugPrintF("VKRenderer::prepareSwapchainBuffers(): Failed to create image view.\n");
#endif
                        return false;
                    }

                    m_swapchainBuffers.push_back(buffer);
                }

                if (presentModes != nullptr)
                    delete[] presentModes;

                return true;
            }

            bool VKRenderer::prepareSwapchainDepth() 
            {
                VkResult err;
                const VkFormat depthFormat = VK_FORMAT_D16_UNORM;

                VkImageCreateInfo image;
                image.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
                image.pNext = nullptr;
                image.imageType = VK_IMAGE_TYPE_2D;
                image.format = depthFormat;
                image.extent = {m_width, m_height, 1};
                image.mipLevels = 1;
                image.arrayLayers = 1;
                image.samples = VK_SAMPLE_COUNT_1_BIT;
                image.tiling = VK_IMAGE_TILING_OPTIMAL;
                image.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
                image.flags = 0;
                image.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
                image.queueFamilyIndexCount = 0;
                image.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

                VkComponentMapping components;
                components.r = VK_COMPONENT_SWIZZLE_R;
                components.g = VK_COMPONENT_SWIZZLE_G;
                components.b = VK_COMPONENT_SWIZZLE_B;
                components.a = VK_COMPONENT_SWIZZLE_A;

                VkImageSubresourceRange depthSubresourceRange;
                depthSubresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT ;
                depthSubresourceRange.baseMipLevel = 0;
                depthSubresourceRange.levelCount = 1;
                depthSubresourceRange.baseArrayLayer = 0;
                depthSubresourceRange.layerCount = 1;

                VkImageViewCreateInfo view;
                view.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
                view.pNext = nullptr;
                view.image = VK_NULL_HANDLE;
                view.format = depthFormat;
                view.subresourceRange = depthSubresourceRange;
                view.flags = 0;
                view.components = components;
                view.viewType = VK_IMAGE_VIEW_TYPE_2D;

                m_depthBuffer.format = depthFormat;

                VkMemoryRequirements memoryRequirements;
                bool pass;

                //Create image
                err = vkCreateImage(m_device, &image, nullptr, &m_depthBuffer.image);
                assert(!err);
                if (err != VK_SUCCESS)
                {
#ifdef _DEBUG
                    Core::DebugPrintF("VKRenderer::prepareSwapchainDepth(): Error, failed to create image\n");
#endif
                    return false;
                }

                vkGetImageMemoryRequirements(m_device, m_depthBuffer.image, &memoryRequirements);

                m_depthBuffer.memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
                m_depthBuffer.memAllocInfo.pNext = nullptr;
                m_depthBuffer.memAllocInfo.allocationSize = memoryRequirements.size;
                m_depthBuffer.memAllocInfo.memoryTypeIndex = 0;
                
                //No requirements
                pass = MemoryTypeFromProperties(memoryRequirements.memoryTypeBits, 0, &m_depthBuffer.memAllocInfo.memoryTypeIndex);
                assert(pass);
                if (!pass)
                {
#ifdef _DEBUG
                    Core::DebugPrintF("VKRenderer::prepareSwapchainDepth(): Error, failed to get memory type for depth buffer\n");
#endif
                    return false;
                }

                //Allocate Memory
                err = vkAllocateMemory(m_device, &m_depthBuffer.memAllocInfo, nullptr, &m_depthBuffer.memory);
                assert(!err);
                if (err != VK_SUCCESS)
                {
#ifdef _DEBUG
                    Core::DebugPrintF("VKRenderer::prepareSwapchainDepth(): Error, failed to allocate memory for depth buffer\n");
#endif
                    return false;
                }

                //Bind Memory
                err = vkBindImageMemory(m_device, m_depthBuffer.image, m_depthBuffer.memory, 0);
                assert(!err);
                if (err != VK_SUCCESS)
                {
#ifdef _DEBUG
                    Core::DebugPrintF("VKRenderer::prepareSwapchainDepth(): Error, failed to bind depth buffer memory\n");
#endif
                    return false;
                }

                SetImageLayout(m_setupCommandBuffer, m_depthBuffer.image, VK_IMAGE_ASPECT_DEPTH_BIT,
                    VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

                //Create image view
                view.image = m_depthBuffer.image;
                err = vkCreateImageView(m_device, &view, nullptr, &m_depthBuffer.view);
                assert(!err);
                if (err != VK_SUCCESS)
                {
#ifdef _DEBUG
                    Core::DebugPrintF("VKRenderer::prepareSwapchainDepth(): Error, failed create image view for depth buffer\n");
#endif
                    return false;
                }


                return true; 
            }

            //TODO: move render pass creation to a new class
            bool VKRenderer::prepareRenderPass() 
            { 
                VkAttachmentDescription attachments[2];
                attachments[0].format = m_preferredImageFormat;
                attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
                attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
                attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
                attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
                attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
                attachments[0].initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                attachments[0].finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                attachments[0].flags = 0;

                attachments[1].format = m_depthBuffer.format;
                attachments[1].samples = VK_SAMPLE_COUNT_1_BIT;
                attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
                attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
                attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
                attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE; 
                attachments[1].initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
                attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
                attachments[1].flags = 0;
                
                VkAttachmentReference colorReference;
                colorReference.attachment = 0;
                colorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                
                VkAttachmentReference depthReference;
                depthReference.attachment = 1;
                depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
                
                VkSubpassDescription subpass;
                subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
                subpass.flags = 0;
                subpass.inputAttachmentCount = 0;
                subpass.pInputAttachments = nullptr;
                subpass.colorAttachmentCount = 1;
                subpass.pColorAttachments = &colorReference;
                subpass.pResolveAttachments = nullptr;
                subpass.pDepthStencilAttachment = &depthReference;
                subpass.preserveAttachmentCount = 0;
                subpass.pPreserveAttachments = nullptr;

                VkRenderPassCreateInfo renderPassInfo;
                renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
                renderPassInfo.pNext = nullptr;
                renderPassInfo.attachmentCount = 2;
                renderPassInfo.pAttachments = attachments;
                renderPassInfo.subpassCount = 1;
                renderPassInfo.pSubpasses = &subpass;
                renderPassInfo.dependencyCount = 0;
                renderPassInfo.pDependencies = nullptr;
                renderPassInfo.flags = 0;
                
                VkResult err;

                err = vkCreateRenderPass(m_device, &renderPassInfo, nullptr, &m_renderPass);
                assert(!err);
                if (err != VK_SUCCESS)
                {
#ifdef _DEBUG
                    Core::DebugPrintF("VKRenderer::prepareDescriptorLayout(): Failed to create render pass\n");
#endif
                    return false;
                }

                return true; 
            }
            bool VKRenderer::prepareFrambuffers() 
            { 
                VkResult err;

                VkImageView attachments[2] = {};
                attachments[1] = m_depthBuffer.view;

                VkFramebufferCreateInfo framebufferInfo = {};
                framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
                framebufferInfo.pNext = nullptr;
                framebufferInfo.renderPass = m_renderPass;
                framebufferInfo.attachmentCount = 2;
                framebufferInfo.pAttachments = attachments;
                framebufferInfo.width = m_width;
                framebufferInfo.height = m_height;
                framebufferInfo.layers = 1;

                for (uint32_t i = 0; i < m_swapchainBuffers.size(); i++)
                {
                    attachments[0] = m_swapchainBuffers[i].view;
                    VkFramebuffer framebuffer;
                    err = vkCreateFramebuffer(m_device, &framebufferInfo, nullptr, &framebuffer);

                    if (err != VK_SUCCESS)
                    {
#ifdef _DEBUG
                        Core::DebugPrintF("VKRenderer::prepareFrambuffers(): Failed to create framebuffer at index:%d \n", i);
#endif
                        return false;
                    }

                    m_framebuffers.push_back(framebuffer);
                }

                return true; 
            }
            bool VKRenderer::allocateCommandBuffers() 
            {
                VkResult err;

                VkCommandBufferAllocateInfo allocateInfo;
                allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
                allocateInfo.pNext = nullptr;
                allocateInfo.commandPool = m_commandPool;
                allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
                allocateInfo.commandBufferCount = 1;

                for (uint32_t i = 0; i < m_swapchainBuffers.size(); i++)
                {
                    err = vkAllocateCommandBuffers(m_device, &allocateInfo, &m_swapchainBuffers[i].command);
                    assert(!err);

                    if (err != VK_SUCCESS)
                    {
#ifdef _DEBUG
                        Core::DebugPrintF("VKRenderer::allocateCommandBuffers(): Failed to allocate for command buffer at index:%d \n", i);
#endif
                        return false;
                    }
                }

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
                imageMemoryBarrier.subresourceRange.layerCount = 1;
                imageMemoryBarrier.subresourceRange.levelCount = 1;
                imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

                // Undefined layout
                // Only allowed as initial layout!
                // Make sure any writes to the image have been finished
                if (oldImageLayout == VK_IMAGE_LAYOUT_UNDEFINED)
                {
                    imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
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
                    imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
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
                    imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
                    imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
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

            bool VKRenderer::buildCommandBuffer(VkCommandBuffer commandBuffer) 
            {
                VkResult err;

                VkCommandBufferInheritanceInfo inheritanceInfo = {};
                inheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
                inheritanceInfo.pNext = nullptr;
                inheritanceInfo.renderPass = VK_NULL_HANDLE;
                inheritanceInfo.subpass = 0;
                inheritanceInfo.framebuffer = VK_NULL_HANDLE;
                inheritanceInfo.occlusionQueryEnable = VK_FALSE;
                inheritanceInfo.queryFlags = 0;
                inheritanceInfo.pipelineStatistics = 0;

                VkCommandBufferBeginInfo beginInfo = {};
                beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
                beginInfo.pNext = nullptr;
                beginInfo.flags = 0;
                beginInfo.pInheritanceInfo = &inheritanceInfo;

                VkClearValue clearValues[2] = {};
                clearValues[0] = m_clearColor;
                clearValues[1] = {1.0f, 0};

                VkRenderPassBeginInfo renderPassBeginInfo = {};
                renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
                renderPassBeginInfo.pNext = nullptr;
                renderPassBeginInfo.renderPass = m_renderPass;
                renderPassBeginInfo.framebuffer = m_framebuffers[m_currentBuffer];
                renderPassBeginInfo.renderArea.offset.x = 0;
                renderPassBeginInfo.renderArea.offset.y = 0;
                renderPassBeginInfo.renderArea.extent.width = m_width;
                renderPassBeginInfo.renderArea.extent.height = m_height;
                renderPassBeginInfo.clearValueCount = 2;
                renderPassBeginInfo.pClearValues = clearValues;

                err = vkBeginCommandBuffer(commandBuffer, &beginInfo);
                assert(!err);
                if (err != VK_SUCCESS)
                {
#ifdef _DEBUG
                    Core::DebugPrintF("VKRenderer::buildCommandBuffer(): Failed to build command buffer.\n");
#endif
                    return false;
                }

                VkViewport viewport = {};
                viewport.width = static_cast<float>(m_width);
                viewport.height = static_cast<float>(m_height);
                viewport.minDepth = 0.0f;
                viewport.maxDepth = 1.0f;

                VkRect2D scissor = {};
                scissor.extent.width = m_width;
                scissor.extent.height = m_height;
                scissor.offset.x = 0;
                scissor.offset.y = 0;

                vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

                vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
                vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

                std::map<IPipeline*, std::vector<Renderable>>::iterator iterator;

                for (iterator = m_pipelineList.begin(); iterator != m_pipelineList.end(); iterator++)
                {
                    VKPipeline* pipeline = static_cast<VKPipeline*>(iterator->first);

                    VkPipeline vkPipeline = pipeline->GetVKPipeline();
                    VkPipelineLayout vkPipelineLayout = pipeline->GetPipelineLayout();

                    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->GetVKPipeline());

                    /*  TODO: Set, get and bind pipeline-wide descriptor sets
                    VkDescriptorSet descriptorSet = material->GetDescriptorSet();

                    vkCmdBindDescriptorSets(m_commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                    vkPipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
                    */

                    std::vector<Renderable> renderables = iterator->second;

                    VkDeviceSize offsets[] = { 0 };

                    for (uint32_t i = 0; i < renderables.size(); i++)
                    {
                        VKMaterial* material = static_cast<VKMaterial*>(renderables[i].material);
                        VKMesh*     mesh = static_cast<VKMesh*>(renderables[i].mesh);

                        VkDescriptorSet* descriptorSet = material->GetDescriptorSet();
                        
                        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            vkPipelineLayout, 0, 1, descriptorSet, 0, nullptr);

                        UniformBlock vertBlock = mesh->GetVertexBlock();
                        UniformBlock indexBlock = mesh->GetIndexBlock();
                        uint32_t indexCount = mesh->GetIndexCount();

                        vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertBlock.buffer, offsets);
                        vkCmdBindIndexBuffer(commandBuffer, indexBlock.buffer, 0, VK_INDEX_TYPE_UINT32);
                        vkCmdDrawIndexed(commandBuffer, indexCount, 1, 0, 0, 0);
                    }
                }

                vkCmdEndRenderPass(commandBuffer);

                VkImageMemoryBarrier prePresentBarrier = {};
                prePresentBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
                prePresentBarrier.pNext = nullptr;
                prePresentBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
                prePresentBarrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
                prePresentBarrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                prePresentBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
                prePresentBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                prePresentBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
                prePresentBarrier.subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};

                prePresentBarrier.image = m_swapchainBuffers[m_currentBuffer].image;

                vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
                    VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 0, 0, nullptr, 0, nullptr, 1, &prePresentBarrier);

                err = vkEndCommandBuffer(commandBuffer);
                assert(!err);
                if (err != VK_SUCCESS)
                {
#ifdef _DEBUG
                    Core::DebugPrintF("VKRenderer::buildCommandBuffer(): Failed to end command buffer.\n");
#endif
                    return false;
                }

                return true;
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
                    Core::DebugPrintF("WARNING: [%s] Code %d : %s", pLayerPrefix, msgCode, pMsg);
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