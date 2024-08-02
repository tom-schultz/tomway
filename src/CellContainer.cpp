#include "CellContainer.h"

#include <random>


tomway::CellContainer::const_iterator::const_iterator(std::vector<Cell>::const_iterator const& c)
    : std::vector<Cell>::const_iterator(c)
{
    
}

tomway::CellContainer::CellContainer(size_t grid_size)
    : _grid_size(grid_size)
{
    _cells.reserve(grid_size * grid_size);
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::bernoulli_distribution dist;
    
    for (size_t x = 0; x < grid_size; x++)
    {
        for (size_t y = 0; y < grid_size; y++)
        _cells.emplace_back(x, y, dist(gen));
    }
}

tomway::CellContainer::const_iterator tomway::CellContainer::begin() const
{
    return _cells.begin();
}

tomway::CellContainer::const_iterator tomway::CellContainer::end() const
{
    return _cells.end();
}

tomway::Cell tomway::CellContainer::get_cell(int x, int y) const
{
    return _cells[y * _grid_size + x];
}

size_t tomway::CellContainer::size() const
{
    return _cells.size();
}

size_t tomway::CellContainer::grid_size() const
{
    return _grid_size;
}
