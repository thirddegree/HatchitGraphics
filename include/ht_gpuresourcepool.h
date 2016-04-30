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

#pragma once

#include <ht_platform.h>
#include <ht_texture.h>
#include <ht_singleton.h>
#include <ht_device.h>
#include <ht_material.h>
#include <ht_pipeline.h>

namespace Hatchit
{
    namespace Graphics
    {

        class IGPUResourceThread;

        class HT_API GPUResourcePool : public Core::Singleton<GPUResourcePool>
        {
        public:
            static bool             Initialize(IDevice* device);
            static void             DeInitialize();
            static void             RequestTextureAsync(TextureHandle _default, TextureHandle temporary, std::string file);
            static void             RequestMaterialAsync(MaterialHandle _default, MaterialHandle temporary, std::string file);


        private:
            IGPUResourceThread* m_thread;
            IDevice*            m_device;
            
            std::map<std::string, TextureHandle> m_defaultTextures;
        };
    }
}
