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
#include <ht_d3d12device.h>
#include <ht_path_singleton.h>
#include <ht_debug.h>
#include <wrl.h>
#include <ht_file.h>
#include <ht_os.h>
#include <ht_math.h>

#include <ht_gpuresourcepool.h>

#include <ht_model.h>
#include <ctime>
#include <ht_math.h>
namespace Hatchit {

    namespace Graphics {

        namespace DX {

            D3D12Renderer::D3D12Renderer()
            {
                
                m_vBuffer = nullptr;
                m_iBuffer = nullptr;
                m_cBuffer = nullptr;
                m_resources = nullptr;
            }

            D3D12Renderer::~D3D12Renderer()
            {
                delete m_resources;
                delete m_cBuffer;

                
                delete m_vBuffer;
                delete m_iBuffer;
            }

            bool D3D12Renderer::VInitialize(const RendererParams& params)
            {
                HRESULT hr = S_OK;

                m_clearColor = params.clearColor;

                /*Create Global D3D12 Hardware Device*/
                if (!_Device)
                {
                    _Device = new D3D12Device;
                    _Type = RendererType::DIRECTX12;
                    if (!_Device->VInitialize())
                    {
                        HT_ERROR_PRINTF("Failed to create D3D12 Device.\n");
                        delete _Device;
                        return false;
                    }
                }

                //m_resources = new D3D12DeviceResources;
                //if (!m_resources->Initialize((HWND)params.window, params.viewportWidth, params.viewportHeight))
                //    return false;

                //auto device = m_resources->GetDevice();
                //auto commandList = m_resources->GetCommandList();
                //commandList->Reset(m_resources->GetCommandAllocator(), nullptr);

                //m_texture = D3D12Texture::GetHandle("raptor.png", "raptor.png", m_resources);
                //m_texture->Upload(m_resources, 1);


                ///*Create Pipeline State*/
                //m_pipeline = D3D12Pipeline::GetHandle("TestPipeline.json",
                //    "TestPipeline.json", m_resources->GetDevice(), m_resources->GetRootLayout()->GetRootSignature());

                //BuildMeshData();

                //GPUResourcePool::Initialize();
                //GPUResourcePool::CreateTexture("raptor.png");
                //GPUResourcePool::CreateTexture("bob.png");
                //GPUResourcePool::CreateTexture("jim.png");
  
                ////Execute command list
                //m_resources->ExecuteCommandList();
                //// Wait for the command list to finish executing; the vertex/index buffers need to be uploaded to the GPU before the upload resources go out of scope.
                //m_resources->WaitForGpu();

                return true;
            }

            void D3D12Renderer::VDeInitialize()
            {
                delete Renderer::_Device;
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
                /*m_resources->Present();

                m_resources->MoveToNextFrame();*/
            }

            void D3D12Renderer::VResizeBuffers(uint32_t width, uint32_t height)
            {
                /*m_resources->Resize(width, height);*/
            }

