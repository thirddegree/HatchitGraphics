/**
**    Hatchit Engine
**    Copyright(c) 2015-2017 Third-Degree
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

/**
 * @file ht_vkapplication.cpp
 * @brief VKApplication class implementation
 * @author Matt Guerrette (direct3Dtutorials@gmail.com)
 *
 * This file contains the implementation for VKApplication class
 */

#include <ht_vkapplication.h>
#include <ht_vkdebug.h>
#include <cassert>
#include <algorithm>
#include <iterator>

namespace Hatchit
{
    namespace Graphics
    {
        namespace Vulkan {
            VKApplication::VKApplication():
                m_window(VK_NULL_HANDLE),
                m_nativeWindow(nullptr),
                m_nativeDisplay(nullptr)
            {
                m_instance = VK_NULL_HANDLE;
                m_info = {};


                m_layers = {
                    // This is a meta layer that enables all of the standard
                    // validation layers in the correct order :
                    // threading, parameter_validation, device_limits, object_tracker, image, core_validation, swapchain, and unique_objects
                    "VK_LAYER_LUNARG_standard_validation"
                };
            }

            VKApplication::VKApplication(const VkApplicationInfo &info):
                m_window(VK_NULL_HANDLE),
                m_nativeWindow(nullptr),
                m_nativeDisplay(nullptr)
            {
                m_instance = VK_NULL_HANDLE;
                m_info = info;
                m_window = VK_NULL_HANDLE;



                m_layers = {
                    // This is a meta layer that enables all of the standard
                    // validation layers in the correct order :
                    // threading, parameter_validation, device_limits, object_tracker, image, core_validation, swapchain, and unique_objects
                    "VK_LAYER_LUNARG_standard_validation"
                };


            }

            VKApplication::~VKApplication() 
            {
                VKDebug::FreeCallback(m_instance);

                /**
                 * Destroy Vulkan instance
                 */
                vkDestroyInstance(m_instance, nullptr);
            }

            bool VKApplication::Initialize(void* window, void* display) {


                m_nativeWindow = window;
                m_nativeDisplay = display;
                
                /**
                 * Verify requested Vulkan instance layers
                 * are available
                 */
                if (!CheckInstanceLayers())
                    return false;

                /**
                 * Verify required extension layers are
                 * available
                 */
                if (!CheckInstanceExtensions())
                    return false;

                m_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
                m_info.pNext = nullptr;
                m_info.pApplicationName = "Test";
                m_info.applicationVersion = 0;
                m_info.pEngineName = "Hatchit";
                m_info.engineVersion = 0;
                m_info.apiVersion = VK_API_VERSION_1_0;

                /**
                 * Attempt to create application instance
                 * using requested layers
                 */
                VkInstanceCreateInfo cInfo;
                cInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
                cInfo.pNext = nullptr;
                cInfo.flags = 0;
                cInfo.pApplicationInfo = &m_info;
                cInfo.enabledLayerCount = static_cast<uint32_t>(m_layers.size());
                std::vector<const char *> layers;
                std::transform(m_layers.begin(), m_layers.end(), std::back_inserter(layers), [](const std::string &s) {
                    return s.c_str();
                });
                cInfo.ppEnabledLayerNames = layers.data();
                cInfo.enabledExtensionCount = static_cast<uint32_t>(m_extensions.size());
                std::vector<const char *> extensions;
                std::transform(m_extensions.begin(), m_extensions.end(), std::back_inserter(extensions),
                               [](const std::string &s) {
                                   return s.c_str();
                               });
                cInfo.ppEnabledExtensionNames = extensions.data();

                VkResult err = vkCreateInstance(&cInfo, nullptr, &m_instance);

                if(err != VK_SUCCESS) {
                    HT_ERROR_PRINTF("VKAppication::Initialize(): Failed to create VkInstance\n");
                    return false;
                }


                VKDebug::SetupCallback(m_instance, VK_DEBUG_REPORT_ERROR_BIT_EXT |
                                                   VK_DEBUG_REPORT_WARNING_BIT_EXT, VK_NULL_HANDLE);

                return true;
            }

