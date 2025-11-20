#include <Simulator.h>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <limits>
#include <stdexcept>

// -----------------------------------------------------------------------------
namespace FDTD {

Simulator::Simulator(const Input::BBoxStatement& Box, const Input::SourceStatement& Source, const Input::VelocityStatement& Velocity, Dimension_t SpatialStep, Time_t TemporalStep)
    :
    m_Box(Box),
    m_Source(Source),
    m_Velocity(Velocity),
    m_SpatialStep(SpatialStep),
    m_TemporalStep(TemporalStep),
    m_GridDimPerStatialStep(computeGridDimPerStatialStep(/*MaxAlloweError*/0.0001f)),
    m_Grids(
        static_cast<std::size_t>(std::ceil((Box.getXMax() - Box.getXMin()) / SpatialStep) + 1),
        static_cast<Grid_t>(computeDimensionGridSpace(Box.getXMin())),
        static_cast<Grid_t>(m_GridDimPerStatialStep),
        static_cast<std::size_t>(std::ceil((Box.getYMax() - Box.getYMin()) / SpatialStep) + 1),
        static_cast<Grid_t>(computeDimensionGridSpace(Box.getYMin())),
        static_cast<Grid_t>(m_GridDimPerStatialStep)
    )
    //FIXME: The above has to become more robust
{
}

// -----------------------------------------------------------------------------

float
computeErrorByRounding(long GridDimPerStatialStep, Dimension_t SpatialStep, Dimension_t Dimension)
{
    Dimension_t GridDim = Dimension * GridDimPerStatialStep / SpatialStep;
    if (GridDim == 0) {
        return 0.0f;
    }
    return abs((GridDim - round(GridDim)) / GridDim);
}

// -----------------------------------------------------------------------------

std::vector<Dimension_t>
computeDimensionsToBeAccommodated(const Input::BBoxStatement& Box)
{
    std::vector<Dimension_t> DimenstionsToBeAccomodated;
    DimenstionsToBeAccomodated.push_back(Box.getXMax() - Box.getXMin());
    DimenstionsToBeAccomodated.push_back(Box.getYMax() - Box.getYMin());
    DimenstionsToBeAccomodated.push_back(-Box.getYMin());
    DimenstionsToBeAccomodated.push_back(-Box.getXMin());
    return DimenstionsToBeAccomodated;
}

// -----------------------------------------------------------------------------

Grid_t 
Simulator::computeDimensionGridSpace(Dimension_t Dimension) const
{
    return static_cast<Grid_t>(std::round(Dimension * m_GridDimPerStatialStep / m_SpatialStep));

}

// -----------------------------------------------------------------------------


long
Simulator::computeGridDimPerStatialStep(float MaxAlloweError) const
{

    const std::vector<Dimension_t> DimenstionsToBeAccomodated{computeDimensionsToBeAccommodated(m_Box)};

    long DimPerStatialStep = 1;
    for (const auto& Dimension : DimenstionsToBeAccomodated) {
        DimPerStatialStep = std::max(Utilities::computeMultiplierToReduceTruncationError(Dimension/m_SpatialStep, MaxAlloweError), DimPerStatialStep);
    }

    try {
        for (const auto& Dimension : DimenstionsToBeAccomodated) {
            double Y = Dimension * DimPerStatialStep/m_SpatialStep;
            double roundedY = std::round(Y);
            if (roundedY < static_cast<double>(std::numeric_limits<Grid_t>::min()) || 
                roundedY > static_cast<double>(std::numeric_limits<Grid_t>::max())) {
                throw std::overflow_error("Y value out of Grid_t range after rounding");
            }
        }
    } catch (const std::exception& e) {
        (void)e;
        throw;
    }

    return DimPerStatialStep;
}

// -----------------------------------------------------------------------------



void
Simulator::save(std::ostream& OS) const
{
    m_Box.save(OS);
    OS << "\n";
    m_Source.save(OS);
    OS << "\n";
    m_Velocity.save(OS);
    OS << "\n";
    OS << "SpatialStep " << m_SpatialStep << "\n";
    OS << "TemporalStep " << m_TemporalStep << "\n";
    OS << "Grid Dime per statial step " << m_GridDimPerStatialStep << "\n";
    OS << "BOX (real coordinates): " << m_Box.getXMin() << " " << m_Box.getXMax() << " " << m_Box.getYMin() << " " << m_Box.getYMax() << "\n";
    OS << "BOX (grid coordinates): " << computeDimensionGridSpace(m_Box.getXMin()) << " " << computeDimensionGridSpace(m_Box.getXMax()) << " " << computeDimensionGridSpace(m_Box.getYMin()) << " " << computeDimensionGridSpace(m_Box.getYMax()) << "\n"; 
    OS << "Grid points (start/end): " << m_Grids.get<X>().front() << " " << m_Grids.get<X>().back() << " " << m_Grids.get<Y>().front() << " " << m_Grids.get<Y>().back() << "\n";
    OS << "Grids: " << "\n";
    m_Grids.save(OS);
}

// -----------------------------------------------------------------------------

}