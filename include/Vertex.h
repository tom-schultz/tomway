#pragma once

#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>

namespace tomway {
	struct Vertex
	{
		glm::vec3 pos;
		glm::vec3 normal;
		glm::vec3 color;

		static std::array<vk::VertexInputAttributeDescription, 3> get_attribute_descriptions();
		static vk::VertexInputBindingDescription get_binding_description();
	};
}