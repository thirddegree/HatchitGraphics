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

/**
 * \file ht_vkpipelinecache.h
 * \brief VKPipelineCache class implementation
 * \author Jhonny Knaak de Vargas (lomaisdoro@gmail.com)
 *
 */

#pragma once

#include <ht_platform.h>
#include <ht_vulkan.h>
#include <ht_vkdevice.h>

namespace Hatchit
{
    namespace Graphics
    {
        namespace Vulkan
        {
            class VKPipelineCache
            {
            public:
                VKPipelineCache();
                ~VKPipelineCache();

                bool Initialize(VKDevice& pDevice, VkPipelineCacheCreateInfo& pCreateInfo);

                operator VkPipelineCache();
                operator VkPipelineCache*();
            private:
                VkPipelineCache m_PipelineCache;
                VKDevice m_Device;
            };
        }
    }
}
