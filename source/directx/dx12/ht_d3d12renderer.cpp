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
                m_pipelineState = nullptr;
                m_rootSignature = nullptr;
                m_vertexShader = nullptr;
                m_pixelShader = nullptr;
                m_commandList = nullptr;
                m_vBuffer = nullptr;
                m_iBuffer = nullptr;
            }

            D3D12Renderer::~D3D12Renderer()
            {
                delete m_resources;
                ReleaseCOM(m_rootSignature);
                ReleaseCOM(m_pipelineState);
                ReleaseCOM(m_vertexShader);
                ReleaseCOM(m_pixelShader);
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

                /*Create Root Signature with one slot for Constant Buffer*/
                CD3DX12_DESCRIPTOR_RANGE range;
                CD3DX12_ROOT_PARAMETER   parameter;

                range.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
                parameter.InitAsDescriptorTable(1, &range, D3D12_SHADER_VISIBILITY_VERTEX);

                D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
                    D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | // Only the input assembler stage needs access to the constant buffer.
                    D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
                    D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
                    D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
                    D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;
                CD3DX12_ROOT_SIGNATURE_DESC descRootSignature;
                descRootSignature.Init(1, &parameter, 0, nullptr, rootSignatureFlags);

                Microsoft::WRL::ComPtr<ID3DBlob> pSignature;
                Microsoft::WRL::ComPtr<ID3DBlob> pError;
                D3D12SerializeRootSignature(&descRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, pSignature.GetAddressOf(), pError.GetAddressOf());
                device->CreateRootSignature(0, pSignature->GetBufferPointer(), pSignature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature));

                /*Load shader files*/
                if (!LoadShaderFiles())
                    return false;
                
                /*Create Pipeline State*/
                // Define the vertex input layout.
                D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
                {
                    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
                    { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA , 0},
                    { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
                };

                D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
                psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
                psoDesc.pRootSignature = m_rootSignature;
                psoDesc.VS = CD3DX12_SHADER_BYTECODE(m_vertexShader);
                psoDesc.PS = CD3DX12_SHADER_BYTECODE(m_pixelShader);
                psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
                psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
                psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
                psoDesc.SampleMask = UINT_MAX;
                psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
                psoDesc.NumRenderTargets = 1;
                psoDesc.RTVFormats[0] = DXGI_FORMAT_B8G8R8A8_UNORM;
                psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
                psoDesc.SampleDesc.Count = 1;
                
                

                hr = device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineState));
                if (FAILED(hr))
                {
                    HT_DEBUG_PRINTF("D3D12Renderer::VInitialize(), Failed to create pipeline state object.\n");
                    return false;
                }

                /*Create command list*/
                hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_resources->GetCommandAllocator(), m_pipelineState, IID_PPV_ARGS(&m_commandList));
                if (FAILED(hr))
                    return false;

				Resource::ModelHandle m = Resource::Model::GetHandle("raptor.obj");

                srand(time(NULL));
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
                    for (int n = 0; n < i.mNumIndices; n++)
                    {
                        indexList.push_back(i.mIndices[n]);
                    }
                }

                m_vBuffer = new D3D12VertexBuffer(vertices.size());
                m_vBuffer->Initialize(device);
                m_vBuffer->UpdateSubData(m_commandList, 0, vertices.size(), &vertices[0]);

                m_numIndices = indexList.size();
                m_iBuffer = new D3D12IndexBuffer(indexList.size());
                m_iBuffer->Initialize(device);
                m_iBuffer->UpdateSubData(m_commandList, 0, m_numIndices, &indexList[0]);

                
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

				Math::Matrix4 scale = Math::MMMatrixScale(Math::Vector3(0.02f, 0.02f, 0.02f));
				Math::Matrix4 rot = Math::MMMatrixRotationXYZ(Math::Vector3(0, angle, 0));
				Math::Matrix4 trans = Math::MMMatrixTranslation(Math::Vector3(0, -30, 0));
				Math::Matrix4 mat = rot * (scale * trans);
				m_constantBufferData.world = mat;
               
                // Update the constant buffer resource.
                UINT8* destination = m_mappedConstantBuffer + (m_resources->GetCurrentFrameIndex() * c_alignedConstantBufferSize);
                memcpy(destination, &m_constantBufferData, sizeof(m_constantBufferData));

                m_resources->GetCommandAllocator()->Reset();
                m_commandList->Reset(m_resources->GetCommandAllocator(), m_pipelineState);

                m_commandList->SetGraphicsRootSignature(m_rootSignature);
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
                m_commandList->DrawIndexedInstanced(m_numIndices, 1, 0, 0, 0);

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

            bool D3D12Renderer::LoadShaderFiles()
            {
                HRESULT hr = S_OK;

                Core::File vShaderFile;
                Core::File pShaderFile;
                try
                {
                    vShaderFile.Open(Core::Path::Value(Core::Path::Directory::Shaders) + "tri_VS.hlsl", Core::FileMode::ReadBinary);
                    pShaderFile.Open(Core::Path::Value(Core::Path::Directory::Shaders) + "tri_PS.hlsl", Core::FileMode::ReadBinary);
                }
                catch (std::exception& e)
                {
                    HT_DEBUG_PRINTF("%s\n", e.what());
                    return false;
                }

                BYTE* vShaderData = new BYTE[vShaderFile.SizeBytes()];
                vShaderFile.Read(vShaderData, vShaderFile.SizeBytes()); //read all of the file into memory

                DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
                // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
                // Setting this flag improves the shader debugging experience, but still allows
                // the shaders to be optimized and to run exactly the way they will run in
                // the release configuration of this program.
                dwShaderFlags |= D3DCOMPILE_DEBUG;

                // Disable optimizations to further improve shader debugging
                dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

                ID3DBlob* errorBlob = nullptr;
                hr = D3DCompile2(vShaderData, vShaderFile.SizeBytes(), nullptr, nullptr,
                    nullptr, "main", "vs_5_0", dwShaderFlags, NULL, NULL, nullptr, NULL,
                    &m_vertexShader, &errorBlob);
                if (FAILED(hr))
                {
                    if (errorBlob)
                    {
                        OutputDebugStringA(reinterpret_cast<const char*>(errorBlob->GetBufferPointer()));
                        ReleaseCOM(errorBlob);
                    }
                    return false;
                }
                ReleaseCOM(errorBlob);
                delete[] vShaderData;


                BYTE* pShaderData = new BYTE[pShaderFile.SizeBytes()];
                pShaderFile.Read(pShaderData, pShaderFile.SizeBytes());
                hr = D3DCompile2(pShaderData, pShaderFile.SizeBytes(), nullptr, nullptr,
                    nullptr, "main", "ps_5_0", dwShaderFlags, NULL, NULL, nullptr, NULL,
                    &m_pixelShader, &errorBlob);
                if (FAILED(hr))
                {
                    if (errorBlob)
                    {
                        OutputDebugStringA(reinterpret_cast<const char*>(errorBlob->GetBufferPointer()));
                        ReleaseCOM(errorBlob);
                    }
                    return false;
                }
                ReleaseCOM(errorBlob);
                delete[] pShaderData;

                return true;
            }

        }

    }

}
