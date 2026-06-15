#ifndef LOGGING_LOGGING_HPP
#define LOGGING_LOGGING_HPP

#include <chrono>
#include <string>
#include <string_view>

#ifndef LOGIFACE_ENABLE_LOGGING
#define LOGIFACE_ENABLE_LOGGING 1
#endif

#ifndef LOGIFACE_PROJECT_ROOT
#define LOGIFACE_PROJECT_ROOT ""
#endif

#if __cplusplus >= 202002L
    #define STARTS_WITH(str, prefix) (std::string_view(str).starts_with(prefix))
#else
    #define STARTS_WITH(str, prefix) \
    (std::string_view(str).size() >= std::string_view(prefix).size() && \
    std::string_view(str).compare(0, std::string_view(prefix).size(), prefix) == 0)
#endif

namespace Logiface {

inline constexpr std::string_view ProjectRoot{LOGIFACE_PROJECT_ROOT};

inline constexpr std::string_view StripProjectRoot(std::string_view file) noexcept {
    if (ProjectRoot.empty() || file.size() < ProjectRoot.size()) {
        return file;
    }
    if (STARTS_WITH(file, ProjectRoot)) {
        file.remove_prefix(ProjectRoot.size());
        if (!file.empty() && (file.front() == '/' || file.front() == '\\')) {
            file.remove_prefix(1);
        }
    }
    return file;
}

// Severity levels
enum class Level {
    trace,
    debug,
    info,
    warn,
    error,
    critical
};

// Convert level to text
inline const char* ToString(Level l) noexcept {
    switch (l) {
        case Level::trace:    return "TRACE";
        case Level::debug:    return "DEBUG";
        case Level::info:     return "INFO";
        case Level::warn:     return "WARN";
        case Level::error:    return "ERROR";
        case Level::critical: return "CRITICAL";
    }
    return "UNKNOWN";
}

// Log record
struct Record {
    Level lvl;
    std::string message;  // own the string
    // `location` holds either a file path or a function name, depending on
    // which logging macro populated it.
    std::string_view location;
    int line;
    std::chrono::system_clock::time_point timestamp;
};

// Logger interface
struct Logger {
    virtual ~Logger() = default;
    virtual void Log(const Record& r) = 0;
    virtual void SetLevel(Level lvl) noexcept = 0;
    [[nodiscard]] virtual Level GetLevel() const noexcept = 0;
};

// Global logger storage
namespace Detail {
    inline Logger* g_logger{nullptr};
}

inline void SetLogger(Logger* l) noexcept {
    Detail::g_logger = l;
}

inline Logger* GetLogger() noexcept {
    return Detail::g_logger;
}

// ------------------------------------------------------------------
// Logging macros — moved to a separate zero-include header that is
// safe for the Global Module Fragment alongside `import std;`.
// ------------------------------------------------------------------
#include "logging_macros.hpp"

} // namespace Logiface

#endif // LOGGING_LOGGING_HPP
