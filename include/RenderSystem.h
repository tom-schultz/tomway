// RenderSystem.hpp : 

#pragma once

#ifdef NDEBUG
	#define VALIDATION_LAYERS
#else
	#define VALIDATION_LAYERS "VK_LAYER_KHRONOS_validation"
#endif

#include "vulkan/vulkan.hpp"
#include "WindowSystem.h"

namespace hagl {
	class RenderSystem {
	public:
		RenderSystem(WindowSystem& windowSystem);
		~RenderSystem();
		void init();
	private:
		struct QueueFamilyIndices {
			uint32_t graphicsFamily;
			bool graphicsAvail;
			uint32_t presentFamily;
			bool presentAvail;
		};

		struct SwapchainSupportDetails {
			vk::SurfaceCapabilitiesKHR capabilities;
			std::vector<vk::SurfaceFormatKHR> formats;
			std::vector<vk::PresentModeKHR> presentModes;
		};

		WindowSystem& _windowSystem;
		vk::SurfaceKHR _surface;
		vk::Instance _vkInstance;
		vk::SampleCountFlagBits _msaaSamples = vk::SampleCountFlagBits::e1;
		QueueFamilyIndices _queueIndices;
		vk::PhysicalDevice _physicalDevice;
		vk::Device _device;
		std::vector<const char*> _requiredExtensions;
		vk::Queue _graphicsQueue;
		vk::Queue _presentQueue;
		std::vector<const char*> _validationLayers = { VALIDATION_LAYERS };

		void createVkInstance();
		void pickPhysicalDevice();
		void createLogicalDevice();
		QueueFamilyIndices findQueueFamilies(const vk::PhysicalDevice& device);
		bool isDeviceSuitable(const vk::PhysicalDevice& device, const QueueFamilyIndices& indices);
		bool isQueueFamilyComplete(QueueFamilyIndices deviceIndices);
		vk::SampleCountFlagBits getMaxUsableSampleCount();
		bool checkDeviceExtensionSupport(const vk::PhysicalDevice& device);
		bool checkValidationLayerSupport();
	};
}