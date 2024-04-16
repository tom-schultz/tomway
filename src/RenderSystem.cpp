#include "RenderSystem.h"
#include "HaglConstants.h"
#include <iostream>
#include <algorithm>
#include "HaglUtility.h"
#include <vector>
#include <fstream>

hagl::RenderSystem::RenderSystem(WindowSystem& windowSystem, uint32_t vertexCount, unsigned maxFramesInFlight)
	: _windowSystem(windowSystem),
	_currFrame(0),
	_maxFramesInFlight(maxFramesInFlight),
	_queueIndices(),
	_swapchainFormat(),
	_vertexCount(vertexCount),
	_vertexBufferSize(vertexCount * sizeof(Vertex))
{
	try {
		createVkInstance();

		_uSurface = _windowSystem.createVulkanSurface(*_uInstance);
		pickPhysicalDevice();
		createLogicalDevice();
		createSwapchain();
		createImageViews();
		_uRenderPass = createRenderPass(_physicalDevice, *_uDevice, _swapchainFormat, vk::SampleCountFlagBits::e1);
		createGraphicsPipeline();
		createFramebuffers();
		createCommandPool();
		createCommandBuffer();
		createSyncObjects();

		createBuffer(
			_vertexBufferSize,
			vk::BufferUsageFlagBits::eVertexBuffer,
			vk::SharingMode::eExclusive,
			vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
			_uVertexBuffer,
			_uVertexBufferMemory);
	}
	catch (std::exception e) {
		LOG_ERROR(0, "Failed to initialize render system with error: %s", e.what());
		throw e;
	}

	_windowSystem.registerFramebufferResizeCallback(std::bind(&RenderSystem::resizeFramebuffer, this));
	_windowSystem.registerMinimizedCallback(std::bind(&RenderSystem::minimized, this));
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
		vk::ImageLayout::ePresentSrcKHR // Final layout
	);

	vk::AttachmentReference colorAttachmentRef(
		0, // Attachment
		vk::ImageLayout::eColorAttachmentOptimal
	);

	//vk::AttachmentDescription resolveAttachment(
	//	{}, // Flags
	//	format, // Format
	//	vk::SampleCountFlagBits::e1, // Sample count
	//	vk::AttachmentLoadOp::eDontCare, // Load op
	//	vk::AttachmentStoreOp::eStore, // Store op
	//	vk::AttachmentLoadOp::eDontCare, // Stencil load op
	//	vk::AttachmentStoreOp::eDontCare, // Stencil store op
	//	vk::ImageLayout::eUndefined, // Initial layout
	//	vk::ImageLayout::ePresentSrcKHR // Final layout
	//);

	//vk::AttachmentReference resolveAttachmentRef(
	//	1, // Attachment
	//	vk::ImageLayout::eColorAttachmentOptimal
	//);

	//vk::AttachmentDescription depthAttachment(
	//	{}, // Flags
	//	hagl::findDepthFormat(physicalDevice), // Format
	//	samples, // Sample count
	//	vk::AttachmentLoadOp::eClear, // Load op
	//	vk::AttachmentStoreOp::eStore, // Store op
	//	vk::AttachmentLoadOp::eDontCare, // Stencil load op
	//	vk::AttachmentStoreOp::eDontCare, // Stencil store op
	//	vk::ImageLayout::eUndefined, // Initial layout
	//	vk::ImageLayout::eDepthStencilAttachmentOptimal // Final layout
	//);

	//vk::AttachmentReference depthAttachmentRef(
	//	2, // Attachment
	//	vk::ImageLayout::eDepthStencilAttachmentOptimal
	//);

	vk::SubpassDescription subpass(
		{}, // Flags
		vk::PipelineBindPoint::eGraphics,
		nullptr, // Input attachments
		colorAttachmentRef, // Color attachment refs
		//resolveAttachmentRef, // Resolve attachment refs
		nullptr,
		//&depthAttachmentRef, // Depth attachment ref, ptr for some reason...
		nullptr // Preserve attachments
	);

	vk::SubpassDependency dependency(
		VK_SUBPASS_EXTERNAL, // Source subpass
		0, // Dest subpass
		vk::PipelineStageFlagBits::eColorAttachmentOutput, // Source stage mask
		vk::PipelineStageFlagBits::eColorAttachmentOutput, // Dest stage mask
		vk::AccessFlagBits::eNone, // Source access mask
		vk::AccessFlagBits::eColorAttachmentWrite, // Dest access mask
		{} // Dependency flags
	);

	vk::AttachmentDescription attachments[]{
		colorAttachment,
		//resolveAttachment,
		//depthAttachment,
	};

	vk::RenderPassCreateInfo createInfo(
		{}, // Flags
		attachments,
		subpass,
		dependency
	);

	return device.createRenderPassUnique(createInfo);
}

