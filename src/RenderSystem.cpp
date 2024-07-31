#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <vector>

#include "HaglConstants.h"
#include "HaglUtility.h"
#include "RenderSystem.h"

hagl::RenderSystem::RenderSystem(WindowSystem& window_system, uint32_t vertex_count, unsigned max_frames_in_flight)
	: _curr_frame(0),
	_max_frames_in_flight(max_frames_in_flight),
	_queue_indices(),
	_swapchain_format(),
	_vertex_count(vertex_count),
	_vertex_buffer_size(vertex_count * sizeof(Vertex)),
	_window_system(window_system)
{
	try {
		create_vk_instance();

		_surface_u = _window_system.create_vulkan_surface(*_instance_u);
		pick_physical_device();
		create_logical_device();
		create_swapchain();
		create_image_views();
		_render_pass_u = create_render_pass(_physical_device, *_unique_device_u, _swapchain_format, vk::SampleCountFlagBits::e1);
		create_descriptor_set_layout();
		create_graphics_pipeline();
		create_framebuffers();
		create_command_pool();
		create_command_buffer();
		create_sync_objects();
		create_vertex_buffers();
		create_uniform_buffers();
		create_descriptor_pool();
		create_descriptor_sets();
	}
	catch (std::exception const& e) {
		LOG_ERROR("Failed to initialize render system with error: %s", e.what());
		throw;
	}

	_window_system.register_framebuffer_resize_callback([this] { resize_framebuffer(); });
	_window_system.register_minimized_callback([this] { minimized(); });
	LOG_INFO("Render system initialized.");
}

hagl::RenderSystem::~RenderSystem() {
}

void hagl::copy_buffer(
	const vk::CommandBuffer& command_buffer,
	const vk::Buffer& src,
	const vk::Buffer& dst,
	vk::DeviceSize const size)
{
	vk::BufferCopy const copy_region(0, 0, size); // srcOffset, dstOffset, size
	command_buffer.copyBuffer(src, dst, copy_region);
}

void hagl::RenderSystem::create_buffer(
	size_t const size,
	vk::BufferUsageFlags const usage_flags,
	vk::SharingMode const sharing_mode,
	vk::MemoryPropertyFlags const memory_property_flags,
	vk::UniqueBuffer& buffer_u,
	vk::UniqueDeviceMemory& buffer_memory_u)
{
	vk::BufferCreateInfo const buffer_info(
		{}, // Flags
		size,
		usage_flags,
		sharing_mode);

	buffer_u = _unique_device_u->createBufferUnique(buffer_info);
	auto const mem_requirements = _unique_device_u->getBufferMemoryRequirements(*buffer_u);

	vk::MemoryAllocateInfo const allocate_info(
		mem_requirements.size,
		find_memory_type(mem_requirements.memoryTypeBits, memory_property_flags));

	buffer_memory_u = _unique_device_u->allocateMemoryUnique(allocate_info);
	_unique_device_u->bindBufferMemory(*buffer_u, *buffer_memory_u, 0);
}

void hagl::RenderSystem::create_command_buffer() {
	_command_buffers_u = _unique_device_u->allocateCommandBuffersUnique({
		*_command_pool_u,
		vk::CommandBufferLevel::ePrimary,
		_max_frames_in_flight });

	LOG_INFO("Command buffer created.");
}

void hagl::RenderSystem::create_command_pool() {
	auto queue_fam_indices = find_queue_families(_physical_device, *_surface_u);

	_command_pool_u = _unique_device_u->createCommandPoolUnique({
		vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
		queue_fam_indices.graphics_family
	});

	LOG_INFO("Command pool created.");
}

