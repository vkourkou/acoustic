#include <Simulator.h>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <limits>
#include <stdexcept>

// -----------------------------------------------------------------------------
namespace FDTD {

constexpr float pi_float = 3.14159265358979323846f;

Simulator::Simulator(const Input::BBoxStatement& Box, const Input::SourceStatement& Source, const Input::VelocityStatement& Velocity, Dimension_t SpatialStep, Time_t TemporalStep)
    :
    m_Box(Box),
    m_Source(Source),
    m_Velocity(Velocity),
    m_SpatialStep(SpatialStep),
    m_TemporalStep(TemporalStep),
    m_GridDimPerStatialStep(computeGridDimPerStatialStep(/*MaxAlloweError*/0.0001f)),
    m_CourantNb(Velocity.getVelocity() * TemporalStep / SpatialStep),
    m_Cr(1.0e0),
    m_CrSquareTimesCourantNb(m_Cr * m_Cr* m_CourantNb),
    m_Grids(
        static_cast<std::size_t>(std::ceil((Box.getXMax() - Box.getXMin()) / SpatialStep) + 1),
        static_cast<Grid_t>(computeDimensionGridSpace(Box.getXMin())),
        static_cast<Grid_t>(m_GridDimPerStatialStep),
        static_cast<std::size_t>(std::ceil((Box.getYMax() - Box.getYMin()) / SpatialStep) + 1),
        static_cast<Grid_t>(computeDimensionGridSpace(Box.getYMin())),
        static_cast<Grid_t>(m_GridDimPerStatialStep)
    )
{
    m_SourceGridIndex_X = m_Grids.get<X>().findIndexForClosestGridPoint(m_Source.getX());
    m_SourceGridIndex_Y = m_Grids.get<Y>().findIndexForClosestGridPoint(m_Source.getY());
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
computeDimensionsToBeAccommodated(const Input::BBoxStatement& Box, const Input::SourceStatement& Source)
{
    std::vector<Dimension_t> DimenstionsToBeAccomodated;
    DimenstionsToBeAccomodated.push_back(Box.getXMax());
    DimenstionsToBeAccomodated.push_back(Box.getYMax());
    DimenstionsToBeAccomodated.push_back(Box.getYMin());
    DimenstionsToBeAccomodated.push_back(Box.getXMin());
    DimenstionsToBeAccomodated.push_back(Source.getX());
    DimenstionsToBeAccomodated.push_back(Source.getY());
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

    const std::vector<Dimension_t> DimenstionsToBeAccomodated{computeDimensionsToBeAccommodated(m_Box, m_Source)};

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
    OS << "Courant number: " << m_CourantNb << "\n";
    OS << "Grids: " << "\n";
    m_Grids.save(OS);
}

// -----------------------------------------------------------------------------

bool
Simulator::initializeMatrices()
{
    std::size_t numRows = m_Grids.get<X>().size();
    std::size_t numCols = m_Grids.get<Y>().size();
    
    m_Pres.resize(numRows, numCols, 0.0e0);
    m_Vx.resize(numRows - 1, numCols - 1, 0.0e0);
    m_Vy.resize(numRows - 1, numCols - 1, 0.0e0);
    return true;
}

// -----------------------------------------------------------------------------

bool
Simulator::runIterations(unsigned numIterations)
{
    for (unsigned i = 0; i < numIterations; ++i) {
        if (!runIteration()) {
            std::cout << "Error running iteration " << i << std::endl;
            return false;
        }
        ++m_iteration;
    }
    return true;
}

// -----------------------------------------------------------------------------

bool
Simulator::runIteration()
{
    // Dummy implementation
    //Update Vx
    for (std::size_t i = 0, iE = m_Grids.get<X>().size() - 1; i < iE; ++i) {
        for (std::size_t j = 0, jE = m_Grids.get<Y>().size(); j < jE; ++j) {
            m_Vx(i,j) = m_Vx(i,j) + m_CourantNb * (m_Pres(i+1,j) - m_Pres(i,j));
        }
    }
    //Update Vy
    for (std::size_t i = 0, iE = m_Grids.get<X>().size(); i < iE; ++i) {
        for (std::size_t j = 0, jE = m_Grids.get<Y>().size() - 1; j < jE; ++j) {
            m_Vy(i,j) = m_Vy(i,j) + m_CourantNb * (m_Pres(i,j+1) - m_Pres(i,j));
        }
    }
    //Update Pres
    //Boundaries are assume to have directlet condition
    for (std::size_t i = 1, iE = m_Grids.get<X>().size() - 1; i < iE; ++i) {
        for (std::size_t j = 1, jE = m_Grids.get<Y>().size() - 1; j < jE; ++j) {
            m_Pres(i,j) = m_Pres(i,j) + m_CrSquareTimesCourantNb * (m_Vx(i+1,j) - m_Vx(i,j) + m_Vy(i,j+1) - m_Vy(i,j));
        }
    }

    m_Pres(m_SourceGridIndex_X, m_SourceGridIndex_X) = 
    m_Source.getAmplitude() * std::sin(2.0e0f * pi_float * m_Source.getFreq() * m_iteration * m_TemporalStep);

    return true;
}

// -----------------------------------------------------------------------------

}