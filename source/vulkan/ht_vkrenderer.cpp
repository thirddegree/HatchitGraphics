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

#include <ht_vkrenderer.h>
#include <ht_debug.h>

#include <cassert>

#ifdef _WIN32
#include <windows.h>
#endif

namespace Hatchit {

	namespace Graphics {

        namespace Vulkan {

            VKRenderer::VKRenderer()
            {
				m_swapchain = 0;
				m_commandBuffer = 0;
            }

            VKRenderer::~VKRenderer()
            {
                vkDestroyInstance(m_instance, nullptr);
            }

            bool VKRenderer::VInitialize(const RendererParams & params)
            {
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

				/*
				* Prepare Vulkan command buffers and memory systems for drawing
				*/
				if (!prepareVulkan(params))
					return false;

                return true;
            }

            void VKRenderer::VDeInitialize()
            {
            }

            void VKRenderer::VResizeBuffers(uint32_t width, uint32_t height)
            {
            }

            void VKRenderer::VSetClearColor(const Color & color)
            {
				m_clearColor.color.float32[0] = color.r;
				m_clearColor.color.float32[1] = color.g;
				m_clearColor.color.float32[2] = color.b;
				m_clearColor.color.float32[3] = color.a;
            }

            void VKRenderer::VClearBuffer(ClearArgs args)
            {
            }

            void VKRenderer::VPresent()
            {
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
				instance = (HINSTANCE)GetWindowLongPtr(window, DWLP_USER);

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


                    bool validated = checkLayers(m_enabledLayerNames, instanceLayers, instanceLayerCount);

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

                bool validated = checkLayers(m_enabledLayerNames, deviceLayers, deviceLayerCount);
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

            bool VKRenderer::checkLayers(std::vector<const char*> layerNames, VkLayerProperties * layers, uint32_t layerCount)
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
					m_format = VK_FORMAT_B8G8R8A8_UNORM;
				else
					m_format = surfaceFormats[0].format;

				m_colorSpace = surfaceFormats[0].colorSpace;

				return true;
			}

			bool VKRenderer::prepareVulkan(const RendererParams& params)
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

				VkCommandBufferAllocateInfo allocateInfo;
				allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
				allocateInfo.pNext = nullptr;
				allocateInfo.commandPool = m_commandPool;
				allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
				allocateInfo.commandBufferCount = 1;

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
				* Prepare the descriptor layout
				*/
				if (!prepareDescriptorLayout())
					return false;

				/*
				* Prepare the render pass
				*/
				if (!prepareRenderPass())
					return false;

				/*
				* Prepare pipeline
				*/
				if (!preparePipeline())
					return false;

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
				swapchainInfo.imageFormat = m_format;
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

					VkImageViewCreateInfo colorImageView;
					colorImageView.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
					colorImageView.pNext = nullptr;
					colorImageView.format = m_format;
					colorImageView.components = components;
					colorImageView.subresourceRange = subresourceRange;
					colorImageView.viewType = VK_IMAGE_VIEW_TYPE_2D;
					colorImageView.flags = 0;

					SwapchainBuffers buffer;
					buffer.image = swapchainImages[i];