void hagl::RenderSystem::createBuffer(
	size_t size,
	vk::BufferUsageFlagBits usageFlags,
	vk::SharingMode sharingMode,
	vk::MemoryPropertyFlags memoryPropertyFlags,
	vk::UniqueBuffer& uBuffer,
	vk::UniqueDeviceMemory& uBufferMemory)
{
	vk::BufferCreateInfo bufferInfo(
		{}, // Flags
		size,
		usageFlags,
		sharingMode);

	uBuffer = _uDevice->createBufferUnique(bufferInfo);
	auto memRequirements = _uDevice->getBufferMemoryRequirements(*uBuffer);

	vk::MemoryAllocateInfo allocInfo(
		memRequirements.size,
		findMemoryType(memRequirements.memoryTypeBits, memoryPropertyFlags));

	uBufferMemory = _uDevice->allocateMemoryUnique(allocInfo);
	_uDevice->bindBufferMemory(*uBuffer, *uBufferMemory, 0);
}

void hagl::RenderSystem::createCommandBuffer() {
	_uCommandBuffers = _uDevice->allocateCommandBuffersUnique({
		*_uCommandPool,
		vk::CommandBufferLevel::ePrimary,
		_maxFramesInFlight });
}

void hagl::RenderSystem::createCommandPool() {
	auto queueFamIndices = findQueueFamilies(_physicalDevice, *_uSurface);

	_uCommandPool = _uDevice->createCommandPoolUnique({
		vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
		queueFamIndices.graphicsFamily
	});
}

void hagl::RenderSystem::createFramebuffers() {
	_uFramebuffers.resize(_uImageViews.size());

	// TODO - create color and image framebuffers

	for (int i = 0; i < _uFramebuffers.size(); i++) {
		vk::FramebufferCreateInfo createInfo(
			{}, // Flags
			*_uRenderPass,
			*_uImageViews[i], // Attachments
			_swapchainExtent.width,
			_swapchainExtent.height,
			1); // Layers

		_uFramebuffers[i] = _uDevice->createFramebufferUnique(createInfo);
	}
}

