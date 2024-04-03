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

		/*
		########  WARNING WARNING WARNING WARNING
		########  DO NOT REORDER THIS SECTION
		########  STRICT ORDERING SECTION BEGIN
		*/
		
		vk::UniqueInstance _vkInstance;
		vk::UniqueSurfaceKHR _surface;
		vk::UniqueDevice _device;
		vk::UniqueSwapchainKHR _swapchain;

		/*
		########  STRICT ORDERING SECTION END
		*/

		WindowSystem& _windowSystem;
		vk::PhysicalDevice _physicalDevice;
		std::vector<const char*> _requiredDeviceExtensions = { vk::KHRSwapchainExtensionName };
		vk::Queue _graphicsQueue;
		vk::Queue _presentQueue;
		std::vector<const char*> _validationLayers = { VALIDATION_LAYERS };
		std::vector<vk::Image> _images;
		std::vector<vk::UniqueImageView> _imageViews;
		vk::Extent2D _swapchainExtent;
		vk::Format _swapchainFormat;

		vk::SampleCountFlagBits _msaaSamples = vk::SampleCountFlagBits::e1;
		QueueFamilyIndices _queueIndices;

		void createVkInstance();
		void pickPhysicalDevice();
		void createLogicalDevice();
		void createSwapchain();
		void createImageViews();
		QueueFamilyIndices findQueueFamilies(const vk::PhysicalDevice& device);
		bool isDeviceSuitable(const vk::PhysicalDevice& device, const QueueFamilyIndices& indices);
		bool isQueueFamilyComplete(QueueFamilyIndices deviceIndices);
		vk::SampleCountFlagBits getMaxUsableSampleCount();
		bool checkDeviceExtensionSupport(const vk::PhysicalDevice& device);
		bool checkValidationLayerSupport();
	};

	static vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> formats);
	static vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR> modes);
	static vk::Extent2D chooseSwapExtent(const WindowSystem& windowSystem, const vk::SurfaceCapabilitiesKHR& capabilities);
	static vk::UniqueImageView createImageView(const vk::Device& device, const vk::Image& image, vk::Format format, vk::ImageAspectFlags aspectMask, uint32_t mipLevels);
}