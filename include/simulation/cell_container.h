#pragma once
#include <vector>
#include "simulation/cell.h"

namespace tomway
{
    class cell_container
    {
    public:        
        class iterator : public std::vector<cell>::iterator {
        public:
            iterator(std::vector<cell>::iterator const& c);
        };
        
        class const_iterator : public std::vector<cell>::const_iterator {
        public:
            const_iterator(std::vector<cell>::const_iterator const& c);
        };

        explicit cell_container(size_t grid_size);
        iterator begin();
        const_iterator begin() const;
        void copy_from(cell_container const& other);
        iterator end();
        const_iterator end() const;
        cell get_cell(size_t x, size_t y) const;
        bool get_alive(size_t x, size_t y) const;
		void randomize();
        void set_alive(size_t x, size_t y, bool alive);
        size_t size() const;
        size_t grid_size() const;
    private:
        std::vector<cell> _cells;
        size_t _grid_size;
    };
}
