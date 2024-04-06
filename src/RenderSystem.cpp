#include "RenderSystem.h"
#include "HaglConstants.h"
#include <iostream>
#include <algorithm>
#include "HaglUtility.h"
#include <vector>
#include <fstream>

hagl::RenderSystem::RenderSystem(WindowSystem& windowSystem)
	: _windowSystem(windowSystem),
	_queueIndices(),
	_swapchainFormat()
{
	try {
		createVkInstance();

		_uSurface = _windowSystem.createVulkanSurface(*_uInstance);
		pickPhysicalDevice();
		createLogicalDevice();
		createSwapchain();
		createImageViews();
		createRenderPass(_physicalDevice, *_uDevice, _swapchainFormat, _msaaSamples);
		createGraphicsPipeline();
	}
	catch (std::exception e) {
		LOG_ERROR(0, "Failed to initialize render system with error: %s", e.what());
		exit(1);
	}

	LOG_INFO("Render system initialized.");
}

hagl::RenderSystem::~RenderSystem() {
}

void hagl::RenderSystem::pickPhysicalDevice() {
	uint32_t deviceCount = 0;
	auto devices = _uInstance->enumeratePhysicalDevices();

	if (devices.size() == 0) {
		throw std::runtime_error("Failed to find a GPU with Vulkan support!");
	}
	
	for (auto device : devices) {

		if (isDeviceSuitable(device, *_uSurface, _requiredDeviceExtensions)) {
			_physicalDevice = device;
			_msaaSamples = getMaxUsableSampleCount(device);
			_queueIndices = findQueueFamilies(device, *_uSurface);
			return;
		}
	}

	throw new std::runtime_error("Could not find a suitable GPU!");
}

vk::UniqueRenderPass hagl::createRenderPass(const vk::PhysicalDevice& physicalDevice, const vk::Device& device, const vk::Format& format, vk::SampleCountFlagBits samples) {
	vk::AttachmentDescription colorAttachment(
		{}, // Flags
		format, // Format
		samples, // Sample count
		vk::AttachmentLoadOp::eClear, // Load op
		vk::AttachmentStoreOp::eStore, // Store op
		vk::AttachmentLoadOp::eDontCare, // Stencil load op
		vk::AttachmentStoreOp::eDontCare, // Stencil store op
		vk::ImageLayout::eUndefined, // Initial layout
		vk::ImageLayout::eColorAttachmentOptimal // Final layout
	);

	vk::AttachmentReference colorAttachmentRef(
		0, // Attachment
		vk::ImageLayout::eColorAttachmentOptimal
	);

	vk::AttachmentDescription resolveAttachment(
		{}, // Flags
		format, // Format
		vk::SampleCountFlagBits::e1, // Sample count
		vk::AttachmentLoadOp::eDontCare, // Load op
		vk::AttachmentStoreOp::eStore, // Store op
		vk::AttachmentLoadOp::eDontCare, // Stencil load op
		vk::AttachmentStoreOp::eDontCare, // Stencil store op
		vk::ImageLayout::eUndefined, // Initial layout
		vk::ImageLayout::ePresentSrcKHR // Final layout
	);

	vk::AttachmentReference resolveAttachmentRef(
		1, // Attachment
		vk::ImageLayout::eColorAttachmentOptimal
	);

	vk::AttachmentDescription depthAttachment(
		{}, // Flags
		hagl::findDepthFormat(physicalDevice), // Format
		samples, // Sample count
		vk::AttachmentLoadOp::eClear, // Load op
		vk::AttachmentStoreOp::eStore, // Store op
		vk::AttachmentLoadOp::eDontCare, // Stencil load op
		vk::AttachmentStoreOp::eDontCare, // Stencil store op
		vk::ImageLayout::eUndefined, // Initial layout
		vk::ImageLayout::eDepthStencilAttachmentOptimal // Final layout
	);

	vk::AttachmentReference depthAttachmentRef(
		2, // Attachment
		vk::ImageLayout::eDepthStencilAttachmentOptimal
	);

	vk::SubpassDescription subpass(
		{}, // Flags
		vk::PipelineBindPoint::eGraphics,
		nullptr, // Input attachments
		colorAttachmentRef, // Color attachment refs
		resolveAttachmentRef, // Resolve attachment refs
		&depthAttachmentRef, // Depth attachment ref, ptr for some reason...
		nullptr // Preserve attachments
	);

	vk::SubpassDependency dependency(
		VK_SUBPASS_EXTERNAL, // Source subpass
		0, // Dest subpass
		vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eLateFragmentTests, // Source stage mask
		vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests, // Dest stage mask
		vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite, // Source access mask
		vk::AccessFlagBits::eColorAttachmentWrite | vk::AccessFlagBits::eDepthStencilAttachmentWrite, // Dest access mask
		{} // Dependency flags
	);

	vk::AttachmentDescription attachments[]{
		colorAttachment,
		resolveAttachment,
		depthAttachment,
	};

	vk::RenderPassCreateInfo createInfo(
		{}, // Flags
		attachments,
		subpass,
		dependency
	);

	return device.createRenderPassUnique(createInfo);
}

