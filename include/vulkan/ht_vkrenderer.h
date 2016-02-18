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
#include <ht_vulkan.h>
#include <ht_renderer.h>
#include <ht_string.h>
#include <vector>

namespace Hatchit {

	namespace Graphics {

		class HT_API VKRenderer : public IRenderer
		{
		public:
			VKRenderer();

			~VKRenderer();

			/** Initialize the renderer
			* \param params The paramaters to intialize this renderer with
			*/
			bool VInitialize(const RendererParams& params) ;

			///Shutdown the renderer
			void VDeInitialize();

			/** Resizes the the screen
			* \param width The new width of the screen
			* \param height The new height of the screen
			*/
			void VResizeBuffers(uint32_t width, uint32_t height);

			/** Sets the color that the screen will clear with
			* \param color The Color to clear the screen with
			*/
			void VSetClearColor(const Color& color);
			/** Clears the screen with the given clear color
			* \param args Arguments to describe which buffer you want to clear
			*/
			void VClearBuffer(ClearArgs args);

			///Present a frame to the screen via a backbuffer
			void VPresent();

		private:
			std::vector<const char*>	m_enabledLayerNames;
			std::vector<const char*>    m_enabledExtensionNames;


			VkApplicationInfo m_appInfo;
			VkInstance		  m_instance;
			VkPhysicalDevice  m_gpu;

			bool checkInstanceLayers();
			bool checkInstanceExtensions();
		};
	}
}