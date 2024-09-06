// RenderSystem.hpp : 

#pragma once

#ifdef NDEBUG
	#define VALIDATION_LAYERS
#else
	#define VALIDATION_LAYERS "VK_LAYER_KHRONOS_validation"
#endif

#include <vulkan/vulkan.hpp>

#include "CellGeometry.h"
#include "Transform.h"
#include "Vertex.h"
#include "WindowSystem.h"

namespace tomway {
	struct QueueFamilyIndices {
		uint32_t graphics_family;
		bool graphics_avail;
		uint32_t present_family;
		bool present_avail;
	};

	struct SwapchainSupportDetails {
		vk::SurfaceCapabilitiesKHR capabilities;
		std::vector<vk::SurfaceFormatKHR> formats;
		std::vector<vk::PresentModeKHR> present_modes;
	};

	class RenderSystem {
	public:
		RenderSystem(WindowSystem& window_system, CellGeometry& cell_geometry, size_t max_vertex_count, unsigned max_frames_in_flight = 2);
		~RenderSystem();

		void draw_frame(Transform const& transform);
		void minimized();
		void new_frame();
		void resize_framebuffer();
	private:
		/*
		########  WARNING WARNING WARNING WARNING
		########  DO NOT REORDER THIS SECTION
		########  STRICT ORDERING SECTION BEGIN
		*/
		
		vk::UniqueInstance _instance_u;
		vk::UniqueDevice _device_u;
		vk::UniqueSurfaceKHR _surface_u;
		vk::UniqueSwapchainKHR _swapchain_u;
		std::vector<vk::UniqueImageView> _image_views_u;
		vk::UniqueRenderPass _render_pass_u;
		vk::UniqueDescriptorSetLayout _descriptor_set_layout_u;
		vk::UniquePipelineLayout _pipeline_layout_u;
		vk::UniquePipeline _graphics_pipeline_u;
		vk::UniqueImage _depth_image_u;
		vk::UniqueDeviceMemory _depth_image_memory_u;
		vk::UniqueImageView _depth_image_view_u;
		std::vector<vk::UniqueFramebuffer> _framebuffers_u;
		vk::UniqueCommandPool _command_pool_u;
		std::vector<vk::UniqueCommandBuffer> _command_buffers_u;
		std::vector<vk::UniqueSemaphore> _image_available_sems_u;
		std::vector<vk::UniqueSemaphore> _render_finished_sems_u;
		std::vector<vk::UniqueFence> _in_flight_fences_u;
		vk::UniqueBuffer _staging_buffer_u;
		vk::UniqueDeviceMemory _staging_buffer_memory_u;
		vk::UniqueBuffer _vertex_buffer_u;
		vk::UniqueDeviceMemory _vertex_buffer_memory_u;
		std::vector<vk::UniqueBuffer> _uniform_buffers_u;
		std::vector<vk::UniqueDeviceMemory> _uniform_buffers_memory_u;
		vk::UniqueDescriptorPool _descriptor_pool_main_u;
		vk::UniqueDescriptorPool _descriptor_pool_imgui_u;

		/*
		########  STRICT ORDERING SECTION END
		*/

		CellGeometry& _cell_geometry;
		unsigned _curr_frame;
		std::vector<vk::DescriptorSet> _descriptor_sets;
		bool _framebuffer_resized;
		vk::Queue _graphics_queue;
		std::vector<vk::Image> _images;
		unsigned _max_frames_in_flight;
		vk::SampleCountFlagBits _msaa_samples = vk::SampleCountFlagBits::e1;
		vk::PhysicalDevice _physical_device;
		vk::Queue _present_queue;
		QueueFamilyIndices _queue_indices;
		std::vector<const char*> _required_device_extensions = { vk::KHRSwapchainExtensionName };
		void* _staging_buffer_mapped;
		vk::Extent2D _swapchain_extent;
		vk::Format _swapchain_format;
		std::vector<void*> _uniform_buffers_mapped;
		std::vector<const char*> _validation_layers = { VALIDATION_LAYERS };
		size_t _max_vertex_count;
		size_t _vertex_buffer_size;
		bool _window_minimized;
		WindowSystem& _window_system;

		void create_buffer(
			size_t size,
			vk::BufferUsageFlags usage_flags,
			vk::SharingMode sharing_mode,
			vk::MemoryPropertyFlags memory_property_flags,
			vk::UniqueBuffer& buffer_u,
			vk::UniqueDeviceMemory& buffer_memory_u);

		void create_command_buffer();
		void create_command_pool();
		void create_depth_resources();
		void create_descriptor_pools();
		void create_descriptor_sets();
		void create_descriptor_set_layout();
		void create_framebuffers();
		void create_graphics_pipeline();
		
		void create_image_u(uint32_t width,uint32_t height, vk::Format image_format, vk::ImageTiling tiling_flags,
			vk::ImageUsageFlagBits image_usage_flags, vk::MemoryPropertyFlagBits memory_property_flags,
			vk::UniqueImage& image_u, vk::UniqueDeviceMemory& memory_u);
		
		void create_image_views();
		void create_logical_device();
		void create_swapchain();
		void create_sync_objects();
		void create_uniform_buffers();
		void create_vertex_buffers();
		void create_vk_instance();
		uint32_t find_memory_type(uint32_t type_filter, vk::MemoryPropertyFlags properties);
		void pick_physical_device();
		void record_command_buffer(vk::CommandBuffer& command_buffer, uint32_t image_index);
		void recreate_swapchain();
		void transfer_vertices(const std::vector<Vertex>& vertices);
		void update_uniform_buffer(Transform transform);
	};


	static void copy_buffer(const vk::CommandBuffer& command_buffer, const vk::Buffer& src, const vk::Buffer& dst, vk::DeviceSize size);
	static bool check_device_extension_support(const vk::PhysicalDevice& device, std::vector<const char*> required_device_extensions);
	static bool check_validation_layer_support(const std::vector<const char*>& validation_layers);
	static vk::Extent2D choose_swap_extent(const WindowSystem& windowSystem, const vk::SurfaceCapabilitiesKHR& capabilities);
	static vk::PresentModeKHR choose_swap_present_mode(const std::vector<vk::PresentModeKHR> modes);
	static vk::SurfaceFormatKHR choose_swap_surface_format(const std::vector<vk::SurfaceFormatKHR> formats);
	static vk::UniqueImageView create_image_view(const vk::Device& device, const vk::Image& image, vk::Format format, vk::ImageAspectFlags aspect_mask, uint32_t mip_levels);
	static vk::UniqueRenderPass create_render_pass(const vk::PhysicalDevice& physical_device, const vk::Device& device, const vk::Format& format, vk::SampleCountFlagBits samples);
	static vk::UniqueShaderModule create_shader_module(const vk::Device& device, const std::vector<char>& bytes);
	static vk::Format find_depth_format(const vk::PhysicalDevice& physical_device);
	static QueueFamilyIndices find_queue_families(const vk::PhysicalDevice& device, const vk::SurfaceKHR& surface);
	static vk::SampleCountFlagBits get_max_usable_sample_count(const vk::PhysicalDevice& physical_device);
	static bool is_device_suitable(const vk::PhysicalDevice& device, const vk::SurfaceKHR& surface, const std::vector<const char*>& required_device_extensions);
	static bool is_queue_family_complete(const QueueFamilyIndices& device_indices);
	static std::vector<char> read_shader_bytes(const std::string& file_path);
}