void hagl::RenderSystem::createGraphicsPipeline() {
	// TODO - make this configurable, probably a list of shaders to create or something
	std::vector<char> _fragShaderBytes = readShaderBytes("shaders/frag.spv");
	std::vector<char> _vertShaderBytes = readShaderBytes("shaders/vert.spv");

	_uFragShaderModule = createShaderModule(*_uDevice, _fragShaderBytes);
	_uVertShaderModule = createShaderModule(*_uDevice, _vertShaderBytes);

	vk::PipelineShaderStageCreateInfo fragStageCreateInfo({}, vk::ShaderStageFlagBits::eFragment, *_uFragShaderModule, "main");
	vk::PipelineShaderStageCreateInfo vertStageCreateInfo({}, vk::ShaderStageFlagBits::eVertex, *_uVertShaderModule, "main");

	std::vector<vk::PipelineShaderStageCreateInfo> shaderStages{ fragStageCreateInfo, vertStageCreateInfo };
}

void hagl::RenderSystem::createVkInstance() {
	vk::ApplicationInfo info(
				APP_NAME, // Application Name
				1, // Application version
				NULL, // Engine Name
				1, // Engine Version
				VK_API_VERSION_1_3 // API Version
			);

	auto requiredInstanceExtensions = _windowSystem.getExtensions();

	vk::InstanceCreateInfo appCreateInfo(
		{}, // flags
		&info, // Application Info
		(uint32_t) _validationLayers.size(), // Enable layer count
		_validationLayers.data(), // Enabled layer names
		(uint32_t)requiredInstanceExtensions.size(), // Extension count
		requiredInstanceExtensions.data() // Extension names
	);

	_uInstance = vk::createInstanceUnique(appCreateInfo);
}