void hagl::RenderSystem::create_descriptor_sets() {
	// This line screams wrong at first glance - I'm taking a value in a resource handler and copying it
	// multiple times in a vector! BUT, remember that the handler here doesn't call delete - it calls Vulkan
	// Destroy commands. We don't actually *care* about the DescriptorSetLayout struct, it's a handle to the
	// actual resource inside of Vulkan. We can copy the handle as many times as we like.
	std::vector<vk::DescriptorSetLayout> layouts(_max_frames_in_flight, *_uDescriptorSetLayout);
	vk::DescriptorSetAllocateInfo const allocate_info(*_descriptor_pool_u, layouts);
	_descriptor_sets = _unique_device_u->allocateDescriptorSets(allocate_info);

	for (uint32_t i = 0; i < _max_frames_in_flight; i++) {
		vk::DescriptorBufferInfo buffer_info(*_uniform_buffers_u[i], 0, sizeof(Transform)); // Buffer, offset, range/size

		vk::WriteDescriptorSet descriptor_writes(
			_descriptor_sets[i],
			0, // Binding
			0, // Array element
			vk::DescriptorType::eUniformBuffer,
			nullptr, // Image info
			buffer_info,
			nullptr); // Texel buffer view

		_unique_device_u->updateDescriptorSets(descriptor_writes, nullptr);
	}

	LOG_INFO("Descriptor sets created.");
}

void hagl::RenderSystem::create_descriptor_pool() {
	vk::DescriptorPoolSize pool_size(vk::DescriptorType::eUniformBuffer, _max_frames_in_flight);
	vk::DescriptorPoolCreateInfo const pool_info({}, _max_frames_in_flight, pool_size); // Flags, max sets, pool sizes
	_descriptor_pool_u = _unique_device_u->createDescriptorPoolUnique(pool_info);
	LOG_INFO("Descriptor pool created.");
}

void hagl::RenderSystem::create_descriptor_set_layout() {
	vk::DescriptorSetLayoutBinding transform_layout_binding(
		0, // Binding
		vk::DescriptorType::eUniformBuffer,
		1, // Descriptor count
		vk::ShaderStageFlagBits::eVertex,
		nullptr); // Immutable samplers

	vk::DescriptorSetLayoutCreateInfo layout_info({}, transform_layout_binding); // Flags, bindings
	_uDescriptorSetLayout = _unique_device_u->createDescriptorSetLayoutUnique(layout_info);
	LOG_INFO("Descriptor set layout created.");
}

void hagl::RenderSystem::create_framebuffers() {
	_framebuffers_u.resize(_image_views_u.size());

	// TODO - create color and image framebuffers

	for (int i = 0; i < _framebuffers_u.size(); i++) {
		vk::FramebufferCreateInfo create_info(
			{}, // Flags
			*_render_pass_u,
			*_image_views_u[i], // Attachments
			_swapchain_extent.width,
			_swapchain_extent.height,
			1); // Layers

		_framebuffers_u[i] = _unique_device_u->createFramebufferUnique(create_info);
	}

	LOG_INFO("Framebuffers created.");
}