					//Render loop will expect image to have been used before
					//Init image ot the VK_IMAGE_ASPECT_COLOR_BIT state
					setImageLayout(buffer.image, VK_IMAGE_ASPECT_COLOR_BIT, 
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

			bool VKRenderer::prepareSwapchainDepth() { return true; }
			bool VKRenderer::prepareDescriptorLayout() { return true; }
			bool VKRenderer::prepareRenderPass() { return true; }
			bool VKRenderer::preparePipeline() { return true; }
			bool VKRenderer::prepareDescriptorPool() { return true; }
			bool VKRenderer::prepareDescriptorSet() { return true; }
			bool VKRenderer::prepareFrambuffers() { return true; }

			bool VKRenderer::setImageLayout(VkImage image, VkImageAspectFlags aspectMask,
				VkImageLayout oldImageLayout, VkImageLayout newImageLayout)
			{
				VkResult err;

				//Start up a basic command buffer if we don't have one already
				if (m_commandBuffer == VK_NULL_HANDLE)
				{
					VkCommandBufferAllocateInfo command;
					command.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
					command.pNext = nullptr;
					command.commandPool = m_commandPool;
					command.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
					command.commandBufferCount = 1;

					err = vkAllocateCommandBuffers(m_device, &command, &m_commandBuffer);
					if(err != VK_SUCCESS)
					{
#ifdef _DEBUG
						Core::DebugPrintF("VKRenderer::setImageLayout(): Failed to allocate command buffer.\n");
#endif
						return false;
					}

					VkCommandBufferInheritanceInfo commandBufferHInfo;
					commandBufferHInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
					commandBufferHInfo.pNext = nullptr;
					commandBufferHInfo.renderPass = VK_NULL_HANDLE;
					commandBufferHInfo.subpass = 0;
					commandBufferHInfo.framebuffer = VK_NULL_HANDLE;
					commandBufferHInfo.occlusionQueryEnable = VK_FALSE;
					commandBufferHInfo.queryFlags = 0;
					commandBufferHInfo.pipelineStatistics = 0;

					VkCommandBufferBeginInfo commandBufferBeginInfo;
					commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
					commandBufferBeginInfo.pNext = nullptr;
					commandBufferBeginInfo.flags = 0;
					commandBufferBeginInfo.pInheritanceInfo = &commandBufferHInfo;

					err = vkBeginCommandBuffer(m_commandBuffer, &commandBufferBeginInfo);
					if (err != VK_SUCCESS)
					{
#ifdef _DEBUG
						Core::DebugPrintF("VKRenderer::setImageLayout(): Failed to begin command buffer.\n");
#endif
						return false;
					}
				}

				VkImageMemoryBarrier imageMemoryBarrier;
				imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
				imageMemoryBarrier.pNext = nullptr;
				imageMemoryBarrier.srcAccessMask = 0;
				imageMemoryBarrier.dstAccessMask = 0;
				imageMemoryBarrier.oldLayout = oldImageLayout;
				imageMemoryBarrier.newLayout = newImageLayout;
				imageMemoryBarrier.image = image;
				imageMemoryBarrier.subresourceRange = { aspectMask, 0, 1, 0, 1 };
				imageMemoryBarrier.srcQueueFamilyIndex = 0;
				imageMemoryBarrier.dstQueueFamilyIndex = 0;

				if (newImageLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
					// Make sure anything that was copying from this image has completed
					imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
				}

				if (newImageLayout == VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL) {
					imageMemoryBarrier.dstAccessMask =
						VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
				}

				if (newImageLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
					imageMemoryBarrier.dstAccessMask =
						VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
				}

				if (newImageLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
					// Make sure any Copy or CPU writes to image are flushed
					imageMemoryBarrier.dstAccessMask =
						VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
				}

				VkPipelineStageFlags srcStages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
				VkPipelineStageFlags destStages = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;

				vkCmdPipelineBarrier(m_commandBuffer, srcStages, destStages, 0, 0, nullptr, 0,
					nullptr, 1, &imageMemoryBarrier);

				return true;
			}

			void VKRenderer::flushCommandBuffer() {}

            VKAPI_ATTR VkBool32 VKAPI_CALL VKRenderer::debugFunction(VkFlags msgFlags, VkDebugReportObjectTypeEXT objType,
                uint64_t srcObject, size_t location, int32_t msgCode,
                const char *pLayerPrefix, const char *pMsg, void *pUserData)
            {
                size_t messageSize = strlen(pMsg) + 100;
                char* message = new char[messageSize];

                assert(message);

                if (msgFlags & VK_DEBUG_REPORT_ERROR_BIT_EXT)
                {
                    sprintf(message, "ERROR: [%s] Code %d : %s", pLayerPrefix, msgCode,
                        pMsg);
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
                    sprintf(message, "WARNING: [%s] Code %d : %s", pLayerPrefix, msgCode, pMsg);
                }
                else {
                    return false;
                }

#ifdef _DEBUG
                Core::DebugPrintF("%s\n", message);
#endif
                delete[] message;

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