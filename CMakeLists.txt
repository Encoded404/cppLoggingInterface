cmake_minimum_required(VERSION 3.30)

if( NOT CMAKE_EXPERIMENTAL_CXX_IMPORT_STD )
    if( ${CMAKE_VERSION} VERSION_GREATER_EQUAL 4.3 )
        set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD 451f2fe2-a8a2-47c3-bc32-94786d8fc91b)
    elseif( ${CMAKE_VERSION} VERSION_GREATER_EQUAL 3.30 )
        set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "d0edc3af-4c50-42ea-a356-e2862fe7a444")
    endif ()
endif ()

project(cppLoggingInterface LANGUAGES CXX)

option(LOGIFACE_USE_STD_MODULE "Use import std; instead of #include in the logging module" OFF)

# Traditional header-only INTERFACE library (always available).
add_library(logiface INTERFACE)
target_compile_features(logiface INTERFACE cxx_std_20)
target_include_directories(logiface INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

add_library(logiface::logiface ALIAS logiface)

# Share the top-level project root so each consumer can trim __FILE__ to a
# workspace-relative path before storing it into the log record.
target_compile_definitions(logiface INTERFACE
    LOGIFACE_PROJECT_ROOT="${CMAKE_SOURCE_DIR}/"
    $<$<CONFIG:Debug>:LOGIFACE_MIN_LEVEL=trace>
    $<$<CONFIG:RelWithDebInfo>:LOGIFACE_MIN_LEVEL=debug>
    $<$<CONFIG:Release>:LOGIFACE_MIN_LEVEL=info>
    $<$<CONFIG:MinSizeRel>:LOGIFACE_MIN_LEVEL=info>
)

if(LOGIFACE_USE_STD_MODULE)
    set(CMAKE_CXX_STANDARD 23)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_MODULE_STD ON)

    add_library(logiface_module OBJECT)

    target_sources(logiface_module PUBLIC
        FILE_SET CXX_MODULES
        BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/src
        FILES
            ${CMAKE_CURRENT_SOURCE_DIR}/src/logging/Logging.cppm
            ${CMAKE_CURRENT_SOURCE_DIR}/src/logging/ConsoleLogger.cppm
    )

    target_compile_features(logiface_module PUBLIC cxx_std_23)
    set_target_properties(logiface_module PROPERTIES CXX_MODULE_STD ON)

    # Depend on the traditional INTERFACE target to pick up
    # include directories and compile definitions (e.g. LOGIFACE_PROJECT_ROOT).
    target_link_libraries(logiface_module PUBLIC logiface)
endif()
