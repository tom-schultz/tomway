#pragma once
#include "rapidjson/document.h"

namespace tomway {
	class cell_geometry;

	class cell
	{
	public:
		cell(size_t x, size_t y, bool alive);
		size_t get_x() const;
		size_t get_y() const;
		bool get_alive() const;
		void serialize(rapidjson::Document& doc, rapidjson::Value& cell_array) const;
		void set_alive(bool alive);
	private:
		size_t _x, _y;
		bool _alive;
        friend cell_geometry;
	};
}
