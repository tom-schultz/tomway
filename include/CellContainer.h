#pragma once
#include <vector>
#include "Cell.h"

namespace tomway
{
    class CellContainer
    {
    public:        
        class iterator : public std::vector<Cell>::iterator {
        public:
            iterator(std::vector<Cell>::iterator const& c);
        };
        
        class const_iterator : public std::vector<Cell>::const_iterator {
        public:
            const_iterator(std::vector<Cell>::const_iterator const& c);
        };

        CellContainer(size_t grid_size);
        iterator begin();
        const_iterator begin() const;
        void copy_from(CellContainer const& other);
        iterator end();
        const_iterator end() const;
        Cell get_cell(size_t x, size_t y) const;
        bool get_alive(size_t x, size_t y) const;
        void set_alive(size_t x, size_t y, bool alive);
        size_t size() const;
        size_t grid_size() const;
    private:
        std::vector<Cell> _cells;
        size_t _grid_size;
    };
}
