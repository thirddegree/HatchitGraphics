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
#include <ht_resourceobject.h>
#include <ht_shadervariable.h>
#include <ht_math.h>
#include <ht_debug.h>

#include <map>

namespace Hatchit {

    namespace Graphics {

        class IPipeline;

        class HT_API IMaterial : Resource::ResourceObject
        {
        public:
            virtual ~IMaterial() { };

            virtual bool VInitFromFile(Core::File* file) = 0;

            virtual bool VSetInt(std::string name, int data) = 0;
            virtual bool VSetFloat(std::string name, float data) = 0;
            virtual bool VSetFloat3(std::string name, Math::Vector3 data) = 0;
            virtual bool VSetFloat4(std::string name, Math::Vector4 data) = 0;
            virtual bool VSetMatrix4(std::string name, Math::Matrix4 data) = 0;

            virtual bool VBindTexture(std::string name, ITexture* texture) = 0;
            virtual bool VUnbindTexture(std::string name, ITexture* texture) = 0;

            virtual bool VPrepare(IPipeline* pipeline) = 0;
            virtual bool VUpdate() = 0;

            inline IPipeline* GetPipeline() 
            {
                return m_pipeline;
            }

        protected:
            std::map<std::string, ShaderVariable*> m_shaderVariables;
            std::map<std::string, ITexture*> m_textures;

            IPipeline* m_pipeline;
        };
    }
}
