#include <Simulator.h>
#include <cmath>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>
#include <algorithm>

// Forward declaration for cleanup function (defined in simulator_kernels.cu)
namespace FDTD {
    struct CudaWorkSpace;
    void cleanupCudaWorkSpace(CudaWorkSpace* ws);
}

// -----------------------------------------------------------------------------
namespace FDTD {

// -----------------------------------------------------------------------------

bool
Simulator::WorkSpace::initialize(size_t numRows, size_t numCols)
{
    m_Pres.resize(numRows, numCols, 0.0e0);
    m_Vx.resize(numRows - 1, numCols, 0.0e0);
    m_Vy.resize(numRows, numCols - 1, 0.0e0);
    return true;
}

// -----------------------------------------------------------------------------

void
Simulator::WorkSpace::updateVx(float courantNb)
{
    for (std::size_t i = 0, iE = m_Vx.rows(); i < iE; ++i) {
        for (std::size_t j = 0, jE = m_Vx.cols(); j < jE; ++j) {
            m_Vx(i,j) = m_Vx(i,j) - courantNb * (m_Pres(i+1,j) - m_Pres(i,j));
        }
    }
}

// -----------------------------------------------------------------------------

void
Simulator::WorkSpace::updateVy(float courantNb)
{
    for (std::size_t i = 0, iE = m_Vy.rows(); i < iE; ++i) {
        for (std::size_t j = 0, jE = m_Vy.cols(); j < jE; ++j) {
            m_Vy(i,j) = m_Vy(i,j) - courantNb * (m_Pres(i,j+1) - m_Pres(i,j));
        }
    }
}

// -----------------------------------------------------------------------------

void
Simulator::WorkSpace::updatepressure(float crSquareTimesCourantNb)
{
    //Boundaries are assume to have directlet condition
    for (std::size_t i = 1, iE = m_Pres.rows() - 1; i < iE; ++i) {
        for (std::size_t j = 1, jE = m_Pres.cols() - 1; j < jE; ++j) {
            m_Pres(i,j) = m_Pres(i,j) - crSquareTimesCourantNb * (m_Vx(i,j) - m_Vx(i - 1,j) + m_Vy(i,j) - m_Vy(i,j - 1));
        }
    }
}

// -----------------------------------------------------------------------------

void
Simulator::WorkSpace::updateFields(float courantNb, float crSquareTimesCourantNb)
{
    updateVx(courantNb);
    updateVy(courantNb);
    updatepressure(crSquareTimesCourantNb);
}

// -----------------------------------------------------------------------------

void
Simulator::WorkSpace::UpdateForSource(unsigned GridIndexX, unsigned GridIndexY, float val)
{
    m_Pres(GridIndexX, GridIndexY) = val;
}

// -----------------------------------------------------------------------------

Simulator::Simulator(const Input::BBoxStatement& Box, const Input::SourceStatement& Source,
                     const Input::VelocityStatement& Velocity, const Input::SimulationParamStatement& SimulationParam, 
                     Dimension_t SpatialStep, Time_t TemporalStep, const std::string& dbFolderPath,
                     const std::vector<Input::WallStatement>* walls)
    :
    m_Box(Box),
    m_Source(Source),
    m_Velocity(Velocity),
    m_SimulationParam(SimulationParam),
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
    m_CudaWorkSpace(nullptr),
    m_walls(walls),
    m_dbFolderPath(dbFolderPath)    
{
    m_SourceGridIndex_X = m_Grids.get<X>().findIndexForClosestGridPoint(static_cast<float>(computeDimensionGridSpace(m_Source.getX())));
    m_SourceGridIndex_Y = m_Grids.get<Y>().findIndexForClosestGridPoint(static_cast<float>(computeDimensionGridSpace(m_Source.getY())));
}

// -----------------------------------------------------------------------------

Simulator::~Simulator()
{
    // Cleanup CUDA workspace if it exists
    // The actual cleanup function is defined in simulator_kernels.cu
    if (m_CudaWorkSpace) {
        FDTD::cleanupCudaWorkSpace(m_CudaWorkSpace);
        m_CudaWorkSpace = nullptr;
    }
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

template<>
bool
Simulator::initializeMatrices<CPU>()
{
    std::size_t numRows = m_Grids.get<X>().size();
    std::size_t numCols = m_Grids.get<Y>().size();
    
    return m_WorkSpace.initialize(numRows, numCols);
}

// -----------------------------------------------------------------------------

size_t
calculateNumIterationsForBatch(const Input::SimulationParamStatement& simulationParam, size_t currentIteration)
{
    return std::min(simulationParam.getBatchSize(), simulationParam.getMaxIteration() - currentIteration);
}

// -----------------------------------------------------------------------------

template<>
void
Simulator::updateFields<CPU>()
{
    m_WorkSpace.updateFields(m_CourantNb, m_CrSquareTimesCourantNb);
}

// -----------------------------------------------------------------------------

template<ProcessingType PT>
bool
Simulator::runBatchIterations(size_t numIterations)
{
    // Dummy implementation
    try {
        for (size_t i = 0; i < numIterations; ++i, ++m_iteration) {
            updateFields<PT>();
            updatePressurePointsForSource<PT>();
        }
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Exception in runBatchIterations: " << e.what() << std::endl;
        return false;
    }
}

// -----------------------------------------------------------------------------

template<ProcessingType PT>
bool
Simulator::runIterations()
{
    size_t numIterationsForThisBatch = calculateNumIterationsForBatch(m_SimulationParam, m_iteration);
    while (numIterationsForThisBatch > 0) {
        
        if (!runBatchIterations<PT>(numIterationsForThisBatch)) {
            std::cout << "Error running batch of iterations. Failed at iteration " << m_iteration << std::endl;
            return false;
        }
        if (!potentiallySaveTheMatricesToDb<PT>()) {
            std::cout << "Error saving matrices to database" << std::endl;
            return false;
        }

        numIterationsForThisBatch = calculateNumIterationsForBatch(m_SimulationParam, m_iteration);
    }
    
    return true;
}

// -----------------------------------------------------------------------------

template<ProcessingType PT>
bool
Simulator::executeForType()
{
    if (!initializeMatrices<PT>()) {
        std::cout << "Error initializing matrices" << std::endl;
        return false;
    }
    if (!runIterations<PT>()) {
        std::cout << "Error running iterations" << std::endl;
        return false;
    }
    return true;
}

// -----------------------------------------------------------------------------

bool
Simulator::execute()
{
    ProcessingType PT = m_SimulationParam.getProcessingType();
    switch (PT) {
        case CPU:
            return executeForType<CPU>();
        case GPU:
            return executeForType<GPU>();
        default:
            return executeForType<CPU>();
    }
}

// -----------------------------------------------------------------------------

template<>
void
Simulator::UpdateForSource<CPU>(unsigned GridIndexX, unsigned GridIndexY, float val) {
    m_WorkSpace.UpdateForSource(GridIndexX, GridIndexY, val);
}

// -----------------------------------------------------------------------------

template<ProcessingType PT>
void
Simulator::updatePressurePointsForSource()
{
    Time_t time = getTime();
    if (time <= m_Source.getDuration()) {
        //If the source stops being applied we would like the point to become "free"
        float val = m_Source.getValue(getTime());
        UpdateForSource<PT>(m_SourceGridIndex_X, m_SourceGridIndex_Y, val);
    }
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

template<ProcessingType PT>
bool
Simulator::potentiallySaveTheMatricesToDb()
{
    // Dummy implementation
    if (!m_dbFolderPath.empty()) {
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
        if constexpr (PT == CPU) {
            m_WorkSpace.m_Pres.save(file, /*bPrintTranspose*/ true);
        } else {
            DenseMatrix<float> To;
            potentiallyTransferToDevice(To);
            To.save(file, /*bPrintTranspose*/ true);
        }
        file.close();
    }
    return true;
}

// -----------------------------------------------------------------------------

}