void hagl::RenderSystem::createSwapchain() {
	SwapchainSupportDetails details = {
			_physicalDevice.getSurfaceCapabilitiesKHR(*_uSurface),
			_physicalDevice.getSurfaceFormatsKHR(*_uSurface),
			_physicalDevice.getSurfacePresentModesKHR(*_uSurface)
	};

	vk::SurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(details.formats);
	vk::PresentModeKHR presentMode = chooseSwapPresentMode(details.presentModes);
	vk::Extent2D extent = chooseSwapExtent(_windowSystem, details.capabilities);
	uint32_t imageCount = details.capabilities.minImageCount + 1;

	imageCount = imageCount < details.capabilities.maxImageCount
		? imageCount
		: details.capabilities.maxImageCount;

	vk::SharingMode sharingMode = _queueIndices.graphicsFamily != _queueIndices.presentFamily
		? vk::SharingMode::eConcurrent
		: vk::SharingMode::eExclusive;

	uint32_t queueFamilyIndices[] = {
		_queueIndices.graphicsFamily,
		_queueIndices.presentFamily
	};

	vk::SwapchainCreateInfoKHR createInfo(
		{} // flags
		, * _uSurface
		, imageCount // minImageCount
		, surfaceFormat.format
		, surfaceFormat.colorSpace
		, extent
		, 1 // imageArrayLayers
		, vk::ImageUsageFlagBits::eColorAttachment
		, sharingMode
		, queueFamilyIndices
		, details.capabilities.currentTransform // preTransform
		, vk::CompositeAlphaFlagBitsKHR::eOpaque
		, presentMode
		, true // clipped
		, VK_NULL_HANDLE); // Old swapchain

	_uSwapchain = _uDevice->createSwapchainKHRUnique(createInfo);
	_images = _uDevice->getSwapchainImagesKHR(*_uSwapchain);
	_swapchainExtent = extent;
	_swapchainFormat = surfaceFormat.format;
}

void hagl::RenderSystem::createImageViews() {
	for (auto image : _images) {
		_imageViews.push_back(createImageView(_uDevice.get(), image, _swapchainFormat, vk::ImageAspectFlagBits::eColor, 1));
	}
}

static vk::UniqueImageView hagl::createImageView(const vk::Device& device, const vk::Image& image, vk::Format format, vk::ImageAspectFlags aspectMask, uint32_t mipLevels) {
	vk::ComponentMapping components(
		vk::ComponentSwizzle::eIdentity,
		vk::ComponentSwizzle::eIdentity,
		vk::ComponentSwizzle::eIdentity,
		vk::ComponentSwizzle::eIdentity
	);

	vk::ImageSubresourceRange subresource(
		aspectMask,
		0, // Base mip level
		mipLevels,
		0, // Base array layer
		1 // Layer count
	);

	vk::ImageViewCreateInfo createInfo(
		{}, // Flags
		image,
		vk::ImageViewType::e2D, // Image view type
		format,
		components,
		subresource
	);

	return device.createImageViewUnique(createInfo);
}

static hagl::QueueFamilyIndices hagl::findQueueFamilies(const vk::PhysicalDevice& device, const vk::SurfaceKHR& surface)  {
	QueueFamilyIndices deviceIndices{};
	auto queueFamilyProperties = device.getQueueFamilyProperties();

	for (int i = 0; i < queueFamilyProperties.size() && !isQueueFamilyComplete(deviceIndices); i++) {
		if (queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eGraphics) {
			deviceIndices.graphicsFamily = i;
			deviceIndices.graphicsAvail = true;
		}

		if (device.getSurfaceSupportKHR(i, surface)) {
			deviceIndices.presentFamily = i;
			deviceIndices.presentAvail = true;
		}
	}

	return deviceIndices;
}

static bool hagl::checkDeviceExtensionSupport(const vk::PhysicalDevice& device, std::vector<const char*> requiredDeviceExtensions) {
	auto deviceExtensions = device.enumerateDeviceExtensionProperties();
	std::vector<std::string> supported;

	for (auto ext : deviceExtensions) {
		supported.push_back(std::string((char*)ext.extensionName));
	}
	
	for (auto reqExtension : requiredDeviceExtensions) {
		bool found = false;

		for (auto devExtension : deviceExtensions) {
			found = found || strcmp(devExtension.extensionName, reqExtension) == 0;
		}

		if (!found) {
			return false;
		}
	}

	return true;
}

static bool hagl::checkValidationLayerSupport(const std::vector<const char*>& validationLayers) {
	auto supportedLayers = vk::enumerateInstanceLayerProperties();

	for (auto reqLayer : validationLayers) {
		bool found = false;

		for (auto supportedLayer : supportedLayers) {
			found = found || strcmp(supportedLayer.layerName, reqLayer);
		}

		if (!found) {
			return false;
		}
	}

	return true;
}

