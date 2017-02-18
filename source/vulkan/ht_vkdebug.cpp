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

#include <ht_vkdebug.h>
#include <cassert>

namespace Hatchit
{
    namespace Graphics
    {
        namespace Vulkan
        {
            VKDebug::VKDebug()
            {
                _CreateDebugReportCallback = VK_NULL_HANDLE;
                _DestroyDebugReportCallback = VK_NULL_HANDLE;
                _BreakCallback = VK_NULL_HANDLE;
                _Callback = VK_NULL_HANDLE;
            }

            VKDebug::~VKDebug()
            {

            }

            VKDebug& VKDebug::instance()
            {
                static VKDebug _instance;

                return _instance;
            }

            void VKDebug::SetupCallback(VkInstance instance,
                                        VkDebugReportFlagsEXT flags,
                                        VkDebugReportCallbackEXT callback)
            {
                VKDebug& _instance = VKDebug::instance();

                _instance._CreateDebugReportCallback = reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT"));
                _instance._DestroyDebugReportCallback = reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>(vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT"));
                _instance._BreakCallback = reinterpret_cast<PFN_vkDebugReportMessageEXT>(vkGetInstanceProcAddr(instance, "vkDebugReportMessageEXT"));

                VkDebugReportCallbackCreateInfoEXT info = {};
                info.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
                info.pfnCallback = reinterpret_cast<PFN_vkDebugReportCallbackEXT>(VKDebug::MessageCallback);
                info.flags = flags;

                VkResult err = _instance._CreateDebugReportCallback(instance,
                                &info,
                                nullptr, (callback != VK_NULL_HANDLE) ? &callback : &_instance._Callback);
                assert(!err);
            }

            void VKDebug::FreeCallback(VkInstance instance)
            {
                VKDebug& _instance = VKDebug::instance();

                if(_instance._Callback != VK_NULL_HANDLE)
                    _instance._DestroyDebugReportCallback(instance, _instance._Callback, nullptr);
            }

            VkBool32 VKDebug::MessageCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objType,
                                              uint64_t srcObject, size_t location, int32_t msgCode,
                                              const char *layerPrefix, const char *msg, void *userData)
            {
                using namespace Core;

                /**
                 * Determine log severity
                 */
                Debug::LogSeverity severity;

                if(flags & VK_DEBUG_REPORT_ERROR_BIT_EXT)
                    severity = Debug::LogSeverity::Error;
                if(flags & VK_DEBUG_REPORT_WARNING_BIT_EXT)
                    severity = Debug::LogSeverity::Warning;
                if(flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT)
                    severity = Debug::LogSeverity::Performance;
                if(flags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT)
                    severity = Debug::LogSeverity::Info;

                /**
                 * Diagnostic info from Vulkan loader and layers.
                 * This is usually not helpful for API issues.
                 */
                if(flags & VK_DEBUG_REPORT_DEBUG_BIT_EXT)
                    severity = Debug::LogSeverity::Debug;


                /**
                 * Output message based on log severity type
                 */
                switch(severity)
                {
                    case Debug::LogSeverity::Debug:
                    {
                        HT_DEBUG_PRINTF("[Layer: %s] [Code: %d] : %s\n", layerPrefix, msgCode, msg);
                    } break;

                    case Debug::LogSeverity::Warning:
                    {
                        HT_WARNING_PRINTF("[Layer: %s] [Code: %d] : %s\n", layerPrefix, msgCode, msg);
                    } break;

                    case Debug::LogSeverity::Error:
                    {
                        HT_ERROR_PRINTF("[Layer: %s] [Code: %d] : %s\n", layerPrefix, msgCode, msg);
                    } break;

                    case Debug::LogSeverity::Info:
                    {
                        HT_INFO_PRINTF("[Layer: %s] [Code: %d] : %s\n", layerPrefix, msgCode, msg);
                    } break;

                    case Debug::LogSeverity::Performance:
                    {
                        HT_PERF_PRINTF("[Layer: %s] [Code: %d] : %s\n", layerPrefix, msgCode, msg);
                    } break;

                    default: 
                        break;
                }

                /**
                 * This return value controls whether failing Vulkan API calls
                 * should about.
                 *
                 * VK_FALSE: no abort, VK_TRUE: abort
                 */
                return VK_FALSE;
            }

        }
    }
}