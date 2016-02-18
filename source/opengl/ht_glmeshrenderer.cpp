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

#define BUFFER_OFFSET(i) ((char *)nullptr + (i))

#include <ht_glmeshrenderer.h>

using namespace Hatchit::Resource;

namespace Hatchit {

	namespace Graphics {

        namespace OpenGL {

            GLMeshRenderer::GLMeshRenderer(MeshPtr mesh, GLMaterial* material)
            {
                //this->mesh = mesh;
                this->material = material;
            }
            GLMeshRenderer::~GLMeshRenderer()
            {
                VFree();
            };

            void GLMeshRenderer::VBuffer()
            {
                // Get sizes and vertex collections
                std::vector<aiVector3D> meshVerts = mesh->getVertices();
                std::vector<aiVector3D> meshNormals = mesh->getNormals();
                std::vector<aiVector2D> meshUVs = mesh->getUVs();

                std::vector<int> meshIndices = mesh->getIndices();

                size_t vertSize = meshVerts.size() * 3 * sizeof(float);
                size_t normalSize = meshNormals.size() * 3 * sizeof(float);
                size_t uvSize = meshUVs.size() * 2 * sizeof(float);

                size_t indicesSize = meshIndices.size() * sizeof(int);

                //Setup VAO
                glGenVertexArrays(1, &vao);
                glBindVertexArray(vao);

                //Setup VBO
                glGenBuffers(1, &vbo);
                glBindBuffer(GL_ARRAY_BUFFER, vbo);

                //Buffer data
                glBufferData(GL_ARRAY_BUFFER, vertSize + normalSize + uvSize, 0, GL_STATIC_DRAW);
                glBufferSubData(GL_ARRAY_BUFFER, 0, vertSize, &meshVerts[0]);
                glBufferSubData(GL_ARRAY_BUFFER, 0, normalSize, &meshNormals[0]);
                glBufferSubData(GL_ARRAY_BUFFER, 0, uvSize, &meshUVs[0]);

                //Setup IBO
                glGenBuffers(1, &ibo);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize, &meshIndices[0], GL_STATIC_DRAW);

                //Setup vertex attributes
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
                glEnableVertexAttribArray(0);

                glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(vertSize));
                glEnableVertexAttribArray(1);

                glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(vertSize + normalSize));
                glEnableVertexAttribArray(2);

                //All set, lets unbind
                glBindVertexArray(0);
                glBindBuffer(GL_ARRAY_BUFFER, 0);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            }
            void GLMeshRenderer::VRender()
            {
                material->VBind();

                glBindVertexArray(vao);

                size_t vertexCount = mesh->getVertices().size();
                glDrawArrays(GL_TRIANGLES, 0, vertexCount);

                glBindVertexArray(0);

                material->VUnbind();
            }
            void GLMeshRenderer::VFree()
            {
                glDeleteVertexArrays(1, &vao);
                glDeleteBuffers(1, &vbo);
                glDeleteBuffers(1, &ibo);
            }
        }
	}
}
