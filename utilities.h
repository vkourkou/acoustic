#ifndef UTILITIES_H
#define UTILITIES_H

#include <string>
#include <optional>
#include <chrono>
#include <iostream>
#include <fstream>
#include <sstream>

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

enum ProcessingType : unsigned {
    CPU = 0,
    GPU = 1
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
std::optional<int>
convertTo<int>(const std::string& str);

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

// -----------------------------------------------------------------------------

class PerformanceStats {
private:
    std::string m_label;
    std::chrono::steady_clock::time_point m_constructionTime;
    
    static std::chrono::steady_clock::time_point getProgramStartTime()
    {
        static std::chrono::steady_clock::time_point s_programStartTime = std::chrono::steady_clock::now();
        return s_programStartTime;
    }

    void getMemoryUsage(long& currentKB, long& peakKB) const
    {
        currentKB = 0;
        peakKB = 0;
        std::ifstream statusFile("/proc/self/status");
        std::string line;
        
        while (std::getline(statusFile, line)) {
            if (line.find("VmRSS:") == 0) {
                std::istringstream iss(line);
                std::string key;
                iss >> key >> currentKB;
            } else if (line.find("VmHWM:") == 0) {
                std::istringstream iss(line);
                std::string key;
                iss >> key >> peakKB;
            }
            
            if (currentKB > 0 && peakKB > 0) {
                break;
            }
        }
    }

    void printTime(const char* tag) const
    {
        auto currentTime = std::chrono::steady_clock::now();
        auto elapsedDuration = std::chrono::duration_cast<std::chrono::seconds>(currentTime - getProgramStartTime()).count();
        auto stageDuration = std::chrono::duration_cast<std::chrono::seconds>(currentTime - m_constructionTime).count();
        
        long currentKB = 0;
        long peakKB = 0;
        getMemoryUsage(currentKB, peakKB);
        
        double currentMB = currentKB / 1024.0;
        double peakMB = peakKB / 1024.0;
        
        std::cout << m_label << " [" << tag << "] ELAPSED TIME " << elapsedDuration << " STAGE TIME " << stageDuration << " MEMORY " << currentMB << " HWM " << peakMB << std::endl;
    }

public:
    // -----------------------------------------------------------------------------

    PerformanceStats(const std::string& label)
    : m_label(label), m_constructionTime(std::chrono::steady_clock::now())
    {
        printTime("start");
    }

    // -----------------------------------------------------------------------------

    ~PerformanceStats()
    {
        printTime("exit");
    }

    // -----------------------------------------------------------------------------

    // Copy constructor - deleted
    PerformanceStats(const PerformanceStats& other) = delete;

    // -----------------------------------------------------------------------------

    // Move constructor - deleted
    PerformanceStats(PerformanceStats&& other) noexcept = delete;

    // -----------------------------------------------------------------------------

    // Copy assignment operator - deleted
    PerformanceStats& operator=(const PerformanceStats& other) = delete;

    // -----------------------------------------------------------------------------

    // Move assignment operator - deleted
    PerformanceStats& operator=(PerformanceStats&& other) noexcept = delete;

    // -----------------------------------------------------------------------------
};

} // namespace Utilities

#endif // UTILITIES_H

