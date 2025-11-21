#include <Runner.h>
#include <Simulator.h>
#include <iostream>
#include <cassert>
#include <filesystem>

// -----------------------------------------------------------------------------

bool
Runner::parseInput(std::istream& IS) {
    Input::FileParser fileParser(IS);
    for (Input::FileParser::const_iterator iter(fileParser); iter.isValid(); ++iter) {
        Input::StatementType type = iter.getType();
        
        if (type == Input::StatementType::SOURCE) {
            const Input::SourceStatement& source = iter.getStatement<Input::StatementType::SOURCE>();
            if (!m_InputCnt.set(source)) {
                return false;
            }
        }
        else if (type == Input::StatementType::BBOX) {
            const Input::BBoxStatement& bbox = iter.getStatement<Input::StatementType::BBOX>();
            if (!m_InputCnt.set(bbox)) {
                return false;
            }
        }
        else if (type == Input::StatementType::VELOCITY) {
            const Input::VelocityStatement& velocity = iter.getStatement<Input::StatementType::VELOCITY>();
            if (!m_InputCnt.set(velocity)) {
                return false;
            }
        }
        else if (type == Input::StatementType::MAXRESOLUTION) {
            const Input::MaxResolutionStatement& maxRes = iter.getStatement<Input::StatementType::MAXRESOLUTION>();
            if (!m_InputCnt.set(maxRes)) {
                return false;
            }
        }
        else if (type == Input::StatementType::COMMENT) {
            // Comments are ignored, continue processing
        }
        else if (type == Input::StatementType::MAX) {
            std::cerr << "Error in line " << iter.getLineNumber() << std::endl;
            return false;
        }
        else {
            assert(false);
            return false;
        }
    }
    return true;
}

// -----------------------------------------------------------------------------

bool
Runner::execute()
{
    FDTD::Simulator simulator(m_InputCnt.get<Input::BBoxStatement>(), m_InputCnt.get<Input::SourceStatement>(), m_InputCnt.get<Input::VelocityStatement>(), m_InputCnt.computeSpatialStep(), m_InputCnt.computeTimeStep());
    simulator.save(std::cout);
    if (!simulator.initializeMatrices()) {
        std::cout << "Error initializing matrices" << std::endl;
        return false;
    }
    if (!simulator.runIterations(m_TotalIterations)) {
        std::cout << "Error running iterations" << std::endl;
        return false;
    }
    return true;
}

// -----------------------------------------------------------------------------

bool
Runner::run(std::istream& IS) {
    if (!parseInput(IS)) {
        return false;
    }
    m_InputCnt.save(std::cout);

    if (!m_InputCnt.isSane()) {
        std::cout << "Input is not sane\n";
        return false;
    }


    std::cout << "//Wavelength: " << m_InputCnt.computeWavelength() << std::endl;

    if (!createDBfolder()) {
        std::cout << "Error creating database folder" << std::endl;
        return false;
    }

    if (!execute()) {
        std::cout << "Error executing simulation" << std::endl;
        return false;
    }
    return true;
}

// -----------------------------------------------------------------------------

bool
Runner::createDBfolder()
{
    const std::filesystem::path dbPath(getDbPathName());
    
    // Remove directory if it exists
    if (std::filesystem::exists(dbPath)) {
        std::filesystem::remove_all(dbPath);
    }
    
    // Create directory from scratch
    try {
        std::filesystem::create_directories(dbPath);
        return true;
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Error creating directory acoustic.db: " << e.what() << std::endl;
        return false;
    }
}

// -----------------------------------------------------------------------------

