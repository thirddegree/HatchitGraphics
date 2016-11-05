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

#include <ht_vkpipelinecache.h>

namespace Hatchit
{
    namespace Graphics
    {
        namespace Vulkan
        {
            VKPipelineCache::VKPipelineCache() : m_PipelineCache{VK_NULL_HANDLE}
            {
            }

            VKPipelineCache::~VKPipelineCache()
            {
                vkDestroyPipelineCache(m_Device, m_PipelineCache, nullptr);
            }

            bool VKPipelineCache::Initialize(VKDevice &pDevice, VkPipelineCacheCreateInfo &pCreateInfo)
            {
                m_Device = pDevice;

                VkResult err = VK_SUCCESS;

                err = vkCreatePipelineCache(m_Device, &pCreateInfo, nullptr, &m_PipelineCache);

                if ( err != VK_SUCCESS )
                {
                    HT_ERROR_PRINTF("VKPipelineCache::Initialize(): Failed to create pipelinecache. %s\n", VKErrorString(err));
                    return false;
                }

                return true;
            }

            VKPipelineCache::operator VkPipelineCache()
            {
                return m_PipelineCache;
            }

            VKPipelineCache::operator VkPipelineCache*()
            {
                return &m_PipelineCache;
            }
        }
    }
}