void hagl::RenderSystem::create_graphics_pipeline() {
	// TODO - make this configurable, probably a list of shaders to create or something
	std::vector<char> frag_shader_bytes = read_shader_bytes("shaders/frag.spv");
	std::vector<char> vert_shader_bytes = read_shader_bytes("shaders/vert.spv");

	vk::UniqueShaderModule frag_shader_module_u = create_shader_module(*_unique_device_u, frag_shader_bytes);
	vk::UniqueShaderModule vert_shader_module_u = create_shader_module(*_unique_device_u, vert_shader_bytes);

	vk::PipelineShaderStageCreateInfo frag_stage_create_info({}, vk::ShaderStageFlagBits::eFragment, *frag_shader_module_u, "main");
	vk::PipelineShaderStageCreateInfo vert_stage_create_info({}, vk::ShaderStageFlagBits::eVertex, *vert_shader_module_u, "main");

	std::vector<vk::PipelineShaderStageCreateInfo> shader_stages{ vert_stage_create_info, frag_stage_create_info };

	std::vector<vk::DynamicState> dynamic_states{
		vk::DynamicState::eViewport,
		vk::DynamicState::eScissor
	};

	vk::PipelineDynamicStateCreateInfo dynamic_state({}, dynamic_states);

	// Vertex input descriptions
	auto binding_desc = Vertex::get_binding_description();
	auto attribute_desc = Vertex::get_attribute_descriptions();

	// How we pass input to the vertex shader
	vk::PipelineVertexInputStateCreateInfo vertex_input_state(
		{}, // Flags
		binding_desc,
		attribute_desc);

	vk::PipelineInputAssemblyStateCreateInfo input_assembly(
		{}, // Flags
		vk::PrimitiveTopology::eTriangleList, //Topology
		vk::False); // Primitive restart

	vk::PipelineViewportStateCreateInfo viewport_state(
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
		vk::FrontFace::eCounterClockwise, // Front face
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
		*_uDescriptorSetLayout, // Descriptor set layouts
		nullptr); // Push constant ranges

	_pipeline_layout_u = _unique_device_u->createPipelineLayoutUnique(pipelineLayoutInfo);

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
		(uint32_t)shader_stages.size(),
		shader_stages.data(),
		&vertex_input_state, // Vertex input state
		&input_assembly,
		nullptr, // Tesselation state create info
		&viewport_state,
		&rasterizer,
		&multisampling,
		nullptr, // Depth stencil state
		&colorBlending,
		&dynamic_state,
		*_pipeline_layout_u,
		*_render_pass_u,
		0, // Subpass
		nullptr, // Base pipeline handle
		-1); // Base pipeline index

	auto result = _unique_device_u->createGraphicsPipeline(nullptr, pipelineCreateInfo);
	_graphics_pipeline_u = vk::UniquePipeline(result.value);
	LOG_INFO("Graphics pipeline created.");
}

void hagl::RenderSystem::create_image_views() {
	_image_views_u.clear();

	for (auto image : _images) {
		_image_views_u.push_back(create_image_view(_unique_device_u.get(), image, _swapchain_format, vk::ImageAspectFlagBits::eColor, 1));
	}

	LOG_INFO("Image views created.");
}

void hagl::RenderSystem::create_logical_device() {
	if (_validation_layers.size() > 0 && !check_validation_layer_support(_validation_layers)) {
		throw new std::runtime_error("Requested validation layers not available!");
	}

	vk::PhysicalDeviceFeatures deviceFeatures{};
	deviceFeatures.samplerAnisotropy = true;

	std::vector<unsigned> queueFams = { _queue_indices.graphics_family };

	if (_queue_indices.graphics_family != _queue_indices.present_family) {
		queueFams.push_back(_queue_indices.present_family);
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
		(uint32_t)createInfos.size(), createInfos.data(), // Queue create infos
		(uint32_t)_validation_layers.size(), _validation_layers.data(), // Enabled layers
		(uint32_t)_required_device_extensions.size(), _required_device_extensions.data(), // Extensions
		&deviceFeatures, // Device features
		NULL); // pNext

	_unique_device_u = _physical_device.createDeviceUnique(deviceCreateInfo);
	_graphics_queue = _unique_device_u->getQueue(_queue_indices.graphics_family, 0);
	_present_queue = _unique_device_u->getQueue(_queue_indices.present_family, 0);
	LOG_INFO("Logical device created.");
}

void hagl::RenderSystem::create_swapchain() {
	SwapchainSupportDetails details = {
		_physical_device.getSurfaceCapabilitiesKHR(*_surface_u),
		_physical_device.getSurfaceFormatsKHR(*_surface_u),
		_physical_device.getSurfacePresentModesKHR(*_surface_u)
	};

	vk::SurfaceFormatKHR surfaceFormat = choose_swap_surface_format(details.formats);
	vk::PresentModeKHR presentMode = choose_swap_present_mode(details.present_modes);
	vk::Extent2D extent = choose_swap_extent(_window_system, details.capabilities);
	uint32_t imageCount = details.capabilities.minImageCount + 1;

	imageCount = imageCount < details.capabilities.maxImageCount
		? imageCount
		: details.capabilities.maxImageCount;

	vk::SharingMode sharingMode = _queue_indices.graphics_family != _queue_indices.present_family
		? vk::SharingMode::eConcurrent
		: vk::SharingMode::eExclusive;

	uint32_t queueFamilyIndices[] = {
		_queue_indices.graphics_family,
		_queue_indices.present_family
	};

	vk::SwapchainCreateInfoKHR createInfo(
		{} // flags
		, * _surface_u
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
		, *_swapchain_u); // Old swapchain

	_swapchain_u = _unique_device_u->createSwapchainKHRUnique(createInfo);
	_images = _unique_device_u->getSwapchainImagesKHR(*_swapchain_u);
	_swapchain_extent = extent;
	_swapchain_format = surfaceFormat.format;
	LOG_INFO("Swapchain created.");
}

