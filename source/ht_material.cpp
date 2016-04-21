/**
**    Hatchit Engine
**    Copyright(c) 2015 Third-Degree
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

#include <ht_material.h> //material base
#include <ht_renderpass.h> //render pass base handle

namespace Hatchit {

    namespace Graphics {

        //TODO: This should probably just be inlined?
        const std::vector<RenderPassBaseHandle>& MaterialBase::GetRenderPasses() const 
        {
            return m_renderPasses;
        }

    }

}