            bool VKApplication::IsValid() const
            {
                return m_instance != VK_NULL_HANDLE;
            }

            std::string VKApplication::Name() const 
            {
                return m_info.pApplicationName;
            }

            uint32_t VKApplication::Version() const 
            {
                return m_info.applicationVersion;
            }

            std::string VKApplication::EngineName() const 
            {
                return m_info.pEngineName;
            }

            uint32_t VKApplication::EngineVersion() const 
            {
                return m_info.engineVersion;
            }

            uint32_t VKApplication::APIVersion() const 
            {
                return m_info.apiVersion;
            }

            uint32_t VKApplication::EnabledLayerCount() const 
            {
                return static_cast<uint32_t>(m_layers.size());
            }

            uint32_t VKApplication::EnabledExtensionCount() const 
            {
                return static_cast<uint32_t>(m_extensions.size());
            }

            const std::vector<std::string>& VKApplication::EnabledLayerNames() const
            {
                return m_layers;
            }

            const std::vector<std::string>& VKApplication::EnabledExtensionNames() const 
            {
                return m_extensions;
            }

            VKApplication::operator VkInstance() const
            {
                return m_instance;
            }

            void* VKApplication::NativeWindow() const
            {
                return m_nativeWindow;
            }

            void* VKApplication::NativeDisplay() const
            {
                return m_nativeDisplay;
            }

            bool VKApplication::CheckInstanceLayers() 
            {

                VkResult err;

                /**
                 * Check the requested Vulkan layers against available
                 */
                uint32_t instanceLayerCnt = 0;
                err = vkEnumerateInstanceLayerProperties(&instanceLayerCnt, nullptr);

                std::vector<VkLayerProperties> instanceLayers(instanceLayerCnt);
                err = vkEnumerateInstanceLayerProperties(&instanceLayerCnt, instanceLayers.data());
                assert(!err);

                /**
                 * Iterate over requested layers and compare with
                 * layers available
                 */
                for (auto layer : m_layers) {
                    bool found = false;

                    for (auto &layerProp : instanceLayers) {
                        if (layer == layerProp.layerName)
                            found = true;
                    }

                    if (!found) {
                        HT_ERROR_PRINTF("VKApplication::CheckInstanceLayers() Cannot Find Layer: %s\n", layer);
                        return false;
                    }
                }

                return true;
            }

            bool VKApplication::CheckInstanceExtensions() 
            {
                VkResult err = VK_SUCCESS;

                uint32_t instanceExtCnt = 0;
                err = vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtCnt, nullptr);
                assert(!err);

                std::vector<VkExtensionProperties> instanceExtensions(instanceExtCnt);
                err = vkEnumerateInstanceExtensionProperties(nullptr, &instanceExtCnt, instanceExtensions.data());
                assert(!err);

                for (auto &ext : instanceExtensions) {
                    if (!strcmp(VK_KHR_SURFACE_EXTENSION_NAME, ext.extensionName))
                        m_extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);

#ifdef HT_SYS_WINDOWS
                    if (!strcmp(VK_KHR_WIN32_SURFACE_EXTENSION_NAME, ext.extensionName))
                        m_extensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#elif defined(HT_SYS_LINUX)
                    if (!strcmp(VK_KHR_XLIB_SURFACE_EXTENSION_NAME, ext.extensionName))
                        m_extensions.push_back(VK_KHR_XLIB_SURFACE_EXTENSION_NAME);
#endif

                    /**
                     * Enable validation extension layer
                     */
                    if (!strcmp(VK_EXT_DEBUG_REPORT_EXTENSION_NAME, ext.extensionName))
                        m_extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
                }

                return true;
            }
        }
    }
}