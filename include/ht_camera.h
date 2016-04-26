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
* \class Camera
* \ingroup HatchitGraphics
*
* \brief A graphics-side camera object
*
* This is a fancy container for a view and a projection matrix
* as well as a bitfield describing which render layers this
* camera is a  part of.
*/

#pragma once

#include <ht_platform.h>
#include <ht_math.h>

namespace Hatchit {

    namespace Graphics {

        class HT_API Camera
        {
        public:
            Camera() = default;
            Camera(Math::Matrix4 view, Math::Matrix4 projection);
            ~Camera(void) = default;

            const Math::Matrix4& GetView() const;
            const Math::Matrix4& GetProjection() const;
            uint64_t GetLayerFlags() const;

            void SetView(Math::Matrix4 view);
            void SetProjection(Math::Matrix4 projection);

            void RegisterCamera();

        private:
            Math::Matrix4 m_view;
            Math::Matrix4 m_projection;
            uint64_t m_layerflags;
        };
    }
}
