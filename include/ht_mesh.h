#pragma once

#include <glm/glm.hpp>

namespace Hatchit
{
    namespace Graphics
    {
        class Vertex
        {
        public:
            glm::vec3 Position;
            glm::vec3 Normal;
            glm::vec3 Color;

            Vertex(const glm::vec3& pPosition, const glm::vec3& pNormal, const glm::vec3& pColor)
                : Position{pPosition}, Normal{pNormal}, Color{pColor} {}
        };
    }
}
