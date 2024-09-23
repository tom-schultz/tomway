#include "simulation/CellContainer.h"

#include <random>


tomway::CellContainer::iterator::iterator(std::vector<Cell>::iterator const& c)
    : std::vector<Cell>::iterator(c)
{
    
}

tomway::CellContainer::const_iterator::const_iterator(std::vector<Cell>::const_iterator const& c)
    : std::vector<Cell>::const_iterator(c)
{
    
}

tomway::CellContainer::CellContainer(size_t grid_size)
    : _grid_size(grid_size)
{
    _cells.reserve(grid_size * grid_size);
    
    for (size_t y = 0; y < grid_size; y++)
    {
        for (size_t x = 0; x < grid_size;x++)
        {
            _cells.emplace_back(x, y, false);
        }
    }
}

tomway::CellContainer::iterator tomway::CellContainer::begin()
{
    return _cells.begin();
}

tomway::CellContainer::const_iterator tomway::CellContainer::begin() const
{
    return _cells.cbegin();
}

void tomway::CellContainer::copy_from(CellContainer const& other)
{
    if (other._grid_size != _grid_size)
    {
        throw std::invalid_argument("Cell container grid sizes must match to copy!");
    }

    memcpy(_cells.data(), other._cells.data(), _cells.size() * sizeof(Cell));
}

tomway::CellContainer::iterator tomway::CellContainer::end()
{
    return _cells.end();
}

tomway::CellContainer::const_iterator tomway::CellContainer::end() const
{
    return _cells.cend();
}

tomway::Cell tomway::CellContainer::get_cell(size_t x, size_t y) const
{
    return _cells[y * _grid_size + x];
}

bool tomway::CellContainer::get_alive(size_t x, size_t y) const
{
    return _cells[y * _grid_size + x].get_alive();
}

void tomway::CellContainer::randomize()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::bernoulli_distribution dist(0.25f);
    
    for (auto & cell : _cells)
    {
        cell.set_alive(dist(gen));
    }
}

void tomway::CellContainer::set_alive(size_t x, size_t y, bool alive)
{
    _cells[y * _grid_size + x].set_alive(alive);
}

size_t tomway::CellContainer::size() const
{
    return _cells.size();
}

size_t tomway::CellContainer::grid_size() const
{
    return _grid_size;
}
