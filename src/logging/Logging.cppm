module;

#ifndef LOGIFACE_USE_STD_MODULE
#include <string_view>
#include <string>
#include <chrono>
#endif

export module logiface;

#ifdef LOGIFACE_USE_STD_MODULE
import std;
#endif

export namespace Logiface {

inline constexpr std::string_view ProjectRoot{LOGIFACE_PROJECT_ROOT};

inline constexpr std::string_view StripProjectRoot(std::string_view file) noexcept {
    if (ProjectRoot.empty() || file.size() < ProjectRoot.size()) {
        return file;
    }
    if (file.starts_with(ProjectRoot)) {
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
    std::string message;
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

// Global logger storage and accessors (raw pointer, no shared_ptr)
namespace Detail {
    inline Logger* g_logger{nullptr};
}

inline void SetLogger(Logger* l) noexcept {
    Detail::g_logger = l;
}

inline Logger* GetLogger() noexcept {
    return Detail::g_logger;
}

} // namespace Logiface
