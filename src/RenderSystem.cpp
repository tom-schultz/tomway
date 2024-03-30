#include "RenderSystem.h"
#include "HaglConstants.h"
#include <iostream>
#include <algorithm>
#include "HaglUtility.h"

hagl::RenderSystem::RenderSystem(WindowSystem& windowSystem)
	: _windowSystem(windowSystem),
	_queueIndices()
{
}

hagl::RenderSystem::~RenderSystem() {

}

void hagl::RenderSystem::init() {
	try {
		createVkInstance();

		VkSurfaceKHR cSurface;
		_windowSystem.createVulkanSurface(_vkInstance, cSurface);
		_surface = vk::SurfaceKHR(cSurface);
		pickPhysicalDevice();
	}
	catch (std::exception e) {
		LOG_ERROR(0, "Failed to initialize Vulkan with error: %s", e.what());
		exit(1);
	}

	LOG_INFO("Render system initialized.");
}

void hagl::RenderSystem::pickPhysicalDevice() {
	uint32_t deviceCount = 0;
	auto devices = _vkInstance.enumeratePhysicalDevices();

	if (devices.size() == 0) {
		throw std::runtime_error("Failed to find a GPU with Vulkan support!");
	}
	
	for (auto device : devices) {
		QueueFamilyIndices deviceIndices = findQueueFamilies(device);

		if (isDeviceSuitable(device, deviceIndices)) {
			_physicalDevice = device;
			_msaaSamples = getMaxUsableSampleCount();
			_queueIndices = deviceIndices;
			return;
		}
	}

	throw new std::runtime_error("Could not find a suitable GPU!");
}

void hagl::RenderSystem::createVkInstance() {
	vk::ApplicationInfo info(
				APP_NAME, // Application Name
				1, // Application version
				NULL, // Engine Name
				1, // Engine Version
				VK_API_VERSION_1_3 // API Version
			);

	_requiredExtensions = _windowSystem.getExtensions();

	vk::InstanceCreateInfo appCreateInfo(
		{}, // flags
		&info, // Application Info
		(uint32_t) _validationLayers.size(), // Enable layer count
		_validationLayers.data(), // Enabled layer names
		(uint32_t) _requiredExtensions.size(), // Extension count
		_requiredExtensions.data() // Extension names
	);

	_vkInstance = vk::createInstance(appCreateInfo);
}

void hagl::RenderSystem::createSwapchain() {
	SwapchainSupportDetails details = {
			_physicalDevice.getSurfaceCapabilitiesKHR(_surface),
			_physicalDevice.getSurfaceFormatsKHR(_surface),
			_physicalDevice.getSurfacePresentModesKHR(_surface)
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
		, _surface
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

	_swapchain = _device.createSwapchainKHR(createInfo);
	_images = _device.getSwapchainImagesKHR(_swapchain);
	_swapchainExtent = extent;
	_swapchainFormat = surfaceFormat.format;
}

hagl::RenderSystem::QueueFamilyIndices hagl::RenderSystem::findQueueFamilies(const vk::PhysicalDevice& device) {
	QueueFamilyIndices deviceIndices{};
	auto queueFamilyProperties = device.getQueueFamilyProperties();

	for (int i = 0; i < queueFamilyProperties.size() && !isQueueFamilyComplete(deviceIndices); i++) {
		if (queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eGraphics) {
			deviceIndices.graphicsFamily = i;
			deviceIndices.graphicsAvail = true;
		}

		if (device.getSurfaceSupportKHR(i, _surface)) {
			deviceIndices.presentFamily = i;
			deviceIndices.presentAvail = true;
		}
	}

	return deviceIndices;
}

bool hagl::RenderSystem::checkDeviceExtensionSupport(const vk::PhysicalDevice& device) {
	auto deviceExtensions = device.enumerateDeviceExtensionProperties();

	for (auto reqExtension : _requiredExtensions) {
		bool found = false;

		for (auto devExtension : deviceExtensions) {
			found = found || strcmp(devExtension.extensionName, reqExtension);
		}

		if (!found) {
			return false;
		}
	}

	return true;
}

bool hagl::RenderSystem::checkValidationLayerSupport() {
	auto supportedLayers = vk::enumerateInstanceLayerProperties();

	for (auto reqLayer : _validationLayers) {
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

bool hagl::RenderSystem::isDeviceSuitable(const vk::PhysicalDevice& device, const QueueFamilyIndices& indices) {
	auto deviceProperties = device.getProperties();
	auto deviceFeatures = device.getFeatures();

	bool extensionsSupported = checkDeviceExtensionSupport(device);
	bool swapchainAdequate = false;

	if (extensionsSupported) {
		SwapchainSupportDetails details = {
			device.getSurfaceCapabilitiesKHR(_surface),
			device.getSurfaceFormatsKHR(_surface),
			device.getSurfacePresentModesKHR(_surface)
		};

		swapchainAdequate = details.formats.size() > 0 && details.presentModes.size() > 0;
	}

	return (
		(deviceProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu
			|| deviceProperties.deviceType == vk::PhysicalDeviceType::eIntegratedGpu)
		&& deviceFeatures.geometryShader
		&& deviceFeatures.samplerAnisotropy
		&& isQueueFamilyComplete(indices)
		&& extensionsSupported
		&& swapchainAdequate
	);
}

vk::SampleCountFlagBits hagl::RenderSystem::getMaxUsableSampleCount() {
	auto physicalDeviceProperties = _physicalDevice.getProperties();

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

bool hagl::RenderSystem::isQueueFamilyComplete(QueueFamilyIndices indices) {
	return indices.graphicsAvail && indices.presentAvail;
}

void hagl::RenderSystem::createLogicalDevice() {
	if (_validationLayers.size() > 0 && !checkValidationLayerSupport()) {
		throw new std::runtime_error("Requested validation layers not available!");
	}
	else {
		LOG_INFO("Validation layers available!\n");
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
		(uint32_t) _requiredExtensions.size(), _requiredExtensions.data(), // Extensions
		&deviceFeatures, // Device features
		NULL); // pNext

	_device = _physicalDevice.createDevice(deviceCreateInfo);
	_graphicsQueue = _device.getQueue(_queueIndices.graphicsFamily, 0);
	_presentQueue = _device.getQueue(_queueIndices.presentFamily, 0);
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