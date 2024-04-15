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

	class RenderSystem {
	public:
		RenderSystem(WindowSystem& windowSystem, unsigned maxFramesInFlight = 2);
		~RenderSystem();
		void drawFrame();
		void minimized();
		void resizeFramebuffer();
	private:
		/*
		########  WARNING WARNING WARNING WARNING
		########  DO NOT REORDER THIS SECTION
		########  STRICT ORDERING SECTION BEGIN
		*/
		
		vk::UniqueInstance _uInstance;
		vk::UniqueDevice _uDevice;
		vk::UniqueSurfaceKHR _uSurface;
		vk::UniqueSwapchainKHR _uSwapchain;
		std::vector<vk::UniqueImageView> _uImageViews;
		vk::UniqueRenderPass _uRenderPass;
		vk::UniquePipelineLayout _uPipelineLayout;
		vk::UniquePipeline _uGraphicsPipeline;
		std::vector<vk::UniqueFramebuffer> _uFramebuffers;
		vk::UniqueCommandPool _uCommandPool;
		std::vector<vk::UniqueCommandBuffer> _uCommandBuffers;
		std::vector<vk::UniqueSemaphore> _uImageAvailableSems;
		std::vector<vk::UniqueSemaphore> _uRenderFinishedSems;
		std::vector<vk::UniqueFence> _uInFlightFences;

		/*
		########  STRICT ORDERING SECTION END
		*/

		bool _framebufferResized;
		unsigned _maxFramesInFlight;
		bool _windowMinimized;
		unsigned _currFrame;
		WindowSystem& _windowSystem;
		vk::PhysicalDevice _physicalDevice;
		std::vector<const char*> _requiredDeviceExtensions = { vk::KHRSwapchainExtensionName };
		vk::Queue _graphicsQueue;
		vk::Queue _presentQueue;
		std::vector<const char*> _validationLayers = { VALIDATION_LAYERS };
		std::vector<vk::Image> _images;
		vk::Extent2D _swapchainExtent;
		vk::Format _swapchainFormat;

		vk::SampleCountFlagBits _msaaSamples = vk::SampleCountFlagBits::e1;
		QueueFamilyIndices _queueIndices;

		void createCommandBuffer();
		void createCommandPool();
		void createFramebuffers();
		void createGraphicsPipeline();
		void createImageViews();
		void createLogicalDevice();
		void createSwapchain();
		void createSyncObjects();
		void createVkInstance();
		void pickPhysicalDevice();
		void recordCommandBuffer(vk::CommandBuffer& commandBuffer, uint32_t imageIndex);
		void recreateSwapchain();
	};

	static bool checkDeviceExtensionSupport(const vk::PhysicalDevice& device, std::vector<const char*> requiredDeviceExtensions);
	static bool checkValidationLayerSupport(const std::vector<const char*>& validationLayers);
	static vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> formats);
	static vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR> modes);
	static vk::Extent2D chooseSwapExtent(const WindowSystem& windowSystem, const vk::SurfaceCapabilitiesKHR& capabilities);
	static vk::UniqueImageView createImageView(const vk::Device& device, const vk::Image& image, vk::Format format, vk::ImageAspectFlags aspectMask, uint32_t mipLevels);
	static vk::UniqueRenderPass createRenderPass(const vk::PhysicalDevice& physicalDevice, const vk::Device& device, const vk::Format& format, vk::SampleCountFlagBits samples);
	static vk::UniqueShaderModule createShaderModule(const vk::Device& device, const std::vector<char>& bytes);
	static vk::Format findDepthFormat(const vk::PhysicalDevice& physicalDevice);
	static QueueFamilyIndices findQueueFamilies(const vk::PhysicalDevice& device, const vk::SurfaceKHR& surface);
	static vk::SampleCountFlagBits getMaxUsableSampleCount(const vk::PhysicalDevice& physicalDevice);
	static bool isDeviceSuitable(const vk::PhysicalDevice& device, const vk::SurfaceKHR& surface, const std::vector<const char*>& requiredDeviceExtensions);
	static bool isQueueFamilyComplete(const QueueFamilyIndices& deviceIndices);
	static std::vector<char> readShaderBytes(const std::string& filePath);
}