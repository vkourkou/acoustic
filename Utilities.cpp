#include <Utilities.h>
#include <stdexcept>

namespace Utilities {

// -----------------------------------------------------------------------------

// Template specialization for size_t
template<>
std::optional<size_t>
convertTo<size_t>(const std::string& str) {
    if (str.empty()) {
        return std::nullopt;
    }
    
    try {
        // Check for negative sign
        if (str[0] == '-') {
            return std::nullopt;
        }
        
        // Convert using stol
        long longVal = std::stol(str);
        
        // Check if result is negative
        if (longVal < 0) {
            return std::nullopt;
        }
        
        // Convert to size_t
        size_t value = static_cast<size_t>(longVal);
        return value;
    } catch (const std::invalid_argument&) {
        return std::nullopt;
    } catch (const std::out_of_range&) {
        return std::nullopt;
    }
}

// -----------------------------------------------------------------------------

// Template specialization for long
template<>
std::optional<long>
convertTo<long>(const std::string& str) {
    if (str.empty()) {
        return std::nullopt;
    }
    
    try {
        // Convert using stol
        long value = std::stol(str);
        return value;
    } catch (const std::invalid_argument&) {
        return std::nullopt;
    } catch (const std::out_of_range&) {
        return std::nullopt;
    }
}

// -----------------------------------------------------------------------------

// Template specialization for float
template<>
std::optional<float>
convertTo<float>(const std::string& str) {
    if (str.empty()) {
        return std::nullopt;
    }
    
    try {
        // Convert using stof
        float value = std::stof(str);
        return value;
    } catch (const std::invalid_argument&) {
        return std::nullopt;
    } catch (const std::out_of_range&) {
        return std::nullopt;
    }
}

} // namespace Utilities

