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

#include <ht_texture_base.h>

namespace Hatchit
{
    namespace Graphics
    {
        /** Gets the width of the texture
        * \return Returns the texture width as a uint32_t
        */
        uint32_t TextureBase::GetWidth() const
        {
            return m_width;
        }

        /** Gets the height of the texture
        * \return Returns the texture height as a uint32_t
        */
        uint32_t TextureBase::GetHeight() const
        {
            return m_height;
        }
    }
}