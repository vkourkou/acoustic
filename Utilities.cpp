#include <Utilities.h>
#include <stdexcept>
#include <cctype>
#include <cmath>
#include <limits>

namespace Utilities {

// -----------------------------------------------------------------------------

bool
caseInsensitiveEquals(const std::string& a, const std::string& b) {
    if (a.length() != b.length()) {
        return false;
    }
    
    for (size_t i = 0; i < a.length(); ++i) {
        if (std::tolower(static_cast<unsigned char>(a[i])) != 
            std::tolower(static_cast<unsigned char>(b[i]))) {
            return false;
        }
    }
    
    return true;
}

// -----------------------------------------------------------------------------

// Template specialization for size_t
template<>
std::optional<size_t>
convertTo<size_t>(const std::string& str) {
    if (str.empty()) {
        return std::nullopt;
    }
    
    try {
        // Check for negative sign or leading whitespace
        if (str[0] == '-' || std::isspace(static_cast<unsigned char>(str[0]))) {
            return std::nullopt;
        }
        
        // Convert using stol and check if entire string was consumed
        size_t pos = 0;
        long longVal = std::stol(str, &pos);
        
        // Check if entire string was consumed
        if (pos != str.length()) {
            return std::nullopt;
        }
        
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
        // Check for leading whitespace
        if (std::isspace(static_cast<unsigned char>(str[0]))) {
            return std::nullopt;
        }
        
        // Convert using stol and check if entire string was consumed
        size_t pos = 0;
        long value = std::stol(str, &pos);
        
        // Check if entire string was consumed
        if (pos != str.length()) {
            return std::nullopt;
        }
        
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
        // Check for leading whitespace
        if (std::isspace(static_cast<unsigned char>(str[0]))) {
            return std::nullopt;
        }
        
        // Convert using stof and check if entire string was consumed
        size_t pos = 0;
        float value = std::stof(str, &pos);
        
        // Check if entire string was consumed
        if (pos != str.length()) {
            return std::nullopt;
        }
        
        return value;
    } catch (const std::invalid_argument&) {
        return std::nullopt;
    } catch (const std::out_of_range&) {
        return std::nullopt;
    }
}

// -----------------------------------------------------------------------------

long 
computeMultiplierToReduceTruncationError(float Value, float MaxAlloweError) {
    if (Value == 0.0f) {
        return 1;
    }
    long multiplier = 1;
    while (multiplier * 2 < std::numeric_limits<long>::max()) {
        double Y = Value * multiplier;
        double Error = std::abs((Y - std::round(Y)) / Y);
        if (Error < MaxAlloweError) {
            return multiplier;
        }
        multiplier *= 2;
    }
    return multiplier;
}

//-----------------------------------------------------------------------------

} // namespace Utilities

