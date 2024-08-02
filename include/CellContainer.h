#pragma once
#include <vector>
#include "Cell.h"

namespace tomway
{
    class CellContainer
    {
    public:        
        class const_iterator : public std::vector<Cell>::const_iterator {
        public:
            const_iterator(std::vector<Cell>::const_iterator const& c);
        };

        CellContainer(size_t grid_size);
        const_iterator begin() const;
        const_iterator end() const;
        Cell get_cell(int x, int y) const;
        size_t size() const;
        size_t grid_size() const;
    private:
        std::vector<Cell> _cells;
        size_t _grid_size;
    };
}
