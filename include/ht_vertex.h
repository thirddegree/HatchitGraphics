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
 * \file ht_mesh.h
 * \brief Some new structures for mesh
 * \author Jhonny Knaak de Vargas (lomaisdoro@gmail.com)
 *
 */

#pragma once

#include <glm/glm.hpp>

namespace Hatchit
{
    namespace Graphics
    {
        class Vertex
        {
        public:
            Vertex(const glm::vec3& pPosition, const glm::vec3& pNormal, const glm::vec3& pColor)
                : Position{pPosition}, Normal{pNormal}, Color{pColor} {}

            glm::vec3 Position;
            glm::vec3 Normal;
            glm::vec3 Color;
        };
    }
}
