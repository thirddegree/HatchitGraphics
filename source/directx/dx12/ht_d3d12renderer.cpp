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

#include <ht_d3d12renderer.h>
#include <ht_path_singleton.h>
#include <ht_debug.h>
#include <wrl.h>
#include <ht_file.h>
#include <ht_os.h>
#include <ht_math.h>

#include <ht_model.h>
#include <ctime>
#include <ht_math.h>
namespace Hatchit {

    namespace Graphics {

        namespace DX {

            D3D12Renderer::D3D12Renderer()
            {
                m_commandList = nullptr;
                m_vBuffer = nullptr;
                m_iBuffer = nullptr;
            }

            D3D12Renderer::~D3D12Renderer()
            {
                delete m_resources;
                ReleaseCOM(m_commandList);
                ReleaseCOM(m_constantBuffer);
                ReleaseCOM(m_cbDescriptorHeap);
                
                delete m_vBuffer;
                delete m_iBuffer;
            }

            bool D3D12Renderer::VInitialize(const RendererParams& params)
            {
                HRESULT hr = S_OK;

                m_clearColor = params.clearColor;
                m_width = params.viewportWidth;
                m_height = params.viewportHeight;

                m_resources = new D3D12DeviceResources;
                if (!m_resources->Initialize((HWND)params.window, params.viewportWidth, params.viewportHeight))
                    return false;

                auto device = m_resources->GetDevice();

                /*Create Pipeline State*/
             
                m_pipeline = D3D12Pipeline::GetHandle("TestPipeline.json",
                    "TestPipeline.json", m_resources->GetDevice(), m_resources->GetRootSignature());

                /*Create command list*/
                hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_resources->GetCommandAllocator(), m_pipeline->GetPipeline(), IID_PPV_ARGS(&m_commandList));
                if (FAILED(hr))
                    return false;

				Resource::ModelHandle m = Resource::Model::GetHandleFromFileName("raptor.obj");

                auto verts = m->GetMeshes()[0]->getVertices();
                auto normals = m->GetMeshes()[0]->getNormals();
                std::vector<Vertex> vertices;
                for (size_t i = 0; i < verts.size(); i++)
                {
                    aiVector3D v = verts[i];
                    aiVector3D n = normals[i];

                    Vertex vertex;
                    vertex.position.x = v.x;
                    vertex.position.y = v.y;
                    vertex.position.z = v.z;

                    if (normals.size() > 0)
                        vertex.normal = Math::Float3(n.x, n.y, n.z);
                    
                    vertex.color = Math::Float4(1.0f, 0.0f, 0.0f, 1.0f);
                    
                    vertices.push_back(vertex);
                }

                auto indices = m->GetMeshes()[0]->getIndices();
                std::vector<unsigned short> indexList;
                for (auto i : indices)
                {
                    for (uint32_t n = 0; n < i.mNumIndices; n++)
                    {
                        indexList.push_back(i.mIndices[n]);
                    }
                }

                m_vBuffer = new D3D12VertexBuffer(static_cast<uint32_t>(vertices.size()));
                m_vBuffer->Initialize(device);
                m_vBuffer->UpdateSubData(m_commandList, 0, static_cast<uint32_t>(vertices.size()), &vertices[0]);

                m_numIndices = indexList.size();
                m_iBuffer = new D3D12IndexBuffer(static_cast<uint32_t>(indexList.size()));
                m_iBuffer->Initialize(device);
                m_iBuffer->UpdateSubData(m_commandList, 0, static_cast<uint32_t>(m_numIndices), &indexList[0]);

                
                /*Create a descriptor heap for the constant buffers*/
                D3D12_DESCRIPTOR_HEAP_DESC heapDesc;
                heapDesc.NumDescriptors = 1;
                heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
                heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
                heapDesc.NodeMask = 0;
                hr = device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_cbDescriptorHeap));
                if (FAILED(hr))
                {
                    return false;
                }
                CD3DX12_HEAP_PROPERTIES uploadHeapProperties(D3D12_HEAP_TYPE_UPLOAD);
                CD3DX12_RESOURCE_DESC constantBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(2 * c_alignedConstantBufferSize);
                hr = device->CreateCommittedResource(
                    &uploadHeapProperties,
                    D3D12_HEAP_FLAG_NONE,
                    &constantBufferDesc,
                    D3D12_RESOURCE_STATE_GENERIC_READ,
                    nullptr,
                    IID_PPV_ARGS(&m_constantBuffer));
                //Create constant buffer views to access the upload buffer
                D3D12_GPU_VIRTUAL_ADDRESS constantBufferGPUAddress = m_constantBuffer->GetGPUVirtualAddress();
                CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle(m_cbDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
                m_cbDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
                
               
                D3D12_CONSTANT_BUFFER_VIEW_DESC desc;
                desc.BufferLocation = constantBufferGPUAddress;
                desc.SizeInBytes = c_alignedConstantBufferSize;
                device->CreateConstantBufferView(&desc, cpuHandle);

                /*Map the constant buffer*/
                hr = m_constantBuffer->Map(0, nullptr, reinterpret_cast<void**>(&m_mappedConstantBuffer));
                ZeroMemory(m_mappedConstantBuffer, 2 * c_alignedConstantBufferSize);

                hr = m_commandList->Close();
                ID3D12CommandList* ppCommandLists[] = { m_commandList };
                m_resources->GetCommandQueue()->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

                // Wait for the command list to finish executing; the vertex/index buffers need to be uploaded to the GPU before the upload resources go out of scope.
                m_resources->WaitForGPU();

                return true;
            }

