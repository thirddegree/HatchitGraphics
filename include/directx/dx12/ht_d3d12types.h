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

#pragma once

#include <ht_directx.h>
#include <ht_string.h>
#include <cstdint>


namespace Hatchit {

    namespace Graphics {

        namespace DX {

            struct ConstantBuffer
            {
                uint32_t		bindIndex;
                ID3D12Buffer*   buffer;
                BYTE*			data;
                std::string     id;
            };

            struct ConstantBufferVariable
            {
                uint32_t		byteOffset;
                size_t			size;
                uint32_t		constantBufferIndex;
            };

        }
    }
}