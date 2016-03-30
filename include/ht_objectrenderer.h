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

/**
* \class IObjectRenderer
* \ingroup HatchitGraphics
*
* \brief An interface for a class that will render something to the screen
*
* This class will be extended by another interface that will describe methods
* for buffering and drawing a resource to the screen
*/

#pragma once

#include <ht_platform.h>

namespace Hatchit {

    namespace Graphics {

        class HT_API IObjectRenderer
        {
        public:
            virtual ~IObjectRenderer() {};

            ///Override to render a resource with a graphics language
            virtual void VRender() = 0;
            
        };
    }
}
