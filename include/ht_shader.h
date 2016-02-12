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
* \class IShader
* \ingroup HatchitGraphics
*
* \brief An interface for a class that will load a shader with a graphics language
*
* This will be extended by another class that will implement its methods to load
* and compile a shader with a given graphics language
*/

#pragma once

#include <ht_platform.h>
#include <ht_resourceobject.h>
#include <ht_texture.h>
#include <ht_string.h>

namespace Hatchit {

    namespace Graphics {

        class HT_API IShader : public Resource::ResourceObject
        {
			friend class IMaterial;

        public:
			virtual ~IShader() {};
        
			virtual void VOnLoaded() = 0;
			virtual void VCompile() = 0;

            virtual bool VSetData(std::string name, const void* data, size_t size) = 0;
            virtual bool VSetInt(std::string name, int data) = 0;
            virtual bool VSetFloat(std::string name, float data) = 0;
            virtual bool VSetFloat2(std::string name, const float data[2]) = 0;
            virtual bool VSetFloat2(std::string name, float x, float y) = 0;
            virtual bool VSetFloat3(std::string name, const float data[3]) = 0;
            virtual bool VSetFloat3(std::string name, float x, float y, float z) = 0;
            virtual bool VSetFloat4(std::string name, const float data[4]) = 0;
            virtual bool VSetFloat4(std::string name, float x, float y, float z, float w) = 0;
            virtual bool VSetMatrix4x4(std::string name, const float data[16]) = 0;

            virtual bool VBindTexture(std::string name, ITexture* texture) = 0;
            virtual bool VUnbindTexture(std::string name, ITexture* texture) = 0;
        };
    }
}
