// RenderSystem.hpp : 

#pragma once

#ifdef NDEBUG
	#define VALIDATION_LAYERS
#else
	#define VALIDATION_LAYERS "VK_LAYER_KHRONOS_validation"
#endif

#include <vulkan/vulkan.hpp>
#include "Transform.h"
#include "Vertex.h"
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
		RenderSystem(WindowSystem& windowSystem, uint32_t vertexCount, unsigned maxFramesInFlight = 2);
		~RenderSystem();

		void drawFrame(
			const Transform& transform,
			const std::vector<Vertex>& vertices,
			const std::vector<uint32_t> indices);

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
		vk::UniqueBuffer _uVertexBuffer;
		vk::UniqueDeviceMemory _uVertexBufferMemory;

		/*
		########  STRICT ORDERING SECTION END
		*/

		unsigned _currFrame;
		bool _framebufferResized;
		vk::Queue _graphicsQueue;
		std::vector<vk::Image> _images;
		unsigned _maxFramesInFlight;
		vk::SampleCountFlagBits _msaaSamples = vk::SampleCountFlagBits::e1;
		vk::PhysicalDevice _physicalDevice;
		vk::Queue _presentQueue;
		QueueFamilyIndices _queueIndices;
		std::vector<const char*> _requiredDeviceExtensions = { vk::KHRSwapchainExtensionName };
		vk::Extent2D _swapchainExtent;
		vk::Format _swapchainFormat;
		std::vector<const char*> _validationLayers = { VALIDATION_LAYERS };
		uint32_t _vertexCount;
		size_t _vertexBufferSize;
		bool _windowMinimized;
		WindowSystem& _windowSystem;

		void createBuffer(
			size_t size,
			vk::BufferUsageFlagBits usageFlags,
			vk::SharingMode sharingMode,
			vk::MemoryPropertyFlags memoryPropertyFlags,
			vk::UniqueBuffer& uBuffer,
			vk::UniqueDeviceMemory& uBufferMemory);

		void createCommandBuffer();
		void createCommandPool();
		void createFramebuffers();
		void createGraphicsPipeline();
		void createImageViews();
		void createLogicalDevice();
		void createSwapchain();
		void createSyncObjects();
		void createVkInstance();
		uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties);
		void pickPhysicalDevice();
		void recordCommandBuffer(vk::CommandBuffer& commandBuffer, uint32_t imageIndex);
		void recreateSwapchain();
		void transferVertices(const std::vector<Vertex>& vertices);
	};

	static bool checkDeviceExtensionSupport(const vk::PhysicalDevice& device, std::vector<const char*> requiredDeviceExtensions);
	static bool checkValidationLayerSupport(const std::vector<const char*>& validationLayers);
	static vk::Extent2D chooseSwapExtent(const WindowSystem& windowSystem, const vk::SurfaceCapabilitiesKHR& capabilities);
	static vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR> modes);
	static vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> formats);
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