#ifndef UTILITIES_H
#define UTILITIES_H

#include <string>
#include <optional>

namespace Utilities {

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

