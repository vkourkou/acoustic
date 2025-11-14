#ifndef UTILITIES_H
#define UTILITIES_H

#include <string>
#include <optional>

// Typedef for float dimension type
typedef float Dimension_t;

// Typedef for frequency type
typedef float Frequency_t;

// Typedef for amplitude type
typedef float Amplitude_t;

typedef float Velocity_t;

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

} // namespace Utilities

#endif // UTILITIES_H