void hagl::RenderSystem::create_sync_objects() {
	_image_available_sems_u.resize(_max_frames_in_flight);
	_render_finished_sems_u.resize(_max_frames_in_flight);
	_in_flight_fences_u.resize(_max_frames_in_flight);

	for (unsigned i = 0; i < _max_frames_in_flight; i++) {
		_image_available_sems_u[i] = _unique_device_u->createSemaphoreUnique({});
		_render_finished_sems_u[i] = _unique_device_u->createSemaphoreUnique({});
		_in_flight_fences_u[i] = _unique_device_u->createFenceUnique({ vk::FenceCreateFlagBits::eSignaled });
	}

	LOG_INFO("Synchronization objects created.");
}

void hagl::RenderSystem::create_uniform_buffers() {
	vk::DeviceSize bufferSize = sizeof(Transform);
	_uniform_buffers_u.resize(_max_frames_in_flight);
	_uniform_buffers_memory_u.resize(_max_frames_in_flight);
	_uniform_buffers_mapped.resize(_max_frames_in_flight);

	for (size_t i = 0; i < _max_frames_in_flight; i++) {
		create_buffer(
			bufferSize,
			vk::BufferUsageFlagBits::eUniformBuffer,
			vk::SharingMode::eExclusive,
			vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
			_uniform_buffers_u[i],
			_uniform_buffers_memory_u[i]);

		_uniform_buffers_mapped[i] = _unique_device_u->mapMemory(*_uniform_buffers_memory_u[i], 0, bufferSize);
	}
	
	LOG_INFO("Uniform buffers created.");
}

void hagl::RenderSystem::create_vertex_buffers() {
	create_buffer(
		_vertex_buffer_size,
		vk::BufferUsageFlagBits::eTransferSrc,
		vk::SharingMode::eExclusive,
		vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
		_staging_buffer_u,
		_staging_buffer_memory_u);

	_staging_buffer_mapped = _unique_device_u->mapMemory(*_staging_buffer_memory_u, 0, _vertex_buffer_size);

	create_buffer(
		_vertex_buffer_size,
		vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst,
		vk::SharingMode::eExclusive,
		vk::MemoryPropertyFlagBits::eDeviceLocal,
		_vertex_buffer_u,
		_vertex_buffer_memory_u);

	LOG_INFO("Vertex buffers created.");
}

void hagl::RenderSystem::create_vk_instance() {
	vk::ApplicationInfo info(
		APP_NAME, // Application Name
		1, // Application version
		NULL, // Engine Name
		1, // Engine Version
		VK_API_VERSION_1_3 // API Version
	);

	auto requiredInstanceExtensions = _window_system.get_extensions();

	vk::InstanceCreateInfo appCreateInfo(
		{}, // flags
		&info, // Application Info
		(uint32_t)_validation_layers.size(), // Enable layer count
		_validation_layers.data(), // Enabled layer names
		(uint32_t)requiredInstanceExtensions.size(), // Extension count
		requiredInstanceExtensions.data() // Extension names
	);

	_instance_u = vk::createInstanceUnique(appCreateInfo);
	LOG_INFO("Vulkan instance created.");
}

