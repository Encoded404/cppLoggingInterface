cmake_minimum_required(VERSION 3.15)

project(cppLoggingInterface LANGUAGES CXX)

add_library(logiface INTERFACE)
target_compile_features(logiface INTERFACE cxx_std_20)
target_include_directories(logiface INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
# Share the top-level project root so each consumer can trim __FILE__ to a
# workspace-relative path before storing it into the log record.
target_compile_definitions(logiface INTERFACE LOGIFACE_PROJECT_ROOT="\"${CMAKE_SOURCE_DIR}/\"")
