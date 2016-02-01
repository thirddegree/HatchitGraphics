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
* \class IMaterial
* \ingroup HatchitGraphics
*
* \brief An interface for a material to draw objects with
*
* This class will be extended by a class that will implement its
* methods with ones that will make calls to a graphics language
*/

#pragma once

#include <ht_platform.h>
#include <ht_shader.h>

namespace Hatchit {

    namespace Graphics {

        class HT_API IMaterial
        {
        public:
			virtual ~IMaterial() { };
        };
    }
}
