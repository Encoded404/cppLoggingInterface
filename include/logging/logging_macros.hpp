#ifndef LOGGING_LOGGING_MACROS_HPP
#define LOGGING_LOGGING_MACROS_HPP

// ------------------------------------------------------------------
// Macro-only compat header for the logging interface.
//
// This file contains NO #include directives and does NOT pull in
// <memory>, so it is safe to place in the Global Module Fragment of
// a C++ module that also does `import std;` (avoids Clang bug
// LLVM #138558).
//
// Consumers must ensure the Logiface types (Level, Record, Logger,
// GetLogger, StripProjectRoot, etc.) are visible at macro-expansion
// time.  This is satisfied either by:
//   - #include <logging/logging.hpp>   (traditional, also provides
//     the types directly)
//   - import logiface;                 (C++20 module mode — types
//     come from the module)
// ------------------------------------------------------------------

// Allow the project to completely disable all logging at build time.
#ifndef LOGIFACE_ENABLE_LOGGING
#define LOGIFACE_ENABLE_LOGGING 1
#endif

#ifndef LOGIFACE_PROJECT_ROOT
#define LOGIFACE_PROJECT_ROOT ""
#endif

#if LOGIFACE_ENABLE_LOGGING

#ifndef LOGIFACE_MIN_LEVEL
#define LOGIFACE_MIN_LEVEL trace
#endif

// Allow choosing the project default at build-time: 0 = file, 1 = function
#ifndef LOGIFACE_DEFAULT_TO_FUNCTION
#define LOGIFACE_DEFAULT_TO_FUNCTION 0
#endif

// FORCE variants always pick a specific location source. The public
// `LOGIFACE_LOG` macro below will be mapped to one of these according to
// `LOGIFACE_DEFAULT_TO_FUNCTION` (and remains overrideable at call sites by
// using the FORCE macros directly).
#define LOGIFACE_FORCE_LOG_FILE(lvl, msg_expr)                                   \
    do {                                                                         \
        if (static_cast<int>(Logiface::Level::lvl) <                              \
            static_cast<int>(Logiface::Level::LOGIFACE_MIN_LEVEL))               \
            break;                                                               \
        auto lg = Logiface::GetLogger();                                         \
        if (!lg) break;                                                          \
        if (static_cast<int>(Logiface::Level::lvl) < static_cast<int>(lg->GetLevel())) \
            break;                                                               \
        lg->Log(Logiface::Record{                                               \
            Logiface::Level::lvl,                                               \
            (msg_expr), /* must produce std::string */                           \
            Logiface::StripProjectRoot(__FILE__),                                \
            __LINE__,                                                            \
            std::chrono::system_clock::now()});                                  \
    } while (0)

#define LOGIFACE_FORCE_LOG_FUNCTION(lvl, msg_expr)                               \
    do {                                                                         \
        if (static_cast<int>(Logiface::Level::lvl) <                              \
            static_cast<int>(Logiface::Level::LOGIFACE_MIN_LEVEL))               \
            break;                                                               \
        auto lg = Logiface::GetLogger();                                         \
        if (!lg) break;                                                          \
        if (static_cast<int>(Logiface::Level::lvl) < static_cast<int>(lg->GetLevel())) \
            break;                                                               \
        lg->Log(Logiface::Record{                                               \
            Logiface::Level::lvl,                                               \
            (msg_expr), /* must produce std::string */                           \
            std::string_view(__func__),                                          \
            __LINE__,                                                            \
            std::chrono::system_clock::now()});                                  \
    } while (0)

// Public mapping: `LOGIFACE_LOG` follows the project default but callers can
// still explicitly use the FORCE variants when needed.
#if LOGIFACE_DEFAULT_TO_FUNCTION
#define LOGIFACE_LOG(lvl, msg_expr) LOGIFACE_FORCE_LOG_FUNCTION(lvl, msg_expr)
#else
#define LOGIFACE_LOG(lvl, msg_expr) LOGIFACE_FORCE_LOG_FILE(lvl, msg_expr)
#endif

#else
// Logging disabled
#define LOGIFACE_LOG(lvl, msg_expr) ((void)0)
#endif

// Optional helper macro to guard expensive expressions
#define LOGIFACE_ENABLED(lvl) \
    (LOGIFACE_ENABLE_LOGGING && static_cast<int>(Logiface::Level::lvl) >= static_cast<int>(Logiface::Level::LOGIFACE_MIN_LEVEL))

#endif // LOGGING_LOGGING_MACROS_HPP
