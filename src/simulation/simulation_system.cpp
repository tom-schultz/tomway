#include "simulation/simulation_system.h"
#include "tomway_utility.h"
#include "Tracy.hpp"
#include "ui_system.h"
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/stringbuffer.h"

tomway::simulation_system::simulation_system()
    : _grid_size(0),
    _cells{ cell_container(0), cell_container(0) }
{
}

bool tomway::simulation_system::deserialize(std::string const& json)
{
    ZoneScoped;
    rapidjson::Document document;
    document.Parse(json.c_str());

    if (not document.HasMember("cells") or not document["cells"].IsArray())
    {
        LOG_ERROR("Malformed save file: document[\"cells\"] not present or not array.");
        return false;
    }
    
    auto const& json_data = document["cells"].GetArray();

    if (not document.HasMember("grid_size") or not document["grid_size"].IsUint64())
    {
        LOG_ERROR("Malformed save file: document[\"grid_size\"] not present or not uint64.");
        return false;
    }
    
    auto grid_size = document["grid_size"].GetUint64();

    if (json_data.Size() > grid_size * grid_size)
    {
        LOG_ERROR("Malformed save file: grid_size * grid_size is less than cell count.");
        return false;
    }

    // Validation pass
    for (rapidjson::SizeType i = 0; i < json_data.Size(); i++)
    {
        if (not json_data[i].IsArray())
        {
            LOG_ERROR("Cell at position %d is not an array.", i);
            return false;
        }
        
        auto const& json_cell = json_data[i].GetArray();
        
        if (json_cell.Size() != 2 or not json_cell[0].IsUint64() or not json_cell[1].IsUint64())
        {
            LOG_ERROR("Cell at position %d does not have two uint64 members.", i);
            return false;
        }
        
    }
    
    _grid_size = grid_size;
    _index = 0;
    _cells[0] = { _grid_size };
    _cells[1] = { _grid_size };
    
    for (rapidjson::SizeType i = 0; i < json_data.Size(); i++)
    {
        auto const& json_cell = json_data[i].GetArray();
        _cells[0].set_alive(json_cell[0].GetUint64(), json_cell[1].GetUint64(), true);
    }

    return true;
}

size_t tomway::simulation_system::get_cell_count() const
{
    return _cells[_index].size();
}

tomway::cell_container const* tomway::simulation_system::get_current_cells() const
{
    ZoneScoped;
    return &_cells[_index];
}

void tomway::simulation_system::new_frame()
{
    if (_grid_size > 0)
    {
        auto grid_string = string_format("Grid size: %zu x %zu", _grid_size, _grid_size);
        ui_system::add_debug_text(grid_string);
    }
}

std::string tomway::simulation_system::serialize() const
{
    ZoneScoped;
    rapidjson::Document document;
    rapidjson::Value cell_array;
    cell_array.SetArray();
    
    for (auto const& cell : _cells[_index])
    {
        cell.serialize(document, cell_array);
    }

    document.SetObject();
    document.AddMember("cells", cell_array, document.GetAllocator());
    document.AddMember("grid_size", _grid_size, document.GetAllocator());
    rapidjson::StringBuffer sb;
    rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
    document.Accept(writer);    // Accept() traverses the DOM and generates Handler events.
    return sb.GetString();
}

void tomway::simulation_system::start(size_t const grid_size)
{
    ZoneScoped;
    _grid_size = grid_size;
    _cells[0] = { _grid_size };
    _cells[0].randomize();
    _cells[1] = { _grid_size };
    _cells[1].randomize();
}

void tomway::simulation_system::step_simulation()
{
    ZoneScoped;
    unsigned int const new_index = (_index + 1) % 2;
    
    for (cell& cell : _cells[_index])
    {
        size_t const x = cell.get_x();
        size_t const y = cell.get_y();
        
        size_t const xp1 = wrap(x + 1);
        size_t const xm1 = wrap(x - 1);
        size_t const yp1 = wrap(y + 1);
        size_t const ym1 = wrap(y - 1);

        int const neighbors_alive = _cells[_index].get_alive(xm1, y) // Left
             + _cells[_index].get_alive(xm1, ym1) // Upper Left
             + _cells[_index].get_alive(x, ym1) // Up
             + _cells[_index].get_alive(xp1, ym1) // Upper Right
             + _cells[_index].get_alive(xp1, y) // Right
             + _cells[_index].get_alive(xp1, yp1) // Lower Right
             + _cells[_index].get_alive(x, yp1) // Down
             + _cells[_index].get_alive(xm1, yp1); // Lower Left

        bool const alive = (cell.get_alive() && neighbors_alive == 2) || neighbors_alive == 3;
        _cells[new_index].set_alive(x, y, alive);
    }

    _index = new_index;
}

inline size_t tomway::simulation_system::wrap(long long int val) const
{
    if (val < 0)
    {
        return _grid_size - 1;
    }
    
    return static_cast<size_t>(val) > _grid_size - 1 ? 0 : val;
}