vk::Format hagl::findDepthFormat(const vk::PhysicalDevice& physicalDevice) {
	std::vector<vk::Format> formats = {
		vk::Format::eD32Sfloat,
		vk::Format::eD32SfloatS8Uint,
		vk::Format::eD24UnormS8Uint
	};

	vk::ImageTiling tiling = vk::ImageTiling::eOptimal;
	vk::FormatFeatureFlags features = vk::FormatFeatureFlagBits::eDepthStencilAttachment;
	vk::FormatProperties props;

	for (auto format : formats) {
		props = physicalDevice.getFormatProperties(format);

		bool hasLinear = (tiling == vk::ImageTiling::eLinear
			&& (props.linearTilingFeatures & features) == features);

		bool hasOptimal = (tiling == vk::ImageTiling::eOptimal
			&& (props.optimalTilingFeatures & features) == features);

		if (hasLinear || hasOptimal) {
			return format;
		}
	}

	throw new std::runtime_error("Failed to find supported device format.\n");
}

static bool hagl::isDeviceSuitable(const vk::PhysicalDevice& device, const vk::SurfaceKHR& surface, const std::vector<const char*>& requiredDeviceExtensions) {
	QueueFamilyIndices deviceIndices = hagl::findQueueFamilies(device, surface);
	auto deviceProperties = device.getProperties();
	auto deviceFeatures = device.getFeatures();

	bool extensionsSupported = hagl::checkDeviceExtensionSupport(device, requiredDeviceExtensions);
	bool swapchainAdequate = false;

	if (extensionsSupported) {
		hagl::SwapchainSupportDetails details = {
			device.getSurfaceCapabilitiesKHR(surface),
			device.getSurfaceFormatsKHR(surface),
			device.getSurfacePresentModesKHR(surface)
		};

		swapchainAdequate = details.formats.size() > 0 && details.presentModes.size() > 0;
	}

	return (
		(deviceProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu
			|| deviceProperties.deviceType == vk::PhysicalDeviceType::eIntegratedGpu)
		&& deviceFeatures.geometryShader
		&& deviceFeatures.samplerAnisotropy
		&& hagl::isQueueFamilyComplete(deviceIndices)
		&& extensionsSupported
		&& swapchainAdequate
	);
}

static vk::SampleCountFlagBits hagl::getMaxUsableSampleCount(const vk::PhysicalDevice& physicalDevice) {
	auto physicalDeviceProperties = physicalDevice.getProperties();

	vk::SampleCountFlags counts = (
		physicalDeviceProperties.limits.framebufferColorSampleCounts
		& physicalDeviceProperties.limits.framebufferDepthSampleCounts
	);

	if (counts & vk::SampleCountFlagBits::e64) { return vk::SampleCountFlagBits::e64; }
	if (counts & vk::SampleCountFlagBits::e32) { return vk::SampleCountFlagBits::e32; }
	if (counts & vk::SampleCountFlagBits::e16) { return vk::SampleCountFlagBits::e16; }
	if (counts & vk::SampleCountFlagBits::e8) { return vk::SampleCountFlagBits::e8; }
	if (counts & vk::SampleCountFlagBits::e4) { return vk::SampleCountFlagBits::e4; }
	if (counts & vk::SampleCountFlagBits::e2) { return vk::SampleCountFlagBits::e2; }

	return vk::SampleCountFlagBits::e1;
}

static bool hagl::isQueueFamilyComplete(const hagl::QueueFamilyIndices& indices) {
	return indices.graphicsAvail && indices.presentAvail;
}

