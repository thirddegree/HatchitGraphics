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

#include <ht_camera.h>      //Camera
#include <ht_renderer.h>    //Renderer
#include <ht_math.h>        //Math::Matrix4
#include <cstdint>          //uint64_t

namespace Hatchit 
{
    namespace Graphics 
    {
        Camera::Camera(Math::Matrix4 view, Math::Matrix4 projection)
        {
            m_view = view;
            m_projection = projection;
            m_layerflags = 0;
        }

        /** Gets the view matrix
        * \return A reference to the camera's view matrix as a Matrix4
        */
        const Math::Matrix4& Camera::GetView() const
        {
            return m_view;
        }

        /** Gets the projection matrix
        * \return A reference to the camera's projection matrix as a Matrix4
        */
        const Math::Matrix4& Camera::GetProjection() const
        {
            return m_projection;
        }

        /** Get the flags describing which render layers this camera is on
        * \return A 64-bit bitflag representing the render layers
        */
        uint64_t Camera::GetLayerFlags() const
        {
            return m_layerflags;
        }

        /** Sets the view matrix of the camera
        * \param view A Math::Matrix4 describing a view matrix
        */
        void Camera::SetView(Math::Matrix4 view)
        {
            m_view = view;
        }

        /** Sets the projection matrix of the camera
        * \param view A Math::Matrix4 describing a projection matrix
        */
        void Camera::SetProjection(Math::Matrix4 projection)
        {
            m_projection = projection;
        }

        /** Sets the layer flags of the camera
        * \param flags A 64 bit integer containing the flags that this camera is a part of
        */
        void Camera::SetLayerFlags(uint64_t flags) 
        {
            m_layerflags = flags;
        }

        /** Register this camera to the renderer based on its layer flags
        * \param renderer The Renderer to register this camera to
        */
        void Camera::RegisterCamera(Renderer& renderer)
        {
            renderer.RegisterCamera(*this);
        }

    }

}