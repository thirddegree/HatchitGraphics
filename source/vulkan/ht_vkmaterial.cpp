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

#include <ht_vkmaterial.h>
#include <ht_vkshader.h>
#include <ht_vkrenderer.h>

#include <cassert>

namespace Hatchit {

    namespace Graphics {

        namespace Vulkan {

            VKMaterial::VKMaterial() { }
            VKMaterial::~VKMaterial() {}

            void VKMaterial::VOnLoaded()
            {
                //TODO: Read the material file and set the appropriate shader variables
            }
            bool VKMaterial::VInitFromFile(Core::File* file) { return true; }

            bool VKMaterial::VSetInt(std::string name, int data) { return true; }
            bool VKMaterial::VSetFloat(std::string name, float data) { return true; }
            bool VKMaterial::VSetFloat3(std::string name, Math::Vector3 data) { return true; }
            bool VKMaterial::VSetFloat4(std::string name, Math::Vector4 data) { return true; }
            bool VKMaterial::VSetMatrix4(std::string name, Math::Matrix4 data) { return true; }

            bool VKMaterial::VBindTexture(std::string name, ITexture* texture) { return true; }
            bool VKMaterial::VUnbindTexture(std::string name, ITexture* texture) { return true; }

            bool VKMaterial::VPrepare() 
            {
                return true;
            }
        }
    }
}
