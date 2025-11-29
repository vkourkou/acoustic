#ifndef SIMULATOR_H
#define SIMULATOR_H

#include <Grid.h>
#include <InputCompiler.h>
#include <Utilities.h>
#include <DenseMatrix.h>
#include <ostream>

namespace FDTD {

// Forward declaration for CUDA workspace (defined in simulator_kernels.cuh)
struct CudaWorkSpace;

class Simulator {
    struct WorkSpace {
        DenseMatrix<float> m_Pres;
        DenseMatrix<float> m_Vx;
        DenseMatrix<float> m_Vy;
        bool initialize(size_t numRows, size_t numCols);
        void updateVx(float courantNb);
        void updateVy(float courantNb);
        void updatepressure(float crSquareTimesCourantNb);
        void updateFields(float courantNb, float crSquareTimesCourantNb);
        void UpdateForSource(unsigned GridIndexX, unsigned GridIndexY, float val);
    };
public:
    Simulator(const Input::BBoxStatement& Box, const Input::SourceStatement& Source, 
              const Input::VelocityStatement& Velocity, const Input::SimulationParamStatement& SimulationParam, 
              Dimension_t SpatialStep, Time_t TemporalStep, const std::string& dbFolderPath);
    ~Simulator();
    
    void save(std::ostream& OS) const;
    Time_t getTime() const;
    
    bool execute();
private:
    Input::BBoxStatement m_Box;
    Input::SourceStatement m_Source;
    Input::VelocityStatement m_Velocity;
    Input::SimulationParamStatement m_SimulationParam;
    Dimension_t m_SpatialStep;
    Time_t m_TemporalStep;
    long m_GridDimPerStatialStep;
    float m_CourantNb{0.0e0};
    float m_Cr{1.0e0};
    float m_CrSquareTimesCourantNb{1.0e0};
    Grid2D m_Grids;
    WorkSpace m_WorkSpace;
    CudaWorkSpace* m_CudaWorkSpace;  // Only used when PT=GPU, managed in CUDA code
    unsigned m_SourceGridIndex_X{0};       
    unsigned m_SourceGridIndex_Y{0};        //The source is located at the center of the grid point
    size_t m_iteration{0};
    std::string m_dbFolderPath;
    //This function will find how many grid points are for each spatial step.
    long computeGridDimPerStatialStep(float MaxAlloweError) const;

    Grid_t computeDimensionGridSpace(Dimension_t Dimension) const;

    template<ProcessingType PT>
    bool runBatchIterations(size_t numIterations);

    template<ProcessingType PT>
    void updateFields();

    template<ProcessingType PT>
    bool executeForType();

    template<ProcessingType PT>
    bool initializeMatrices();

    template<ProcessingType PT>
    bool runIterations();

    template<ProcessingType PT>
    void updatePressurePointsForSource();

    template<ProcessingType PT>
    void UpdateForSource(unsigned GridIndexX, unsigned GridIndexY, float val);

    bool potentiallySaveTheMatricesToDb();

};

} // namespace FDTD

#endif // SIMULATOR_H

