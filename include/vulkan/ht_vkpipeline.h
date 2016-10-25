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
 * \file ht_vkpipeline.h
 * \brief VkPipeline class implementation
 * \author Jhonny Knaak de Vargas (lomaisdoro@gmail.com)
 *
 */

#pragma once

#include <ht_platform.h>
#include <ht_vkdevice.h>
#include <ht_vulkan.h>

namespace Hatchit
{
    namespace Graphics
    {
        namespace Vulkan
        {
            class HT_API VKPipeline
            {
            public:
                VKPipeline(VKDevice& pDevice);
                ~VKPipeline();

                bool Initialize(const VkPipelineLayoutCreateInfo& pCreateInfo);

                operator VkPipelineLayout();
                operator VkPipelineLayout*();
            private:
                VkPipelineLayout m_PipelineLayout;
                VkDevice m_Device;
            };
        }
    }
}
