#include <grid.h>
#include <algorithm>
#include <cmath>

// -----------------------------------------------------------------------------

Grid1D::Grid1D(std::size_t size, Grid_t Min, Grid_t Delta)
    : m_data(size)
{
    if (Delta <= 0) {
        m_data.clear();
    } else if (!m_data.empty()) {
        m_data[0] = Min;
        for (std::size_t i = 1, iE = m_data.size(); i < iE; ++i) {
            m_data[i] = m_data[i - 1] + Delta;
        }
    }
}

// -----------------------------------------------------------------------------

void
Grid1D::save(std::ostream& OS) const
{
    OS << m_data.size();
    for (const auto& value : m_data) {
        OS << " " << value;
    }
}

// -----------------------------------------------------------------------------

std::vector<Grid_t>
Grid1D::computeDiffs() const
{
    std::vector<Grid_t> diffs;
    if (m_data.size() < 2) {
        return diffs;
    }
    
    diffs.reserve(m_data.size() - 1);
    for (std::size_t i = 0; i < m_data.size() - 1; ++i) {
        diffs.push_back(m_data[i + 1] - m_data[i]);
    }
    
    return diffs;
}

// -----------------------------------------------------------------------------

bool
Grid1D::isUniform() const
{
    if (m_data.size() < 2) {
        return true;
    }
    
    Grid_t firstDiff = m_data[1] - m_data[0];
    for (std::size_t i = 1; i < m_data.size() - 1; ++i) {
        if (m_data[i + 1] - m_data[i] != firstDiff) {
            return false;
        }
    }
    
    return true;
}

// -----------------------------------------------------------------------------

bool
Grid1D::addGridPoint(Grid_t value)
{
    if (m_data.empty() || m_data.back() < value) {
        m_data.push_back(value);
        return true;
    }
    else {
        auto it = std::lower_bound(m_data.begin(), m_data.end(), value);
        if (it != m_data.end() && *it == value) {
            return false;
        }
        else {
            m_data.insert(it, value);
            return true;
        }
    }
}

// -----------------------------------------------------------------------------

bool
Grid1D::isSane() const
{
    if (m_data.size() < 2) {
        return true;
    }
    
    for (std::size_t i = 0; i < m_data.size() - 1; ++i) {
        if (m_data[i] >= m_data[i + 1]) {
            return false;
        }
    }
    
    return true;
}

// -----------------------------------------------------------------------------

unsigned
Grid1D::findIndexForClosestGridPoint(float value) const
{
    if (m_data.empty()) {
        return 0;
    }
    
    if (m_data.size() == 1) {
        return 0;
    }
    
    // Use binary search to find the insertion point
    auto it = std::lower_bound(m_data.begin(), m_data.end(), static_cast<Grid_t>(value));
    
    if (it == m_data.end()) {
        // Value is greater than all grid points, return last index
        return static_cast<unsigned>(m_data.size() - 1);
    }
    
    if (it == m_data.begin()) {
        // Value is less than or equal to first grid point
        return 0;
    }
    
    // Compare with current and previous element to find closest
    std::size_t currentIdx = std::distance(m_data.begin(), it);
    std::size_t prevIdx = currentIdx - 1;
    
    float distCurrent = std::abs(static_cast<float>(m_data[currentIdx]) - value);
    float distPrev = std::abs(static_cast<float>(m_data[prevIdx]) - value);
    
    if (distPrev <= distCurrent) {
        return static_cast<unsigned>(prevIdx);
    } else {
        return static_cast<unsigned>(currentIdx);
    }
}

// -----------------------------------------------------------------------------

Grid2D::Grid2D(std::size_t sizeX, Grid_t MinX, Grid_t DeltaX,
               std::size_t sizeY, Grid_t MinY, Grid_t DeltaY)
    : 
    m_Grids(Grid1D(sizeX, MinX, DeltaX), Grid1D(sizeY, MinY, DeltaY))
{
}

// -----------------------------------------------------------------------------

void
Grid2D::save(std::ostream& OS) const
{
    OS << 2 << "\n";
    get<X>().save(OS);
    OS << "\n";
    get<Y>().save(OS);
    OS << "\n";
}

// -----------------------------------------------------------------------------

bool
Grid2D::isSane() const
{
    return get<X>().isSane() && get<Y>().isSane();
}

// -----------------------------------------------------------------------------


