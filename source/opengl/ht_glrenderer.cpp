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

#include <ht_glrenderer.h>
#include <ht_debug.h>

namespace Hatchit {

    namespace Graphics {

        namespace OpenGL {

            GLRenderer::GLRenderer()
            {

            }

            GLRenderer::~GLRenderer()
            {

            }

            bool GLRenderer::VInitialize(const RendererParams& params)
            {
                m_params = params;

                /*Initialize GLEW*/
                GLenum glewErr = glewInit();
                if (glewErr != GLEW_OK)
                {
#ifdef _DEBUG
                    Core::DebugPrintF("[GLRenderer]--Failed to initialize GLEW. Exiting.\n");
#endif
                    return false;
                }

                VSetClearColor(m_params.clearColor);

                return true;
            }

            void GLRenderer::VDeInitialize()
            {

            }

            void GLRenderer::VSetClearColor(const Color& color)
            {
                m_params.clearColor = color;

                glClearColor(color.r, color.g, color.b, color.a);
            }

            void GLRenderer::VClearBuffer(ClearArgs args)
            {
                switch (args)
                {
                case ClearArgs::Color:
                    glClear(GL_COLOR_BUFFER_BIT);
                    break;
                case ClearArgs::Depth:
                    glClear(GL_DEPTH_BUFFER_BIT);
                    break;
                case ClearArgs::Stencil:
                    glClear(GL_STENCIL_BUFFER_BIT);
                    break;
                case ClearArgs::ColorDepth:
                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
                    break;
                case ClearArgs::ColorStencil:
                    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
                    break;
                case ClearArgs::ColorDepthStencil:
                    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
                    break;

                default:
                    break;
                }
            }

            void GLRenderer::VPresent()
            {

            }

            void GLRenderer::VResizeBuffers(uint32_t width, uint32_t height)
            {

            }
        }

    }

}
