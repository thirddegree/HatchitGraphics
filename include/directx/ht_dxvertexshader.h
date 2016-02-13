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

#include <ht_platform.h>
#include <ht_directx.h>
#include <ht_dxshader.h>

namespace Hatchit {

	namespace Graphics {

		class HT_API DXVertexShader : public DXShader
		{
		public:
			DXVertexShader(ID3D11Device* device, ID3D11DeviceContext* context);
		
			~DXVertexShader();

			ID3D11VertexShader* GetShader();
			ID3D11InputLayout*  GetLayout();

			bool VSetShaderResourceView(std::string name, ID3D11ShaderResourceView* rv) override;
			bool VSetSamplerState(std::string name, ID3D11SamplerState* ss)             override;

		private:
			ID3D11VertexShader* m_shader;
			ID3D11InputLayout*  m_inputLayout;

		protected:
			void VBind()		override;
			void VUnbind()		override;
			bool VInitShader()	override;
		};

	}
}