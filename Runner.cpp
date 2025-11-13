#include <Runner.h>
#include <iostream>

// -----------------------------------------------------------------------------

bool
Runner::parseInput(std::istream& IS) {
    Input::FileParser fileParser(IS);
    for (Input::FileParser::const_iterator iter(fileParser); iter.isValid(); ++iter) {
        Input::StatementType type = iter.getType();
        
        if (type == Input::StatementType::SOURCE) {
            const Input::SourceStatement& source = iter.getStatement<Input::StatementType::SOURCE>();
            if (!m_InputCnt.setSource(source)) {
                return false;
            }
        } else if (type == Input::StatementType::BBOX) {
            const Input::BBoxStatement& bbox = iter.getStatement<Input::StatementType::BBOX>();
            if (!m_InputCnt.setBBox(bbox)) {
                return false;
            }
        }
        else if (type == Input::StatementType::MAX) {
            std::cerr << "Error in line " << iter.getLineNumber() << std::endl;
            return false;
        }
    }
    return true;
}

// -----------------------------------------------------------------------------

bool
Runner::run(std::istream& IS) {
    return parseInput(IS);
}

// -----------------------------------------------------------------------------