void hagl::RenderSystem::createGraphicsPipeline() {
	// TODO - make this configurable, probably a list of shaders to create or something
	std::vector<char> _fragShaderBytes = readShaderBytes("shaders/frag.spv");
	std::vector<char> _vertShaderBytes = readShaderBytes("shaders/vert.spv");

	vk::UniqueShaderModule uFragShaderModule = createShaderModule(*_uDevice, _fragShaderBytes);
	vk::UniqueShaderModule uVertShaderModule = createShaderModule(*_uDevice, _vertShaderBytes);

	vk::PipelineShaderStageCreateInfo fragStageCreateInfo({}, vk::ShaderStageFlagBits::eFragment, *uFragShaderModule, "main");
	vk::PipelineShaderStageCreateInfo vertStageCreateInfo({}, vk::ShaderStageFlagBits::eVertex, *uVertShaderModule, "main");

	std::vector<vk::PipelineShaderStageCreateInfo> shaderStages{ vertStageCreateInfo, fragStageCreateInfo };

	std::vector<vk::DynamicState> dynamicStates{
		vk::DynamicState::eViewport,
		vk::DynamicState::eScissor
	};

	vk::PipelineDynamicStateCreateInfo dynamicState({}, dynamicStates);

	// Vertex input descriptions
	auto bindingDesc = Vertex::getBindingDescription();
	auto attributeDesc = Vertex::getAttributeDescriptions();

	// How we pass input to the vertex shader
	vk::PipelineVertexInputStateCreateInfo vertexInputState(
		{}, // Flags
		bindingDesc,
		attributeDesc);

	vk::PipelineInputAssemblyStateCreateInfo inputAssembly(
		{}, // Flags
		vk::PrimitiveTopology::eTriangleList, //Topology
		vk::False); // Primitive restart

	vk::PipelineViewportStateCreateInfo viewportState(
		{}, // Flags
		1, // Viewport count
		nullptr, // Viewports, null here because we're setting them dynamically
		1); // Scissor count

	vk::PipelineRasterizationStateCreateInfo rasterizer(
		{}, //Flags
		vk::False, // Depth clamp enable
		vk::False, // Rasterizer discard enable
		vk::PolygonMode::eFill, // Polygon mode
		vk::CullModeFlagBits::eBack, // Cull mode
		vk::FrontFace::eClockwise, // Front face
		vk::False, // Depth bias enable
		0.0f, // Depth bias constant factor
		0.0f, // Depth bias clamp
		0.0f, // Depth bias slope factor
		1.0f); // Line width

	vk::PipelineMultisampleStateCreateInfo multisampling(
		{}, // Flags
		vk::SampleCountFlagBits::e1,
		vk::False, // Sample shading enable
		1.0f, // Min sample shading
		nullptr, // Sample mask
		vk::False, // Alpha to coverage enable
		vk::False); // Alpha to one enable

	vk::PipelineColorBlendAttachmentState colorBlendAttachment(
		vk::False, // Blend enable
		vk::BlendFactor::eSrcAlpha, // Src color blend factor
		vk::BlendFactor::eOneMinusSrcAlpha, // Dst color blend factor
		vk::BlendOp::eAdd, // Color blend op
		vk::BlendFactor::eOne, // Src alpha blend factor
		vk::BlendFactor::eZero, // Dst alpha blend factor
		vk::BlendOp::eAdd, // Alpha blend op
		vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA); 

	vk::PipelineColorBlendStateCreateInfo colorBlending(
		{}, // Flags
		vk::False, // Logic op enable
		vk::LogicOp::eCopy, // Logic op
		colorBlendAttachment, // Attachments
		{ 0.0f, 0.0f, 0.0f, 0.0f }); // Blend constants

	vk::PipelineLayoutCreateInfo pipelineLayoutInfo(
		{}, // Flags
		nullptr, // Descriptor set layouts
		nullptr); // Push constant ranges

	_uPipelineLayout = _uDevice->createPipelineLayoutUnique(pipelineLayoutInfo);

	// TODO - Depth stencil create info
	/*
	VkPipelineDepthStencilStateCreateInfo depthStencil = {
		.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
		.depthTestEnable = VK_TRUE,
		.depthWriteEnable = VK_TRUE,
		.depthCompareOp = VK_COMPARE_OP_LESS,
		.depthBoundsTestEnable = VK_FALSE,
		.minDepthBounds = 0.0f,
		.maxDepthBounds = 1.0f,
		.stencilTestEnable = VK_FALSE,
		.front = {},
		.back = {}
	};
	*/

	vk::GraphicsPipelineCreateInfo pipelineCreateInfo(
		{}, // Flags
		(uint32_t)shaderStages.size(),
		shaderStages.data(),
		&vertexInputState, // Vertex input state
		&inputAssembly,
		nullptr, // Tesselation state create info
		&viewportState,
		&rasterizer,
		&multisampling,
		nullptr, // Depth stencil state
		&colorBlending,
		&dynamicState,
		*_uPipelineLayout,
		*_uRenderPass,
		0, // Subpass
		nullptr, // Base pipeline handle
		-1); // Base pipeline index

	auto result = _uDevice->createGraphicsPipeline(nullptr, pipelineCreateInfo);
	_uGraphicsPipeline = vk::UniquePipeline(result.value);
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

void hagl::RenderSystem::createSyncObjects() {
	_uImageAvailableSems.resize(_maxFramesInFlight);
	_uRenderFinishedSems.resize(_maxFramesInFlight);
	_uInFlightFences.resize(_maxFramesInFlight);

	for (unsigned i = 0; i < _maxFramesInFlight; i++) {
		_uImageAvailableSems[i] = _uDevice->createSemaphoreUnique({});
		_uRenderFinishedSems[i] = _uDevice->createSemaphoreUnique({});
		_uInFlightFences[i] = _uDevice->createFenceUnique({ vk::FenceCreateFlagBits::eSignaled });
	}
}

void hagl::RenderSystem::createImageViews() {
	_uImageViews.clear();

	for (auto image : _images) {
		_uImageViews.push_back(createImageView(_uDevice.get(), image, _swapchainFormat, vk::ImageAspectFlagBits::eColor, 1));
	}
}

void hagl::RenderSystem::drawFrame(
	const Transform& transform,
	const std::vector<Vertex>& vertices,
	const std::vector<uint32_t> indices)
{
	if (_windowMinimized) {
		_windowSystem.waitWhileMinimized();
		_windowMinimized = false;
	}

	_uDevice->waitForFences(*_uInFlightFences[_currFrame], vk::True, UINT64_MAX);
	transferVertices(vertices);

	vk::Result result;
	uint32_t imageIndex;

	std::tie(result, imageIndex) = _uDevice->acquireNextImageKHR(
		*_uSwapchain,
		UINT64_MAX,
		*_uImageAvailableSems[_currFrame],
		nullptr);

	if (result == vk::Result::eErrorOutOfDateKHR) {
		recreateSwapchain();
		return;
	}

	_uDevice->resetFences(*_uInFlightFences[_currFrame]);
	_uCommandBuffers[_currFrame]->reset();
	recordCommandBuffer(*_uCommandBuffers[_currFrame], imageIndex);
	vk::Flags<vk::PipelineStageFlagBits> waitDstStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;

	vk::SubmitInfo submitInfo({
		*_uImageAvailableSems[_currFrame],
		waitDstStage,
		*_uCommandBuffers[_currFrame],
		*_uRenderFinishedSems[_currFrame]});

	_graphicsQueue.submit(submitInfo, *_uInFlightFences[_currFrame]);

	vk::PresentInfoKHR presentInfo({
		*_uRenderFinishedSems[_currFrame],
		*_uSwapchain,
		imageIndex });

	result = _presentQueue.presentKHR(presentInfo);

	if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR || _framebufferResized) {
		recreateSwapchain();
	}

	_currFrame = (_currFrame + 1) % _maxFramesInFlight;
}