void hagl::RenderSystem::createLogicalDevice() {
	if (_validationLayers.size() > 0 && !checkValidationLayerSupport(_validationLayers)) {
		throw new std::runtime_error("Requested validation layers not available!");
	}

	vk::PhysicalDeviceFeatures deviceFeatures {};
	deviceFeatures.samplerAnisotropy = true;

	std::vector<unsigned> queueFams = { _queueIndices.graphicsFamily };

	if (_queueIndices.graphicsFamily != _queueIndices.presentFamily) {
		queueFams.push_back(_queueIndices.presentFamily);
	}

	std::vector<vk::DeviceQueueCreateInfo> createInfos(queueFams.size());
	float queuePriority = 1.0f;

	for (int i = 0; i < createInfos.size(); i++) {
		createInfos[i].queueFamilyIndex = queueFams[i];
		createInfos[i].queueCount = 1;
		createInfos[i].pQueuePriorities = &queuePriority;
	}

	vk::DeviceCreateInfo deviceCreateInfo(
		{}, // Flags
		(uint32_t) createInfos.size(), createInfos.data(), // Queue create infos
		(uint32_t) _validationLayers.size(), _validationLayers.data(), // Enabled layers
		(uint32_t) _requiredDeviceExtensions.size(), _requiredDeviceExtensions.data(), // Extensions
		&deviceFeatures, // Device features
		NULL); // pNext

	_uDevice = _physicalDevice.createDeviceUnique(deviceCreateInfo);
	_graphicsQueue = _uDevice->getQueue(_queueIndices.graphicsFamily, 0);
	_presentQueue = _uDevice->getQueue(_queueIndices.presentFamily, 0);
}

static vk::UniqueShaderModule hagl::createShaderModule(const vk::Device& device, const std::vector<char>& bytes) {
	vk::ShaderModuleCreateInfo createInfo({}, bytes.size(), reinterpret_cast<const uint32_t*>(bytes.data()), nullptr);
	return device.createShaderModuleUnique(createInfo);
}

static vk::SurfaceFormatKHR hagl::chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR> formats) {

	for (auto format : formats) {
		if (format.format == vk::Format::eB8G8R8A8Srgb && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
			return format;
		}
	}

	return formats[0];
}

static vk::PresentModeKHR hagl::chooseSwapPresentMode(const std::vector<vk::PresentModeKHR> presentModes) {	
	for (auto mode : presentModes) {
		if (mode == vk::PresentModeKHR::eMailbox) {
			return mode;
		}
	}

	return vk::PresentModeKHR::eFifo;
}

static vk::Extent2D hagl::chooseSwapExtent(const hagl::WindowSystem& windowSystem, const vk::SurfaceCapabilitiesKHR& capabilities) {
	if (capabilities.currentExtent.width != UINT32_MAX) {
		return capabilities.currentExtent;
	} else {
		uint32_t width, height;
		windowSystem.getVulkanFramebufferSize(width, height);

		VkExtent2D actualExtent = {
			width,
			height
		};

		actualExtent.width = actualExtent.width > capabilities.minImageExtent.width
			? actualExtent.width
			: capabilities.minImageExtent.width;

		actualExtent.width = actualExtent.width < capabilities.maxImageExtent.width
			? actualExtent.width
			: capabilities.maxImageExtent.width;

		actualExtent.height = actualExtent.height > capabilities.minImageExtent.height
			? actualExtent.height
			: capabilities.minImageExtent.height;

		actualExtent.width = actualExtent.width < capabilities.maxImageExtent.width
			? actualExtent.width
			: capabilities.maxImageExtent.width;

		return actualExtent;
	}
}

static std::vector<char> hagl::readShaderBytes(const std::string& filePath) {
	std::ifstream file(filePath, std::ios::ate | std::ios::binary);
	

	if (!file.is_open()) {
		throw std::runtime_error("Cannot find file to read!\n");
	}

	size_t fileSize = (size_t)file.tellg();
	std::vector<char> bytes(fileSize);
	file.seekg(0);
	file.read(reinterpret_cast<char*>(bytes.data()), fileSize);
	file.close();

	return bytes;
}