            void D3D12Renderer::VDeInitialize()
            {
                m_resources->WaitForGPU();
            }

            void D3D12Renderer::VSetClearColor(const Color& color)
            {
                m_clearColor = color;
            }

            void D3D12Renderer::VClearBuffer(ClearArgs args)
            {
                
            }

            void D3D12Renderer::VPresent()
            {
                m_resources->Present();
            }

            void D3D12Renderer::VResizeBuffers(uint32_t width, uint32_t height)
            {
				m_width = width;
				m_height = height;

				m_resources->Resize(width, height);
            }

            void D3D12Renderer::VRender(float dt)
            {
                using namespace DirectX;

                static float angle = 0.0f;
				angle += dt;
                m_constantBufferData.proj = Math::MMMatrixPerspProj(3.14f * 0.25f, static_cast<float>(m_width), static_cast<float>(m_height), 0.1f, 1000.0f);
                m_constantBufferData.view = Math::MMMatrixLookAt(Math::Vector3(0.0f, 0.0f, -5.0f),
                    Math::Vector3(0.0f, 0.0f, 1.0f),
                    Math::Vector3(0.0f, 1.0f, 0.0f));

				Math::Matrix4 scale = Math::MMMatrixScale(Math::Vector3(1.0f, 1.0f, 1.0f));
				Math::Matrix4 rot = Math::MMMatrixRotationXYZ(Math::Vector3(0, angle, 0));
				Math::Matrix4 trans = Math::MMMatrixTranslation(Math::Vector3(0, -1, 0));
                Math::Matrix4 mat = trans * scale * rot; // rot * (scale * trans);
				m_constantBufferData.world = mat;
               
                // Update the constant buffer resource.
                UINT8* destination = m_mappedConstantBuffer + (m_resources->GetCurrentFrameIndex() * c_alignedConstantBufferSize);
                memcpy(destination, &m_constantBufferData, sizeof(m_constantBufferData));

                m_resources->GetCommandAllocator()->Reset();
                m_commandList->Reset(m_resources->GetCommandAllocator(), m_pipeline->GetPipeline());

                m_commandList->SetGraphicsRootSignature(m_resources->GetRootSignature());
				
                ID3D12DescriptorHeap* ppHeaps[] = { m_cbDescriptorHeap };
                m_commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

                // Bind the current frame's constant buffer to the pipeline.
                CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle(m_cbDescriptorHeap->GetGPUDescriptorHandleForHeapStart(), m_resources->GetCurrentFrameIndex(), 0);
                m_commandList->SetGraphicsRootDescriptorTable(0, gpuHandle);
                
                D3D12_VIEWPORT viewport = m_resources->GetScreenViewport();
                D3D12_RECT scissor = { 0, 0, static_cast<LONG>(viewport.Width), static_cast<LONG>(viewport.Height) };
                m_commandList->RSSetViewports(1, &viewport);
                m_commandList->RSSetScissorRects(1, &scissor);

                // Indicate this resource will be in use as a render target.
                CD3DX12_RESOURCE_BARRIER renderTargetResourceBarrier =
                    CD3DX12_RESOURCE_BARRIER::Transition(m_resources->GetRenderTarget(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
                m_commandList->ResourceBarrier(1, &renderTargetResourceBarrier);

                // Record drawing commands.
                D3D12_CPU_DESCRIPTOR_HANDLE renderTargetView = m_resources->GetRenderTargetView();
                D3D12_CPU_DESCRIPTOR_HANDLE depthStencilView = m_resources->GetDepthStencilView();
                m_commandList->ClearRenderTargetView(renderTargetView, reinterpret_cast<float*>(&m_clearColor), 0, nullptr);
                m_commandList->ClearDepthStencilView(depthStencilView, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

                m_commandList->OMSetRenderTargets(1, &renderTargetView, false, &depthStencilView);

                m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
                m_commandList->IASetVertexBuffers(0, 1, &m_vBuffer->GetView());
                m_commandList->IASetIndexBuffer(&m_iBuffer->GetView());
                m_commandList->DrawIndexedInstanced(static_cast<uint32_t>(m_numIndices), 1, 0, 0, 0);

                // Indicate that the render target will now be used to present when the command list is done executing.
                CD3DX12_RESOURCE_BARRIER presentResourceBarrier =
                    CD3DX12_RESOURCE_BARRIER::Transition(m_resources->GetRenderTarget(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
                m_commandList->ResourceBarrier(1, &presentResourceBarrier);

                m_commandList->Close();
                // Execute the command list.
                ID3D12CommandList* ppCommandLists[] = { m_commandList };
                m_resources->GetCommandQueue()->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

                m_resources->WaitForGPU();
            }

        }

    }

}
