#ifndef UTILITIES_H
#define UTILITIES_H

#include <string>
#include <optional>

// Typedef for float dimension type
typedef float Dimension_t;

// Typedef for frequency type
typedef float Frequency_t;

typedef float Time_t;

// Typedef for amplitude type
typedef float Amplitude_t;

typedef float Velocity_t;

enum Direction : unsigned {
    X = 0,
    Y = 1,
    MAX = 2
};

namespace Utilities {

// Case-insensitive string comparison
bool caseInsensitiveEquals(const std::string& a, const std::string& b);

// Template function to convert string to type T
template<typename T>
std::optional<T> convertTo(const std::string& str);

// Forward declarations of template specializations
template<>
std::optional<size_t>
convertTo<size_t>(const std::string& str);

template<>
std::optional<long>
convertTo<long>(const std::string& str);

template<>
std::optional<float>
convertTo<float>(const std::string& str);

// This function will compute the minimum multiplier to reduce the truncation relative error
// below the given MaxAlloweError.
// if Y = value * multiplier
// we will compute the minimum T (power of 2) such that
// abs((Y - round(Y)) / Y) < MaxAlloweError

long 
computeMultiplierToReduceTruncationError(float Value, float MaxAlloweError);

} // namespace Utilities

#endif // UTILITIES_H

