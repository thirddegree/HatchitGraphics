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
#include <ht_resource.h>
#include <ht_string.h>

#include <ht_shader_base.h>

namespace Hatchit {

    namespace Graphics {


        class HT_API Shader : public Core::RefCounted<Shader>
        {
        public:
            Shader(Core::Guid ID);

            ~Shader();

            bool Initialize(const std::string& file);

            ShaderBase* const GetBase() const;

        private:
            ShaderBase* m_base;
        };

        using ShaderHandle = Core::Handle<Shader>;
    }
}