void hagl::RenderSystem::draw_frame(
	Transform transform,
	const std::vector<Vertex>& vertices,
	const std::vector<uint32_t> indices)
{
	if (_window_minimized) {
		_window_system.wait_while_minimized();
		_window_minimized = false;
	}

	transfer_vertices(vertices);
	_unique_device_u->waitForFences(*_in_flight_fences_u[_curr_frame], vk::True, UINT64_MAX);

	vk::Result result;
	uint32_t imageIndex;

	// TODO - figure out how to make this faster when in FIFO mode due to integrated graphics or whatever
	// https://stackoverflow.com/questions/22387586/measuring-execution-time-of-a-function-in-c
	std::tie(result, imageIndex) = _unique_device_u->acquireNextImageKHR(
		*_swapchain_u,
		UINT64_MAX,
		*_image_available_sems_u[_curr_frame],
		nullptr);

	if (result == vk::Result::eErrorOutOfDateKHR) {
		recreate_swapchain();
		return;
	}

	_unique_device_u->resetFences(*_in_flight_fences_u[_curr_frame]);
	_command_buffers_u[_curr_frame]->reset();
	update_uniform_buffer(transform);
	record_command_buffer(*_command_buffers_u[_curr_frame], imageIndex);
	vk::Flags<vk::PipelineStageFlagBits> waitDstStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;

	vk::SubmitInfo submitInfo({
		*_image_available_sems_u[_curr_frame],
		waitDstStage,
		*_command_buffers_u[_curr_frame],
		*_render_finished_sems_u[_curr_frame]});

	_graphics_queue.submit(submitInfo, *_in_flight_fences_u[_curr_frame]);

	vk::PresentInfoKHR presentInfo({
		*_render_finished_sems_u[_curr_frame],
		*_swapchain_u,
		imageIndex });

	try
	{
		result = _present_queue.presentKHR(presentInfo);
	} catch (vk::OutOfDateKHRError) {
		recreate_swapchain();
	}

	if (result == vk::Result::eSuboptimalKHR || _framebuffer_resized) {
		recreate_swapchain();
	}

	_curr_frame = (_curr_frame + 1) % _max_frames_in_flight;
}

uint32_t hagl::RenderSystem::find_memory_type(uint32_t type_filter, vk::MemoryPropertyFlags properties) {
	auto memProperties = _physical_device.getMemoryProperties();

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if ((type_filter & (1 << i))
			&& (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
		{
			return i;
		}
	}

	throw std::runtime_error("Failed to find a suitable memory type!");
}

void hagl::RenderSystem::minimized() {
	_window_minimized = true;
}

void hagl::RenderSystem::pick_physical_device() {
	uint32_t deviceCount = 0;
	auto devices = _instance_u->enumeratePhysicalDevices();

	if (devices.size() == 0) {
		throw std::runtime_error("Failed to find a GPU with Vulkan support!");
	}

	for (auto device : devices) {
		if (is_device_suitable(device, *_surface_u, _required_device_extensions)) {
			_physical_device = device;
			_msaa_samples = get_max_usable_sample_count(device);
			_queue_indices = find_queue_families(device, *_surface_u);
			LOG_INFO("Physical device selected.");
			return;
		}
	}

	throw new std::runtime_error("Could not find a suitable GPU!");
}

void hagl::RenderSystem::record_command_buffer(vk::CommandBuffer& command_buffer, uint32_t image_index) {
	vk::CommandBufferBeginInfo beginInfo;
	command_buffer.begin(beginInfo);
	// I'm not sure this belongs in *this* function, but this is functionally where it should happen
	copy_buffer(*_command_buffers_u[_curr_frame], *_staging_buffer_u, *_vertex_buffer_u, _vertex_buffer_size);
	constexpr vk::ClearColorValue clear_color_value = vk::ClearColorValue { 0.0f, 0.0f, 0.0f, 1.0f };
	vk::ClearValue clear_value = vk::ClearValue(clear_color_value);

	vk::RenderPassBeginInfo render_pass({
		*_render_pass_u,
		*_framebuffers_u[image_index],
		{ vk::Offset2D(0, 0), _swapchain_extent }, // Render area
		clear_value });

	command_buffer.beginRenderPass(render_pass, vk::SubpassContents::eInline);
	command_buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *_graphics_pipeline_u);
	command_buffer.bindVertexBuffers(0, *_vertex_buffer_u, { 0 }); // First binding, buffer, offsets

	vk::Viewport viewport {
		0, 0, // x and y
		static_cast<float>(_swapchain_extent.width),
		static_cast<float>(_swapchain_extent.height),
		0.0f, // Min depth
		1.0f }; // Max depth
		
	command_buffer.setViewport(0, viewport);

	vk::Rect2D scissor({ 0, 0 }, _swapchain_extent);
	command_buffer.setScissor(0, scissor);

	command_buffer.bindDescriptorSets(
		vk::PipelineBindPoint::eGraphics,
		*_pipeline_layout_u,
		0, // First set
		_descriptor_sets[_curr_frame],
		nullptr); // Dynamic offsets

	command_buffer.draw(_vertex_count, 1, 0, 0); // Vertex count, instance count, first vertex, first instance
	command_buffer.endRenderPass();
	command_buffer.end();
}

