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

#pragma once

/**
 * @file ht_vkapplication.h
 * @brief VKApplication class definition
 * @author Matt Guerrette (direct3Dtutorials@gmail.com)
 *
 * This file contains definition for VKApplication class
 */

#include <ht_platform.h>
#include <ht_vulkan.h>
#include <vector>

namespace Hatchit
{
    namespace Graphics
    {
        namespace Vulkan
        {
            class VKDevice;

            /**
             * @class VKApplication
             * @brief Defines a Vulkan application instance.
             * 
             *  A Vulkan application instance stores the per-application vulkan states.
             */
            class HT_API VKApplication
            {
            public:
                /**
                 * @brief Default constructor
                 */
                VKApplication();

                /**
                 * @brief Constructs application instance
                 * 
                 * Constructs Vulkan application instance with specified
                 * parameters from VkApplicationInfo structure
                 * 
                 * @param info Application parameters
                 */
                explicit VKApplication(const VkApplicationInfo& info);

                /**
                 * @brief Destructor
                 * 
                 * @note
                 *      Any Vulkan objects created using this
                 *      application instance MUST be destroyed
                 *      prior to calling this destructor.
                 */
                ~VKApplication();

                /**
                 * @brief Initialize application instance
                 * 
                 * Initializes Vulkan application instance
                 * 
                 * @param window  Handle to window
                 * @param display [Linux] Handle to Xlib display port
                 * @return [true] : success
                 * @return [false] : failure
                 */
                bool Initialize(void* window, void* display);

                /**
                 * @brief Utility to check if instance has been initialized
                 * 
                 * @return [true] : initialized
                 * @return [false] : unitialized
                 */
                bool IsValid() const;

                /**
                 * @brief Gets instance name
                 * 
                 * @returns name
                 */
                std::string   Name()              const;

                /**
                 * @brief Gets instance version
                 * 
                 * @returns version
                 */
                uint32_t      Version()           const;

                /**
                 * @brief Gets instance engine name
                 * 
                 * @returns engine name
                 */
                std::string   EngineName()        const;

                /**
                 * @brief Gets instance engine version
                 * 
                 * @returns engine version
                 */
                uint32_t      EngineVersion()     const;

                /**
                 * @brief Gets instance API version
                 * 
                 * @returns API version
                 */
                uint32_t      APIVersion()        const;

                /**
                 * @brief Gets enabled layer count
                 * 
                 * @returns enabled layer count
                 */
                uint32_t      EnabledLayerCount() const;

                /**
                 * @brief Gets enabled extension count
                 * 
                 * @returns enabled extension count
                 */
                uint32_t      EnabledExtensionCount() const;

                /**
                 * @brief Gets enabled layer names
                 * 
                 * @returns enabled layer names
                 */
                const std::vector<std::string>& EnabledLayerNames() const;

                /**
                 * @brief Gets enabled extension names
                 * 
                 * @returns enabled extension names
                 */
                const std::vector<std::string>& EnabledExtensionNames() const;

                /**
                 * @brief Gets native window handle
                 * 
                 * @returns native window handle
                 */
                void* NativeWindow() const;
                
                /**
                 * @brief Gets native display handle [Linux]
                 * 
                 * @returns native display handle
                 * 
                 */
                void* NativeDisplay() const;

                /**
                 * @brief Explicit cast from VKApplication to VkInstance
                 * 
                 * @return underlying VkInstance handle
                 */
                explicit operator VkInstance() const;

            private:
                VkInstance                          m_instance;
                VkApplicationInfo                   m_info;
                VkSurfaceKHR                        m_window;
                void*                               m_nativeWindow;
                void*                               m_nativeDisplay;
                std::vector<std::string>            m_layers;
                std::vector<std::string>            m_extensions;

                
                bool CheckInstanceLayers();
                bool CheckInstanceExtensions();
            };
        }
    }
}