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

#pragma once

/**
 * \file ht_vkdebug.h
 * \brief VKDebug class definition
 * \author Matt Guerrette (direct3Dtutorials@gmail.com)
 * 
 * This file contains definition for VKDebug class
 */

#include <ht_platform.h>
#include <ht_vulkan.h>

namespace Hatchit
{
    namespace Graphics
    {
        namespace Vulkan
        {
            /**
             * \class VKDebug
             * \brief Defines a Vulkan debug wrapper singleton
             * 
             * Vulkan debug wrapper that initializes the debug
             * callback functions
             */
            class HT_API VKDebug
            {
            public:
                /**
                 * \brief Instance function for singleton access
                 * \returns VKDebug class instance
                 */
                static VKDebug& instance();

                /**
                 * \brief Initializes the Vulkan debug callback
                 */
                static void SetupCallback(VkInstance instance,
                                          VkDebugReportFlagsEXT flags,
                                          VkDebugReportCallbackEXT callback);

                /**
                 * \brief Unintializes the VUlkan debug callback
                 */
                static void FreeCallback(VkInstance instance);

            private:
                VKDebug();

                ~VKDebug();

                PFN_vkCreateDebugReportCallbackEXT   _CreateDebugReportCallback;
                PFN_vkDestroyDebugReportCallbackEXT  _DestroyDebugReportCallback;
                PFN_vkDebugReportMessageEXT          _BreakCallback;
                VkDebugReportCallbackEXT             _Callback;


                
                static VkBool32 MessageCallback(VkDebugReportFlagsEXT flags,
                                                VkDebugReportObjectTypeEXT objType,
                                                uint64_t srcObject,
                                                size_t location,
                                                int32_t msgCode,
                                                const char* layerPrefix,
                                                const char* msg,
                                                void* userData);
            };
        }
    }
}