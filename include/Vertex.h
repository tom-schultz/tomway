#pragma once

#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>

namespace tomway {
	struct Vertex
	{
		glm::vec2 pos;
		glm::vec3 color;

		static std::array<vk::VertexInputAttributeDescription, 2> get_attribute_descriptions();
		static vk::VertexInputBindingDescription get_binding_description();
	};
}