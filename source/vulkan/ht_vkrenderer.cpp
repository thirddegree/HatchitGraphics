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
			instanceInfo.enabledLayerCount = static_cast<uint32_t>(m_enabledLayerNames.size());
			instanceInfo.ppEnabledLayerNames = &m_enabledLayerNames[0];
			instanceInfo.enabledExtensionCount = static_cast<uint32_t>(m_enabledExtensionNames.size());
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
			if (!enumeratePhysicalDevices())
				return false;

			/*
			* Check layers that we want against the layers available on the device
			*/
			if (!checkDeviceLayers())
				return false;

			/*
			* Check extensions that we want against the extensions supported by the device
			*/
			if (!checkDeviceExtensions())
				return false;

			/*
			* Setup debug callbacks
			*/
#ifdef _DEBUG
			if (!setupDebugCallbacks())
				return false;
#endif

			/*
			* Device should be valid at this point, get device properties
			*/
			vkGetPhysicalDeviceProperties(m_gpu, &m_gpuProps);

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


				bool validated = checkLayers(m_enabledLayerNames, instanceLayers, instanceLayerCount);

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

		bool VKRenderer::enumeratePhysicalDevices()
		{
			VkResult err;
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

		bool VKRenderer::checkDeviceLayers()
		{
			VkResult err;
			uint32_t deviceLayerCount = 0;
			err = vkEnumerateDeviceLayerProperties(m_gpu, &deviceLayerCount, NULL);
			assert(!err);

			if (deviceLayerCount == 0)
			{
#ifdef _DEBUG
				Core::DebugPrintF("VKRenderer::checkValidationLayers(): No layers were found on the device.\n");
#endif
				return false;
			}

			VkLayerProperties* deviceLayers = new VkLayerProperties[deviceLayerCount];
			err = vkEnumerateDeviceLayerProperties(m_gpu, &deviceLayerCount, deviceLayers);
			assert(!err);

			bool validated = checkLayers(m_enabledLayerNames, deviceLayers, deviceLayerCount);
			delete[] deviceLayers;

			if (!validated)
			{
#ifdef _DEBUG
				Core::DebugPrintF("VkRenderer::checkValidationLayers(): Could not validate enabled layers against device layers.\n");
#endif
				return false;
			}

			return true;
		}

		bool VKRenderer::checkLayers(std::vector<const char*> layerNames, VkLayerProperties * layers, uint32_t layerCount)
		{
			bool validated = true;
			for (size_t i = 0; i < layerNames.size(); i++)
			{
				VkBool32 found = 0;
				for (uint32_t j = 0; j < layerCount; j++)
				{
					if (!strcmp(layerNames[i], layers[j].layerName))
					{
						found = true;
						break;
					}
				}
				if (!found)
				{
#ifdef _DEBUG
					Core::DebugPrintF("VKRenderer::checkLayers(), Cannot find layer: %s\n", layerNames[i]);
#endif
					validated = false;
				}

			}

			return validated;
		}

		bool VKRenderer::checkDeviceExtensions()
		{
			VkResult err;
			uint32_t deviceExtensionCount = 0;
			VkBool32 swapchainExtFound = 0;
			//memset(extension_names, 0, sizeof(extension_names));

			//Check how many extensions are on the device
			err = vkEnumerateDeviceExtensionProperties(m_gpu, NULL, &deviceExtensionCount, NULL);
			assert(!err);

			if (deviceExtensionCount == 0)
			{
#ifdef _DEBUG
				Core::DebugPrintF("VKRenderer::checkDeviceExtensions(): Device reported no available extensions\n");
#endif
				return false;
			}

			//Get extension properties
			VkExtensionProperties* deviceExtensions = new VkExtensionProperties[deviceExtensionCount];
			err = vkEnumerateDeviceExtensionProperties(m_gpu, NULL, &deviceExtensionCount, deviceExtensions);
			assert(!err);

			uint32_t extensionCount = 0;
			for (uint32_t i = 0; i < deviceExtensionCount; i++) {
				if (!strcmp(VK_KHR_SWAPCHAIN_EXTENSION_NAME,
					deviceExtensions[i].extensionName)) {
					swapchainExtFound = 1;
					m_enabledExtensionNames[extensionCount++] = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
				}
				assert(extensionCount < 64);
			}

			delete[] deviceExtensions;


			if (!swapchainExtFound)
			{
#ifdef _DEBUG
				Core::DebugPrintF("vkEnumerateDeviceExtensionProperties failed to find "
					"the " VK_KHR_SWAPCHAIN_EXTENSION_NAME
					" extension.\n\nDo you have a compatible "
					"Vulkan installable client driver (ICD) installed?\nPlease "
					"look at the Getting Started guide for additional "
					"information.\n");
#endif
				return false;
			}

			return true;
		}

		bool VKRenderer::setupDebugCallbacks() 
		{
			VkResult err;

			//Get debug callback function pointers
			m_createDebugReportCallback =
				(PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(m_instance, "vkCreateDebugReportCallbackEXT");
			m_destroyDebugReportCallback =
				(PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(m_instance, "vkDestroyDebugReportCallbackEXT");

			if (!m_createDebugReportCallback)
			{
#ifdef _DEBUG
				Core::DebugPrintF("GetProcAddr: Unable to find vkCreateDebugReportCallbackEXT\n");
#endif
				return false;
			}
			if (!m_destroyDebugReportCallback) {
#ifdef _DEBUG
				Core::DebugPrintF("GetProcAddr: Unable to find vkDestroyDebugReportCallbackEXT\n");
#endif
				return false;
			}

			m_debugReportMessage =
				(PFN_vkDebugReportMessageEXT)vkGetInstanceProcAddr(m_instance, "vkDebugReportMessageEXT");
			if (!m_debugReportMessage) {
#ifdef _DEBUG
				Core::DebugPrintF("GetProcAddr: Unable to find vkDebugReportMessageEXT\n");
#endif
				return false;
			}

			PFN_vkDebugReportCallbackEXT callback;
			callback = VKRenderer::debugFunction;

			VkDebugReportCallbackCreateInfoEXT dbgCreateInfo;
			dbgCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
			dbgCreateInfo.pNext = NULL;
			dbgCreateInfo.pfnCallback = callback;
			dbgCreateInfo.pUserData = NULL;
			dbgCreateInfo.flags =
				VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
			err = m_createDebugReportCallback(m_instance, &dbgCreateInfo, NULL,
				&msg_callback);
			switch (err) {
			case VK_SUCCESS:
				break;
			case VK_ERROR_OUT_OF_HOST_MEMORY:
#ifdef _DEBUG
				Core::DebugPrintF("ERROR: Out of host memory!\n");
#endif
				return false;
			default:
#ifdef _DEBUG
				Core::DebugPrintF("ERROR: An unknown error occured!\n");
#endif
				return false;
			}

			return true;
		}

		VKAPI_ATTR VkBool32 VKAPI_CALL VKRenderer::debugFunction(VkFlags msgFlags, VkDebugReportObjectTypeEXT objType,
			uint64_t srcObject, size_t location, int32_t msgCode,
			const char *pLayerPrefix, const char *pMsg, void *pUserData)
		{
			size_t messageSize = strlen(pMsg) + 100;
			char* message = new char[messageSize];

			assert(message);

			if (msgFlags & VK_DEBUG_REPORT_ERROR_BIT_EXT) 
			{
				sprintf(message, "ERROR: [%s] Code %d : %s", pLayerPrefix, msgCode,
					pMsg);
			}
			else if (msgFlags & VK_DEBUG_REPORT_WARNING_BIT_EXT)
			{
				// We know that we're submitting queues without fences, ignore this
				// warning
				if (strstr(pMsg,
					"vkQueueSubmit parameter, VkFence fence, is null pointer")) 
				{
					return false;
				}
				sprintf(message, "WARNING: [%s] Code %d : %s", pLayerPrefix, msgCode, pMsg);
			}
			else {
				return false;
			}

#ifdef _DEBUG
			Core::DebugPrintF("%s\n", message);
#endif
			delete[] message;

			/*
			* false indicates that layer should not bail-out of an
			* API call that had validation failures. This may mean that the
			* app dies inside the driver due to invalid parameter(s).
			* That's what would happen without validation layers, so we'll
			* keep that behavior here.
			*/
			return false;
		}
	}
}