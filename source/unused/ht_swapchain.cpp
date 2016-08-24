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

#include <ht_swapchain.h>

namespace Hatchit 
{

    namespace Graphics 
    {

        /** Get the width of this swapchain's "screen"
        * \return The width of the swapchain's rendering area as a uint32_t
        */
        uint32_t SwapChain::GetWidth() const
        { 
            return m_width; 
        }

        /** Get the height of this swapchain's "screen"
        * \return The height of the swapchain's rendering area as a uint32_t
        */
        uint32_t SwapChain::GetHeight() const
        { 
            return m_height;
        }

    }
}