void hagl::RenderSystem::recreate_swapchain() {
	LOG_INFO("Resizing framebuffer!");
	_window_system.wait_while_minimized(); // Probably unnecessary, but it's safe
	_framebuffer_resized = false;
	_window_minimized = false;

	_unique_device_u->waitIdle();

	// TODO - try recreating the swap chain while the old one is in-flight!

	create_swapchain();
	create_image_views();
	create_framebuffers();
}

inline void hagl::RenderSystem::resize_framebuffer() {
	_framebuffer_resized = true;
}

// TODO - use a different queue family for transfer operations
// https://docs.vulkan.org/tutorial/latest/00_Introduction.html
void hagl::RenderSystem::transfer_vertices(const std::vector<Vertex>& vertices) {
	memcpy(_staging_buffer_mapped, vertices.data(), _vertex_buffer_size);
}

void hagl::RenderSystem::update_uniform_buffer(Transform transform) {
	transform.projection[1][1] *= -1;
	memcpy(_uniform_buffers_mapped[_curr_frame], &transform, sizeof(transform));
}

#pragma region statics

static bool hagl::check_device_extension_support(const vk::PhysicalDevice& device, std::vector<const char*> required_device_extensions) {
	auto deviceExtensions = device.enumerateDeviceExtensionProperties();
	std::vector<std::string> supported;

	for (auto ext : deviceExtensions) {
		supported.push_back(std::string((char*)ext.extensionName));
	}
	
	for (auto reqExtension : required_device_extensions) {
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

static bool hagl::check_validation_layer_support(const std::vector<const char*>& validation_layers) {
	auto supportedLayers = vk::enumerateInstanceLayerProperties();

	for (auto reqLayer : validation_layers) {
		bool found = false;

		for (auto supportedLayer : supportedLayers) {
			found = found || strcmp(supportedLayer.layerName, reqLayer) == 0;
		}

		if (!found) {
			return false;
		}
	}

	return true;
}

static vk::Extent2D hagl::choose_swap_extent(const hagl::WindowSystem& windowSystem, const vk::SurfaceCapabilitiesKHR& capabilities) {
	if (capabilities.currentExtent.width != UINT32_MAX) {
		return capabilities.currentExtent;
	}
	else {
		uint32_t width, height;
		windowSystem.get_vulkan_framebuffer_size(width, height);

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

static vk::PresentModeKHR hagl::choose_swap_present_mode(const std::vector<vk::PresentModeKHR> presentModes) {
	for (auto mode : presentModes) {
		if (mode == vk::PresentModeKHR::eMailbox) {
			return mode;
		}
	}

	return vk::PresentModeKHR::eFifo;
}

static vk::SurfaceFormatKHR hagl::choose_swap_surface_format(const std::vector<vk::SurfaceFormatKHR> formats) {

	for (auto format : formats) {
		if (format.format == vk::Format::eB8G8R8A8Srgb && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
			return format;
		}
	}

	return formats[0];
}

static vk::UniqueImageView hagl::create_image_view(const vk::Device& device, const vk::Image& image, vk::Format format, vk::ImageAspectFlags aspect_mask, uint32_t mip_levels) {
	vk::ComponentMapping components(
		vk::ComponentSwizzle::eIdentity,
		vk::ComponentSwizzle::eIdentity,
		vk::ComponentSwizzle::eIdentity,
		vk::ComponentSwizzle::eIdentity
	);

	vk::ImageSubresourceRange subresource(
		aspect_mask,
		0, // Base mip level
		mip_levels,
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

vk::UniqueRenderPass hagl::create_render_pass(const vk::PhysicalDevice& physical_device, const vk::Device& device, const vk::Format& format, vk::SampleCountFlagBits samples) {
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

static vk::UniqueShaderModule hagl::create_shader_module(const vk::Device& device, const std::vector<char>& bytes) {
	vk::ShaderModuleCreateInfo createInfo({}, bytes.size(), reinterpret_cast<const uint32_t*>(bytes.data()), nullptr);
	return device.createShaderModuleUnique(createInfo);
}

static vk::Format hagl::find_depth_format(const vk::PhysicalDevice& physical_device) {
	std::vector<vk::Format> formats = {
		vk::Format::eD32Sfloat,
		vk::Format::eD32SfloatS8Uint,
		vk::Format::eD24UnormS8Uint
	};

	vk::ImageTiling tiling = vk::ImageTiling::eOptimal;
	vk::FormatFeatureFlags features = vk::FormatFeatureFlagBits::eDepthStencilAttachment;
	vk::FormatProperties props;

	for (auto format : formats) {
		props = physical_device.getFormatProperties(format);

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

static hagl::QueueFamilyIndices hagl::find_queue_families(const vk::PhysicalDevice& device, const vk::SurfaceKHR& surface) {
	QueueFamilyIndices deviceIndices{};
	auto queueFamilyProperties = device.getQueueFamilyProperties();

	for (int i = 0; i < queueFamilyProperties.size() && !is_queue_family_complete(deviceIndices); i++) {
		if (queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eGraphics) {
			deviceIndices.graphics_family = i;
			deviceIndices.graphics_avail = true;
		}

		if (device.getSurfaceSupportKHR(i, surface)) {
			deviceIndices.present_family = i;
			deviceIndices.present_avail = true;
		}
	}

	return deviceIndices;
}

static vk::SampleCountFlagBits hagl::get_max_usable_sample_count(const vk::PhysicalDevice& physical_device) {
	auto physicalDeviceProperties = physical_device.getProperties();

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

static bool hagl::is_device_suitable(const vk::PhysicalDevice& device, const vk::SurfaceKHR& surface, const std::vector<const char*>& required_device_extensions) {
	QueueFamilyIndices deviceIndices = hagl::find_queue_families(device, surface);
	auto deviceProperties = device.getProperties();
	auto deviceFeatures = device.getFeatures();

	bool extensionsSupported = hagl::check_device_extension_support(device, required_device_extensions);
	bool swapchainAdequate = false;

	if (extensionsSupported) {
		hagl::SwapchainSupportDetails details = {
			device.getSurfaceCapabilitiesKHR(surface),
			device.getSurfaceFormatsKHR(surface),
			device.getSurfacePresentModesKHR(surface)
		};

		swapchainAdequate = details.formats.size() > 0 && details.present_modes.size() > 0;
	}

	return (
		(deviceProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
		&& deviceFeatures.geometryShader
		&& deviceFeatures.samplerAnisotropy
		&& hagl::is_queue_family_complete(deviceIndices)
		&& extensionsSupported
		&& swapchainAdequate
	);
}

static bool hagl::is_queue_family_complete(const hagl::QueueFamilyIndices& indices) {
	return indices.graphics_avail && indices.present_avail;
}

static std::vector<char> hagl::read_shader_bytes(const std::string& file_path) {
	std::ifstream file(file_path, std::ios::ate | std::ios::binary);

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