            void D3D12Renderer::VRender(float dt)
            {
                //m_resources->GetCommandAllocator()->Reset();
                //m_resources->GetCommandList()->Reset(m_resources->GetCommandAllocator(), m_pipeline->GetPipeline());

                //using namespace DirectX;

                //static float angle = 0.0f;
                //angle += dt;
                //if (angle > 360.0f)
                //    angle = 0.0f;
                //m_constantBufferData.proj = Math::MMMatrixPerspProj(3.14f * 0.25f, static_cast<float>(m_width), static_cast<float>(m_height), 0.1f, 1000.0f);
                //m_constantBufferData.view = Math::MMMatrixLookAt(Math::Vector3(0.0f, 0.0f, -5.0f),
                //    Math::Vector3(0.0f, 0.0f, 1.0f),
                //    Math::Vector3(0.0f, 1.0f, 0.0f));

                //Math::Matrix4 scale = Math::MMMatrixScale(Math::Vector3(1.0f, 1.0f, 1.0f));
                //Math::Matrix4 rot = Math::MMMatrixRotationXYZ(Math::Vector3(0, angle, 0));
                //Math::Matrix4 trans = Math::MMMatrixTranslation(Math::Vector3(0, -1, 0));
                //Math::Matrix4 mat = trans * scale * rot; // rot * (scale * trans);
                //m_constantBufferData.world = mat;

                //m_cBuffer->Fill(reinterpret_cast<void**>(&m_constantBufferData), sizeof(m_constantBufferData), sizeof(ConstantBuffer),
                //    m_resources->GetCurrentFrameIndex());

                //m_resources->GetCommandList()->SetGraphicsRootSignature(m_resources->GetRootLayout()->GetRootSignature());
                //
                //ID3D12DescriptorHeap* ppHeaps[] = { m_resources->GetRootLayout()->GetHeap(D3D12RootLayout::HeapType::CBV_SRV_UAV) };
                //m_resources->GetCommandList()->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

                //// Bind the current frame's constant buffer to the pipeline.
                //CD3DX12_GPU_DESCRIPTOR_HANDLE cbHandle(m_resources->GetRootLayout()->GetHeap(D3D12RootLayout::HeapType::CBV_SRV_UAV)->GetGPUDescriptorHandleForHeapStart(), m_resources->GetCurrentFrameIndex(), 0);
                //CD3DX12_GPU_DESCRIPTOR_HANDLE svHandle(m_resources->GetRootLayout()->GetHeap(D3D12RootLayout::HeapType::CBV_SRV_UAV)->GetGPUDescriptorHandleForHeapStart(), m_resources->GetCurrentFrameIndex(), 0);
                //svHandle.Offset(1, m_resources->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
                //
                //m_resources->GetCommandList()->SetGraphicsRootDescriptorTable(0, cbHandle);
                //m_resources->GetCommandList()->SetGraphicsRootDescriptorTable(1, svHandle);
                //
                //D3D12_VIEWPORT viewport = m_resources->GetScreenViewport();
                //D3D12_RECT scissor = { 0, 0, static_cast<LONG>(viewport.Width), static_cast<LONG>(viewport.Height) };
                //m_resources->GetCommandList()->RSSetViewports(1, &viewport);
                //m_resources->GetCommandList()->RSSetScissorRects(1, &scissor);

                //// Indicate this resource will be in use as a render target.
                //CD3DX12_RESOURCE_BARRIER renderTargetResourceBarrier =
                //    CD3DX12_RESOURCE_BARRIER::Transition(m_resources->GetRenderTarget(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
                //m_resources->GetCommandList()->ResourceBarrier(1, &renderTargetResourceBarrier);

                //// Record drawing commands.
                //D3D12_CPU_DESCRIPTOR_HANDLE renderTargetView = m_resources->GetRenderTargetView();
                //D3D12_CPU_DESCRIPTOR_HANDLE depthStencilView = m_resources->GetDepthStencilView();
                //m_resources->GetCommandList()->ClearRenderTargetView(renderTargetView, reinterpret_cast<float*>(&m_clearColor), 0, nullptr);
                //m_resources->GetCommandList()->ClearDepthStencilView(depthStencilView, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

                //m_resources->GetCommandList()->OMSetRenderTargets(1, &renderTargetView, false, &depthStencilView);

                //m_resources->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
                //m_resources->GetCommandList()->IASetVertexBuffers(0, 1, &m_vBuffer->GetView());
                //m_resources->GetCommandList()->IASetIndexBuffer(&m_iBuffer->GetView());
                //m_resources->GetCommandList()->DrawIndexedInstanced(static_cast<uint32_t>(m_numIndices), 1, 0, 0, 0);

                //// Indicate that the render target will now be used to present when the command list is done executing.
                //CD3DX12_RESOURCE_BARRIER presentResourceBarrier =
                //    CD3DX12_RESOURCE_BARRIER::Transition(m_resources->GetRenderTarget(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
                //m_resources->GetCommandList()->ResourceBarrier(1, &presentResourceBarrier);


                //// Execute the command list.
                //m_resources->ExecuteCommandList();

            }

            void D3D12Renderer::BuildMeshData()
            {
                /*Resource::ModelHandle m = Resource::Model::GetHandleFromFileName("raptor.obj");

                auto verts = m->GetMeshes()[0]->getVertices();
                auto normals = m->GetMeshes()[0]->getNormals();
                auto uvs = m->GetMeshes()[0]->getUVs();
                std::vector<Vertex> vertices;
                for (size_t i = 0; i < verts.size(); i++)
                {
                    aiVector3D v = verts[i];
                    aiVector3D n = normals[i];
                    aiVector3D u = uvs[i];

                    Vertex vertex;
                    vertex.position.x = v.x;
                    vertex.position.y = v.y;
                    vertex.position.z = v.z;

                    if (normals.size() > 0)
                        vertex.normal = Math::Float3(n.x, n.y, n.z);
                    if (uvs.size() > 0)
                        vertex.uv = Math::Float2(u.x, u.y);
                    
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
                m_vBuffer->Initialize(m_resources->GetDevice());
                m_vBuffer->UpdateSubData(m_resources->GetCommandList(), 0, static_cast<uint32_t>(vertices.size()), &vertices[0]);
                m_numIndices = indexList.size();
                m_iBuffer = new D3D12IndexBuffer(static_cast<uint32_t>(indexList.size()));
                m_iBuffer->Initialize(m_resources->GetDevice());
                m_iBuffer->UpdateSubData(m_resources->GetCommandList(), 0, static_cast<uint32_t>(m_numIndices), &indexList[0]);
                m_cBuffer = new D3D12ConstantBuffer;
                m_cBuffer->Initialize(m_resources->GetDevice(), m_resources->GetRootLayout()->GetHeap(D3D12RootLayout::HeapType::CBV_SRV_UAV), sizeof(ConstantBuffer));
                m_cBuffer->Map(0, sizeof(ConstantBuffer));
*/

            }
        }

    }

}
