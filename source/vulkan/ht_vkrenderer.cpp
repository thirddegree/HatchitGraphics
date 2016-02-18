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

#include <ht_vkrenderer.h>
#include <ht_debug.h>

#include <cassert>

namespace Hatchit {
	
	namespace Graphics {

		VKRenderer::VKRenderer()
		{

		}

		VKRenderer::~VKRenderer()
		{
			vkDestroyInstance(m_instance, nullptr);
		}

		bool VKRenderer::VInitialize(const RendererParams & params)
		{
			bool success = true;

			VkResult err;

			/*
			* Check Vulkan instance layers
			*/
			if (!checkInstanceLayers())
				return false;

			/*
			* Check Vulkan instance extensions
			*/
			if (!checkInstanceExtensions())
				return false;

			/*
			* Setup Vulkan application info structure
			*/

			m_appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
			m_appInfo.pNext = nullptr;
			m_appInfo.pApplicationName = params.applicationName.c_str();
			m_appInfo.applicationVersion = 0;
			m_appInfo.pEngineName = "Hatchit";
			m_appInfo.engineVersion = 0;
			m_appInfo.apiVersion = VK_API_VERSION;

			/*
			* Setup Vulkan instance create info
			*/
			VkInstanceCreateInfo instanceInfo;
			instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
			instanceInfo.pNext = nullptr;
			instanceInfo.pApplicationInfo = &m_appInfo;
			instanceInfo.enabledLayerCount = m_enabledLayerNames.size();
			instanceInfo.ppEnabledLayerNames = &m_enabledLayerNames[0];
			instanceInfo.enabledExtensionCount = m_enabledExtensionNames.size();
			instanceInfo.ppEnabledExtensionNames = &m_enabledExtensionNames[0];

			/**
			* Create Vulkan instance handle
			*/
			err = vkCreateInstance(&instanceInfo, nullptr, &m_instance);
			switch (err)
			{
				case VK_SUCCESS:
					break;

				case VK_ERROR_INCOMPATIBLE_DRIVER:
				{
#ifdef _DEBUG
					Core::DebugPrintF("Cannot find a compatible Vulkan installable client driver"
						"(ICD).\n\nPlease look at the Getting Started guide for "
						"additional information.\n" 
						"vkCreateInstance Failure\n");
#endif
				} return false;

				case VK_ERROR_EXTENSION_NOT_PRESENT:
				{
					//TODO: print something
				} return false;

				default:
					return false;
			}

			/**
			*
			* Enumerate available GPU devices for use with Vulkan 
			*
			*/

			uint32_t gpuCount = 0;

			err = vkEnumeratePhysicalDevices(m_instance, &gpuCount, nullptr);
			if (gpuCount <= 0 || err != VK_SUCCESS)
			{
#ifdef _DEBUG
				Core::DebugPrintF("No compatible devices were found.\n");
#endif
				return false;
			}

			VkPhysicalDevice* physicalDevices = new VkPhysicalDevice[gpuCount];
			err = vkEnumeratePhysicalDevices(m_instance, &gpuCount, physicalDevices);
			if (err)
			{
#ifdef _DEBUG
				Core::DebugPrintF("Vulkan encountered error enumerating physical devices.\n");
#endif
				delete[] physicalDevices;
				return false;
			}

			/*For now, we store the first device Vulkan finds*/
			m_gpu = physicalDevices[0];
			delete[] physicalDevices;

			

			return true;
		}

		void VKRenderer::VDeInitialize()
		{
		}

		void VKRenderer::VResizeBuffers(uint32_t width, uint32_t height)
		{
		}

		void VKRenderer::VSetClearColor(const Color & color)
		{
		}

		void VKRenderer::VClearBuffer(ClearArgs args)
		{
		}

		void VKRenderer::VPresent()
		{
		}



		bool VKRenderer::checkInstanceLayers()
		{
			VkResult err;

			/**
			* Vulkan:
			*
			* Check the following requested Vulkan layers against available layers
			*
			*/
			VkBool32 validationFound = 0;
			uint32_t instanceLayerCount = 0;
			err = vkEnumerateInstanceLayerProperties(&instanceLayerCount, NULL);
			assert(!err);

			m_enabledLayerNames = {
				"VK_LAYER_LUNARG_threading",      "VK_LAYER_LUNARG_mem_tracker",
				"VK_LAYER_LUNARG_object_tracker", "VK_LAYER_LUNARG_draw_state",
				"VK_LAYER_LUNARG_param_checker",  "VK_LAYER_LUNARG_swapchain",
				"VK_LAYER_LUNARG_device_limits",  "VK_LAYER_LUNARG_image",
				"VK_LAYER_GOOGLE_unique_objects",
			};

			if (instanceLayerCount > 0)
			{
				VkLayerProperties* instanceLayers = new VkLayerProperties[instanceLayerCount];
				err = vkEnumerateInstanceLayerProperties(&instanceLayerCount, instanceLayers);
				assert(!err);


				bool validated = true;
				for (size_t i = 0; i < m_enabledLayerNames.size(); i++)
				{
					VkBool32 found = 0;
					for (uint32_t j = 0; j < instanceLayerCount; j++)
					{
						if (!strcmp(m_enabledLayerNames[i], instanceLayers[j].layerName))
						{
							found = true;
							break;
						}
					}
					if (!found)
					{
#ifdef _DEBUG
						Core::DebugPrintF("VKRenderer::checkInstanceLayers(), Cannot find layer: %s\n", m_enabledLayerNames[i]);
#endif
						validated = false;
					}

				}

				delete[] instanceLayers;
				if (!validated)
					return false;

				return true;
			}

#ifdef _DEBUG
			Core::DebugPrintF("VKRenderer::checkInstanceLayers(), instanceLayerCount is zero. \n");
#endif
			return false;
		}

		bool VKRenderer::checkInstanceExtensions()
		{
			VkResult err;

			/**
			* Vulkan:
			*
			* Check the for correct Vulkan instance extensions
			*
			*/
			VkBool32 surfaceExtFound = 0;
			VkBool32 platformSurfaceExtFound = 0;
			uint32_t instanceExtensionCount = 0;

			err = vkEnumerateInstanceExtensionProperties(NULL, &instanceExtensionCount, NULL);
			assert(!err);

			if (instanceExtensionCount > 0) 
			{
				VkExtensionProperties* instanceExtensions = new VkExtensionProperties[instanceExtensionCount];
				err = vkEnumerateInstanceExtensionProperties(NULL, &instanceExtensionCount, instanceExtensions);
				assert(!err);
				for (uint32_t i = 0; i < instanceExtensionCount; i++) 
				{
					if (!strcmp(VK_KHR_SURFACE_EXTENSION_NAME, instanceExtensions[i].extensionName))
					{
						surfaceExtFound = 1;
						m_enabledExtensionNames.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
					}
#ifdef _WIN32
					if (!strcmp(VK_KHR_WIN32_SURFACE_EXTENSION_NAME, instanceExtensions[i].extensionName)) {
						platformSurfaceExtFound = 1;
						m_enabledExtensionNames.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
					}
#endif

					if (!strcmp(VK_EXT_DEBUG_REPORT_EXTENSION_NAME, instanceExtensions[i].extensionName)) {
						m_enabledExtensionNames.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
					}

					assert(m_enabledExtensionNames.size() < 64);
				}

				delete[] instanceExtensions;

				return true;
			}

#ifdef _DEBUG
			Core::DebugPrintF("VKRenderer::checkInstanceExtensions(), instanceExtensionCount is zero. \n");
#endif
			return false;
		}

	}
}