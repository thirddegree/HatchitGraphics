/**
**    Hatchit Engine
**    Copyright(c) 2015-2016 Third-Degree
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

#include <ht_vkdevice.h>

namespace Hatchit
{
    namespace Graphics
    {
        namespace Vulkan
        {

            VKDevice::VKDevice() 
            {
                m_enabledLayerNames = {
                    "VK_LAYER_GOOGLE_threading",      "VK_LAYER_LUNARG_core_validation",
                    "VK_LAYER_LUNARG_object_tracker", "VK_LAYER_LUNARG_parameter_validation",
                    "VK_LAYER_LUNARG_standard_validation",  "VK_LAYER_LUNARG_swapchain",
                    "VK_LAYER_LUNARG_device_limits",  "VK_LAYER_LUNARG_image",
                    "VK_LAYER_GOOGLE_unique_objects",
                };

                m_validate = false;

                m_instance = VK_NULL_HANDLE;
            }

            VKDevice::~VKDevice() {}

            bool VKDevice::VInitialize() 
            {
                if (!setupInstance())
                    return false;

                if (!enumeratePhysicalDevices())
                    return false;

                if (!queryDeviceCapabilities())
                    return false;

                if (!setupDevices())
                    return false;

                if (!setupProcAddresses())
                    return false;

                return true;
            }

            void VKDevice::VReportDeviceInfo() {}

            void VKDevice::SetValidation(bool validate) { m_validate = validate; }

            const std::vector<VkDevice>&                            VKDevice::GetVKDevices() const { return m_devices; }
            const std::vector<VkPhysicalDevice>&                    VKDevice::GetVKPhysicalDevices() const { return m_gpus; }
            const std::vector<VkPhysicalDeviceFeatures>&            VKDevice::GetVKPhysicalDeviceFeatures() const { return m_gpuFeatures; }
            const std::vector<VkPhysicalDeviceMemoryProperties>&    VKDevice::GetVKPhysicalDeviceMemoryProperties() const { return m_gpuMemoryProps; }
            const VkInstance&                                       VKDevice::GetVKInstance() const { return m_instance; }

            /*
                Private methods
            */

            bool VKDevice::setupInstance() 
            {
                VkResult err;
                bool success = true;

                success = checkInstanceLayers();
                assert(success);
                if (!success)
                    return false;

                success = checkInstanceExtensions();
                assert(success);
                if (!success)
                    return false;

                VkApplicationInfo appInfo;
                appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
                appInfo.pNext = nullptr;
                appInfo.pApplicationName = "HatchitTest";
                appInfo.applicationVersion = 0;
                appInfo.pEngineName = "Hatchit";
                appInfo.engineVersion = 0;
                appInfo.apiVersion = VK_MAKE_VERSION(1, 0, 11);

                VkInstanceCreateInfo instanceInfo;
                instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
                instanceInfo.pNext = nullptr;
                instanceInfo.flags = 0;
                instanceInfo.pApplicationInfo = &appInfo;
                instanceInfo.enabledLayerCount = static_cast<uint32_t>(m_enabledLayerNames.size());
                instanceInfo.ppEnabledLayerNames = m_enabledLayerNames.data();
                instanceInfo.enabledExtensionCount = static_cast<uint32_t>(m_enabledExtensionNames.size());
                instanceInfo.ppEnabledExtensionNames = m_enabledExtensionNames.data();

                err = vkCreateInstance(&instanceInfo, nullptr, &m_instance);
                switch (err)
                {
                case VK_SUCCESS:
                    break;

                case VK_ERROR_INCOMPATIBLE_DRIVER:
                {
                    HT_ERROR_PRINTF("VKDevice::setupInstance: Cannot find a compatible Vulkan installable client driver"
                        "(ICD).\n\nPlease look at the Getting Started guide for "
                        "additional information.\n"
                        "vkCreateInstance Failure\n");
                } return false;

                case VK_ERROR_EXTENSION_NOT_PRESENT:
                {
                    //TODO: print something
                } return false;

                default:
                    return false;
                }

                return true;
            }

            bool VKDevice::enumeratePhysicalDevices()
            {
                VkResult err;
                uint32_t gpuCount = 0;

                err = vkEnumeratePhysicalDevices(m_instance, &gpuCount, nullptr);
                if (gpuCount <= 0 || err != VK_SUCCESS)
                {
                    HT_ERROR_PRINTF("VKDevice::enumeratePhysicalDevices: No compatible devices were found.\n");
                    return false;
                }

                m_gpus.resize(gpuCount);
                err = vkEnumeratePhysicalDevices(m_instance, &gpuCount, m_gpus.data());
                if (err)
                {
                    HT_ERROR_PRINTF("VKDevice::enumeratePhysicalDevices: Vulkan encountered error enumerating physical devices.\n");
                    return false;
                }

                return true;
            }

            bool VKDevice::queryDeviceCapabilities() 
            {
                //Get physical device memory props
                m_gpuMemoryProps.resize(m_gpus.size());

                for (size_t i = 0; i < m_gpus.size(); i++)
                {
                    VkPhysicalDevice gpu = m_gpus[i];
                    // Query fine-grained feature support for this device.
                    //  If app has specific feature requirements it should check supported
                    //  features based on this query
                    VkPhysicalDeviceFeatures gpuFeatures;
                    vkGetPhysicalDeviceFeatures(gpu, &gpuFeatures);

                    vkGetPhysicalDeviceMemoryProperties(gpu, &m_gpuMemoryProps[i]);

                    m_gpuFeatures.push_back(gpuFeatures);
                }

                return true;
            }

            bool VKDevice::setupDevices() 
            {
                VkResult err;
                bool success = true;

                m_devices.resize(m_gpus.size());

                for (size_t i = 0; i < m_gpus.size(); i++)
                {
                    VkPhysicalDevice gpu = m_gpus[i];

                    success = true;

                    success = checkDeviceLayers(gpu);
                    assert(success);
                    if (!success)
                        return false;

                    success = checkDeviceExtensions(gpu);
                    assert(success);
                    if (!success)
                        return false;

                    float queuePriorities[1] = { 0.0f };

                    VkDeviceQueueCreateInfo queue;
                    queue.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
                    queue.pNext = nullptr;
                    queue.queueFamilyIndex = 0; //TODO: Grab this index from the swapchain
                    queue.queueCount = 1;
                    queue.pQueuePriorities = queuePriorities;

                    VkDeviceCreateInfo device;
                    device.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
                    device.pNext = nullptr;
                    device.queueCreateInfoCount = 1;
                    device.pQueueCreateInfos = &queue;
                    device.enabledLayerCount = static_cast<uint32_t>(m_enabledLayerNames.size());
                    device.ppEnabledLayerNames = m_enabledLayerNames.data();
                    device.enabledExtensionCount = static_cast<uint32_t>(m_enabledExtensionNames.size());
                    device.ppEnabledExtensionNames = m_enabledExtensionNames.data();
                    device.pEnabledFeatures = nullptr; //Request specific features here

                    err = vkCreateDevice(gpu, &device, nullptr, &m_devices[i]);
                    if (err != VK_SUCCESS)
                    {
                        HT_DEBUG_PRINTF("Failed to create device. \n");
                        return false;
                    }
                }

                return true;
            }

            bool VKDevice::setupProcAddresses()
            {
                //Pointer to function to get function pointers from device
                PFN_vkGetDeviceProcAddr g_gdpa = (PFN_vkGetDeviceProcAddr)
                    vkGetInstanceProcAddr(m_instance, "vkGetDeviceProcAddr");

                //Set these up just to the first device
                fpCreateSwapchainKHR = (PFN_vkCreateSwapchainKHR)g_gdpa(m_devices[0], "vkCreateSwapchainKHR");
                fpDestroySwapchainKHR = (PFN_vkDestroySwapchainKHR)g_gdpa(m_devices[0], "vkDestroySwapchainKHR");
                fpGetSwapchainImagesKHR = (PFN_vkGetSwapchainImagesKHR)g_gdpa(m_devices[0], "vkGetSwapchainImagesKHR");
                fpAcquireNextImageKHR = (PFN_vkAcquireNextImageKHR)g_gdpa(m_devices[0], "vkAcquireNextImageKHR");
                fpQueuePresentKHR = (PFN_vkQueuePresentKHR)g_gdpa(m_devices[0], "vkQueuePresentKHR");

                fpGetPhysicalDeviceSurfaceSupportKHR = (PFN_vkGetPhysicalDeviceSurfaceSupportKHR)
                    vkGetInstanceProcAddr(m_instance, "vkGetPhysicalDeviceSurfaceSupportKHR");
                if (fpGetPhysicalDeviceSurfaceSupportKHR == nullptr)
                {
                    HT_DEBUG_PRINTF("VKDevice::setupProcAddresses: vkGetPhysicalDeviceSurfaceSupportKHR not found.\n");
                    return false;
                }

                fpGetPhysicalDeviceSurfaceCapabilitiesKHR = (PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR)
                    vkGetInstanceProcAddr(m_instance, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR");
                if (fpGetPhysicalDeviceSurfaceCapabilitiesKHR == nullptr)
                {
                    HT_DEBUG_PRINTF("VKDevice::setupProcAddresses: vkGetPhysicalDeviceSurfaceCapabilitiesKHR not found.\n");
                    return false;
                }

                fpGetPhysicalDeviceSurfaceFormatsKHR = (PFN_vkGetPhysicalDeviceSurfaceFormatsKHR)
                    vkGetInstanceProcAddr(m_instance, "vkGetPhysicalDeviceSurfaceFormatsKHR");
                if (fpGetPhysicalDeviceSurfaceFormatsKHR == nullptr)
                {
                    HT_DEBUG_PRINTF("VKDevice::setupProcAddresses: vkGetPhysicalDeviceSurfaceFormatsKHR not found.\n");
                    return false;
                }

                fpGetPhysicalDeviceSurfacePresentModesKHR = (PFN_vkGetPhysicalDeviceSurfacePresentModesKHR)
                    vkGetInstanceProcAddr(m_instance, "vkGetPhysicalDeviceSurfacePresentModesKHR");
                if (fpGetPhysicalDeviceSurfacePresentModesKHR == nullptr)
                {
                    HT_DEBUG_PRINTF("VKDevice::setupProcAddresses: vkGetPhysicalDeviceSurfacePresentModesKHR not found.\n");
                    return false;
                }

                return true;
            }

            bool VKDevice::checkInstanceLayers()
            {
                VkResult err;

                /**
                * Check the following requested Vulkan layers against available layers
                */
                VkBool32 validationFound = 0;
                uint32_t instanceLayerCount = 0;
                err = vkEnumerateInstanceLayerProperties(&instanceLayerCount, NULL);
                assert(!err);

                if (instanceLayerCount > 0)
                {
                    std::vector<VkLayerProperties> instanceLayers(instanceLayerCount);
                    err = vkEnumerateInstanceLayerProperties(&instanceLayerCount, instanceLayers.data());
                    assert(!err);

                    bool validated = checkLayers(m_enabledLayerNames, instanceLayers);

                    if (!validated)
                        return false;

                    return true;
                }

                HT_DEBUG_PRINTF("VKRenderer::checkInstanceLayers(), instanceLayerCount is zero. \n");
                return false;
            }

            bool VKDevice::checkInstanceExtensions()
            {
                VkResult err;

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
                        if (m_validate)
                        {
                            if (!strcmp(VK_EXT_DEBUG_REPORT_EXTENSION_NAME, instanceExtensions[i].extensionName)) {
                                m_enabledExtensionNames.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
                            }
                        }

                        assert(m_enabledExtensionNames.size() < 64);
                    }

                    delete[] instanceExtensions;

                    return true;
                }

                HT_ERROR_PRINTF("VKDevice::checkInstanceExtensions(), instanceExtensionCount is zero. \n");

                return false;
            }

            bool VKDevice::checkDeviceLayers(const VkPhysicalDevice& gpu)
            {
                VkResult err;

                uint32_t deviceLayerCount = 0;
                err = vkEnumerateDeviceLayerProperties(gpu, &deviceLayerCount, NULL);
                assert(!err);

                if (deviceLayerCount == 0)
                {
                    HT_DEBUG_PRINTF("VKRenderer::checkValidationLayers(): No layers were found on the device.\n");
                    return false;
                }

                std::vector<VkLayerProperties> deviceLayers(deviceLayerCount);
                err = vkEnumerateDeviceLayerProperties(gpu, &deviceLayerCount, deviceLayers.data());
                assert(!err);

                bool validated = checkLayers(m_enabledLayerNames, deviceLayers);

                if (!validated)
                {
                    HT_DEBUG_PRINTF("VkRenderer::checkValidationLayers(): Could not validate enabled layers against device layers.\n");
                    return false;
                }
                

                return true;
            }

            bool VKDevice::checkDeviceExtensions(const VkPhysicalDevice& gpu)
            {
                VkResult err;
                uint32_t deviceExtensionCount = 0;
                VkBool32 swapchainExtFound = 0;
                m_enabledExtensionNames.clear();

                //Check how many extensions are on the device
                err = vkEnumerateDeviceExtensionProperties(gpu, NULL, &deviceExtensionCount, NULL);
                assert(!err);

                if (deviceExtensionCount == 0)
                {
                    HT_ERROR_PRINTF("VKDevice::checkDeviceExtensions(): Device reported no available extensions\n");
                    return false;
                }

                //Get extension properties
                VkExtensionProperties* deviceExtensions = new VkExtensionProperties[deviceExtensionCount];
                err = vkEnumerateDeviceExtensionProperties(gpu, NULL, &deviceExtensionCount, deviceExtensions);
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
                    HT_ERROR_PRINTF("vkEnumerateDeviceExtensionProperties failed to find "
                        "the " VK_KHR_SWAPCHAIN_EXTENSION_NAME
                        " extension.\n\nDo you have a compatible "
                        "Vulkan installable client driver (ICD) installed?\nPlease "
                        "look at the Getting Started guide for additional "
                        "information.\n");
                    return false;
                }

                return true;
            }

            bool VKDevice::checkLayers(std::vector<const char*> layerNames, std::vector <VkLayerProperties> layers)
            {
                bool validated = true;
                for (size_t i = 0; i < layerNames.size(); i++)
                {
                    VkBool32 found = 0;
                    for (size_t j = 0; j < layers.size(); j++)
                    {
                        if (!strcmp(layerNames[i], layers[j].layerName))
                        {
                            found = true;
                            break;
                        }
                    }
                    if (!found)
                    {
                        HT_ERROR_PRINTF("VKDevice::checkLayers(), Cannot find layer: %s\n", layerNames[i]);
                        validated = false;
                    }

                }

                return validated;
            }

        }
    }
}