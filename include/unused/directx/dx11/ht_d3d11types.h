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
#include <cstdint>
#include <ht_string.h>

namespace Hatchit {

	namespace Graphics {

        namespace DirectX {

            struct ConstantBuffer
            {
                uint32_t		bindIndex;
                ID3D11Buffer*   buffer;
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