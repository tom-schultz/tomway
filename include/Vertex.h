#pragma once

#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>

namespace hagl {
	struct Vertex
	{
		glm::vec2 pos;
		glm::vec3 color;

		static vk::VertexInputBindingDescription getBindingDescription();
		static std::array<vk::VertexInputAttributeDescription, 2> getAttributeDescriptions();
	};
}
