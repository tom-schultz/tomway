#include "vertex.h"

std::array<vk::VertexInputAttributeDescription, 3> tomway::vertex::get_attribute_descriptions() {
	std::array<vk::VertexInputAttributeDescription, 3> ret{};

	ret[0] = { 0, 0, vk::Format::eR32G32B32Sfloat, offsetof(vertex, pos) }; // Location, binding, format, offset
	ret[1] = { 1, 0, vk::Format::eR32G32B32Sfloat, offsetof(vertex, normal) }; // Location, binding, format, offset
	ret[2] = { 2, 0, vk::Format::eR32G32B32Sfloat, offsetof(vertex, color) }; // Location, binding, format, offset

	return ret;
}

vk::VertexInputBindingDescription tomway::vertex::get_binding_description() {
	vk::VertexInputBindingDescription constexpr desc(0, sizeof(vertex), vk::VertexInputRate::eVertex);
	return desc;
}