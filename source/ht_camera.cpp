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
* \class IMesh
* \ingroup HatchitGraphics
*
* \brief An interface to a mesh that exists on the GPU
*
* You must pass this interface a Resource::Mesh which is a collection
* of data that you want buffered onto the graphics card
*/

#include <ht_camera.h>
#include <ht_renderer.h>
namespace Hatchit {
    namespace Graphics {
        Camera::Camera(Math::Matrix4 view, Math::Matrix4 projection)
        {
            m_view = view;
            m_projection = projection;
        }

        uint32_t Camera::GetLayerFlags()
        {
            return m_layerflags;
        }
        
        void Camera::RegisterCamera()
        {
            IRenderer::Instance->RegisterCamera(*this, m_layerflags);
        }

        void Camera::SetView(Math::Matrix4 view)
        {
            m_view = view;
        }

        void Camera::SetProjection(Math::Matrix4 projection)
        {
            m_projection = projection;
        }
    }
}