uint32_t hagl::RenderSystem::findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags properties) {
	auto memProperties = _physicalDevice.getMemoryProperties();

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if ((typeFilter & (1 << i))
			&& (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
		{
			return i;
		}
	}

	throw std::runtime_error("Failed to find a suitable memory type!");
}

void hagl::RenderSystem::minimized() {
	_windowMinimized = true;
}

void hagl::RenderSystem::recordCommandBuffer(vk::CommandBuffer& commandBuffer, uint32_t imageIndex) {
	vk::CommandBufferBeginInfo beginInfo;
	commandBuffer.begin(beginInfo);
	vk::ClearValue clearValue = vk::ClearValue({ { 0.0f, 0.0f, 0.0f, 1.0f } });

	vk::RenderPassBeginInfo renderPass({
		*_uRenderPass,
		*_uFramebuffers[imageIndex],
		{ vk::Offset2D(0, 0), _swapchainExtent }, // Render area
		clearValue });

	commandBuffer.beginRenderPass(renderPass, vk::SubpassContents::eInline);
	commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *_uGraphicsPipeline);
	commandBuffer.bindVertexBuffers(0, *_uVertexBuffer, { 0 }); // First binding, buffer, offsets

	vk::Viewport viewport({
		0, 0, // x and y
		static_cast<float>(_swapchainExtent.width),
		static_cast<float>(_swapchainExtent.height),
		0.0f, // Min depth
		1.0f }); // Max depth
		
	commandBuffer.setViewport(0, viewport);

	vk::Rect2D scissor({ 0, 0 }, _swapchainExtent);
	commandBuffer.setScissor(0, scissor);
	commandBuffer.draw(_vertexCount, 1, 0, 0); // Vertex count, instance count, first vertex, first instance
	commandBuffer.endRenderPass();
	commandBuffer.end();
}

void hagl::RenderSystem::recreateSwapchain() {
	LOG_INFO("Resizing framebuffer!");
	_windowSystem.waitWhileMinimized(); // Probably unnecessary, but it's safe
	_framebufferResized = false;
	_windowMinimized = false;

	_uDevice->waitIdle();

	// TODO - try recreating the swap chain while the old one is in-flight!

	createSwapchain();
	createImageViews();
	createFramebuffers();
}

inline void hagl::RenderSystem::resizeFramebuffer() {
	_framebufferResized = true;
}

// TODO - use a different queue family for transfer operations
// https://docs.vulkan.org/tutorial/latest/00_Introduction.html
void hagl::RenderSystem::transferVertices(const std::vector<Vertex>& vertices) {
	void* data = _uDevice->mapMemory(*_uVertexBufferMemory, 0, _vertexBufferSize);
	memcpy(data, vertices.data(), _vertexBufferSize);
	_uDevice->unmapMemory(*_uVertexBufferMemory);
}

#pragma region statics

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

static vk::Format hagl::findDepthFormat(const vk::PhysicalDevice& physicalDevice) {
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

#pragma endregion