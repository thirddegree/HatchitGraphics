/**
**    Hatchit Engine
**    Copyright(c) 2015-2016 ThirdDegree
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

#include <ht_rootlayout.h>
#include <ht_rootlayout_base.h>

namespace Hatchit
{
    namespace Graphics
    {

        RootLayout::RootLayout(Core::Guid ID) :
            Core::RefCounted<RootLayout>(std::move(ID))
        {
        
        }

        RootLayoutBase* const RootLayout::GetBase() const
        {
            return m_base;
        }

    }
}