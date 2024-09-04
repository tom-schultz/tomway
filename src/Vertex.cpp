#include "Vertex.h"

std::array<vk::VertexInputAttributeDescription, 3> tomway::Vertex::get_attribute_descriptions() {
	std::array<vk::VertexInputAttributeDescription, 3> ret{};

	ret[0] = { 0, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, pos) }; // Location, binding, format, offset
	ret[1] = { 1, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, normal) }; // Location, binding, format, offset
	ret[2] = { 2, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, color) }; // Location, binding, format, offset

	return ret;
}

vk::VertexInputBindingDescription tomway::Vertex::get_binding_description() {
	vk::VertexInputBindingDescription constexpr desc(0, sizeof(Vertex), vk::VertexInputRate::eVertex);
	return desc;
}