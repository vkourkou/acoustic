#include <Simulator.h>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>

// -----------------------------------------------------------------------------
namespace FDTD {

Simulator::Simulator(const Input::BBoxStatement& Box, const Input::SourceStatement& Source,
                     const Input::VelocityStatement& Velocity, Dimension_t SpatialStep,
                     Time_t TemporalStep, const std::string& dbFolderPath)
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
    ),
    m_dbFolderPath(dbFolderPath)    
{
    m_SourceGridIndex_X = m_Grids.get<X>().findIndexForClosestGridPoint(static_cast<float>(computeDimensionGridSpace(m_Source.getX())));
    m_SourceGridIndex_Y = m_Grids.get<Y>().findIndexForClosestGridPoint(static_cast<float>(computeDimensionGridSpace(m_Source.getY())));
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

Time_t
Simulator::getTime() const
{
    return m_TemporalStep * m_iteration;
}

// -----------------------------------------------------------------------------

bool
Simulator::initializeMatrices()
{
    std::size_t numRows = m_Grids.get<X>().size();
    std::size_t numCols = m_Grids.get<Y>().size();
    
    m_Pres.resize(numRows, numCols, 0.0e0);
    m_Vx.resize(numRows - 1, numCols , 0.0e0);
    m_Vy.resize(numRows, numCols - 1, 0.0e0);
    return true;
}

// -----------------------------------------------------------------------------

bool
Simulator::runIterations(size_t numIterations)
{
    for (size_t i = 0; i < numIterations; ++i) {
        if (!runIteration()) {
            std::cout << "Error running iteration " << m_iteration << std::endl;
            return false;
        }        
    }
    return true;
}

// -----------------------------------------------------------------------------

bool
Simulator::runIteration()
{
    // Dummy implementation
    try {
        //Update Vx
        for (std::size_t i = 0, iE = m_Vx.rows(); i < iE; ++i) {
            for (std::size_t j = 0, jE = m_Vx.cols(); j < jE; ++j) {
                m_Vx(i,j) = m_Vx(i,j) - m_CourantNb * (m_Pres(i+1,j) - m_Pres(i,j));
            }
        }
        //Update Vy
        for (std::size_t i = 0, iE = m_Vy.rows(); i < iE; ++i) {
            for (std::size_t j = 0, jE = m_Vy.cols(); j < jE; ++j) {
                m_Vy(i,j) = m_Vy(i,j) - m_CourantNb * (m_Pres(i,j+1) - m_Pres(i,j));
            }
        }
        //Update Pres
        //Boundaries are assume to have directlet condition
        for (std::size_t i = 1, iE = m_Pres.rows() - 1; i < iE; ++i) {
            for (std::size_t j = 1, jE = m_Pres.cols() - 1; j < jE; ++j) {
                m_Pres(i,j) = m_Pres(i,j) - m_CrSquareTimesCourantNb * (m_Vx(i,j) - m_Vx(i - 1,j) + m_Vy(i,j) - m_Vy(i,j - 1));
            }
        }
        m_Pres(m_SourceGridIndex_X, m_SourceGridIndex_Y) = m_Source.getValue(getTime());
    
        
    } catch (const std::exception& e) {
        std::cerr << "Exception in runIteration: " << e.what() << std::endl;
        return false;
    }


    if (!potentiallySaveTheMatricesToDb()) {
        std::cout << "Error saving matrices to database" << std::endl;
        return false;
    }

    ++m_iteration;
    
    return true;
}

// -----------------------------------------------------------------------------

void
saveGrid(const Grid1D& grid, float multiplier, std::ostream& OS)
{
    if (grid.empty()) {
        return;
    }
    OS << multiplier * grid.front();
    for (std::size_t i = 1; i < grid.size(); ++i) {
        OS << " " << multiplier * grid[i];
    }
    OS << std::endl;
}

//-----------------------------------------------------------------------------

bool
Simulator::potentiallySaveTheMatricesToDb()
{
    // Dummy implementation
    if (!m_dbFolderPath.empty() && m_iteration % 10 == 0) {
        std::string filename = m_dbFolderPath + "/iteration_" + std::to_string(m_iteration) + ".db";
        std::ofstream file(filename.c_str());
        if (!file.is_open()) {
            std::cout << "Error opening file " << filename << std::endl;
            return false;
        }
        file << "Time: " << m_iteration * m_TemporalStep << std::endl;
        float multiplier = m_SpatialStep / m_GridDimPerStatialStep;
        saveGrid(m_Grids.get<X>(), multiplier, file);
        saveGrid(m_Grids.get<Y>(), multiplier, file);
        m_Pres.save(file, /*bPrintTranspose*/ true);
        file.close();
    }
    return true;
}

// -----------------------------------------------------------------------------

}