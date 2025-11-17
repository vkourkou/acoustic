#ifndef GRID_H
#define GRID_H

#include <vector>
#include <cstddef>
#include <cassert>
#include <tuple>
#include <ostream>
#include <Utilities.h>

using Grid_t = int;

class Grid1D {
private:
    std::vector<Grid_t> m_data;

public:
    // Constructor with size, Min, and Delta
    Grid1D(std::size_t size, Grid_t Min, Grid_t Delta);

    std::size_t size() const {
        return m_data.size();
    }

    const Grid_t& operator[](std::size_t index) const {
        assert(index < m_data.size());;
        return m_data[index];
    }

    bool empty() const {
        return m_data.empty();
    }

    // Save grid to output stream
    void save(std::ostream& OS) const;

    // Compute differences between consecutive elements
    std::vector<Grid_t> computeDiffs() const;

    // Check if all spacing between grid points are the same
    bool isUniform() const;

    bool isSane() const;

    bool addGridPoint(Grid_t value);
    
};

// -----------------------------------------------------------------------------

class Grid2D {
private:
    std::tuple<Grid1D, Grid1D> m_Grids;

public:
    // Constructor with sizeX, MinX, DeltaX, sizeY, MinY, DeltaY
    Grid2D(std::size_t sizeX, Grid_t MinX, Grid_t DeltaX,
           std::size_t sizeY, Grid_t MinY, Grid_t DeltaY);


    // Get grid for specified direction
    template<Direction Dir>
    constexpr const Grid1D& get() const;

    // -----------------------------------------------------------------------------

    // Get size for specified direction
    template<Direction Dir>
    constexpr std::size_t size() const;

    // -----------------------------------------------------------------------------

    // Save grid to output stream
    void save(std::ostream& OS) const;

};

// -----------------------------------------------------------------------------

// Template implementation for get<Direction>
template<Direction Dir>
inline constexpr const Grid1D&
Grid2D::get() const
{
    return std::get<static_cast<std::size_t>(Dir)>(m_Grids);
}

// -----------------------------------------------------------------------------

// Template implementation for size<Direction>
template<Direction Dir>
inline constexpr std::size_t
Grid2D::size() const
{
    return get<Dir>().size();
}

// -----------------------------------------------------------------------------

#endif // GRID_H

