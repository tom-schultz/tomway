#include "simulation/cell_container.h"

#include <random>


tomway::cell_container::iterator::iterator(std::vector<cell>::iterator const& c)
    : std::vector<cell>::iterator(c)
{
    
}

tomway::cell_container::const_iterator::const_iterator(std::vector<cell>::const_iterator const& c)
    : std::vector<cell>::const_iterator(c)
{
    
}

tomway::cell_container::cell_container(size_t grid_size)
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

tomway::cell_container::iterator tomway::cell_container::begin()
{
    return _cells.begin();
}

tomway::cell_container::const_iterator tomway::cell_container::begin() const
{
    return _cells.cbegin();
}

void tomway::cell_container::copy_from(cell_container const& other)
{
    if (other._grid_size != _grid_size)
    {
        throw std::invalid_argument("Cell container grid sizes must match to copy!");
    }

    memcpy(_cells.data(), other._cells.data(), _cells.size() * sizeof(cell));
}

tomway::cell_container::iterator tomway::cell_container::end()
{
    return _cells.end();
}

tomway::cell_container::const_iterator tomway::cell_container::end() const
{
    return _cells.cend();
}

tomway::cell tomway::cell_container::get_cell(size_t x, size_t y) const
{
    return _cells[y * _grid_size + x];
}

bool tomway::cell_container::get_alive(size_t x, size_t y) const
{
    return _cells[y * _grid_size + x].get_alive();
}

void tomway::cell_container::randomize()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::bernoulli_distribution dist(0.25f);
    
    for (auto & cell : _cells)
    {
        cell.set_alive(dist(gen));
    }
}

void tomway::cell_container::set_alive(size_t x, size_t y, bool alive)
{
    _cells[y * _grid_size + x].set_alive(alive);
}

size_t tomway::cell_container::size() const
{
    return _cells.size();
}

size_t tomway::cell_container::grid_size() const
{
    return _grid_size;
}
