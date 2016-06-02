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

#include <ht_platform.h>
#include <ht_refcounted.h>

namespace Hatchit
{
    namespace Graphics
    {
        class RootLayoutBase;

        class HT_API RootLayout : public Core::RefCounted<RootLayout>
        {
        public:
            RootLayout(Core::Guid ID);
            
            ~RootLayout();

            RootLayoutBase* const GetBase() const;

            bool Initialize(const std::string& file);
            
        private:
            RootLayoutBase* m_base;
        };

        using RootLayoutHandle = Core::Handle<RootLayout>;
    }
}