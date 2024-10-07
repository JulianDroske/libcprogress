cmake_minimum_required(VERSION 3.10)

project(libcprogress)

set(LIBCProgress_VERSION_MAJOR 1)
set(LIBCProgress_VERSION_MINOR 0)

add_library(cprogress STATIC
    cprogress.h
    ${CMAKE_CURRENT_BINARY_DIR}/cprogress.c
)

# Create a temporary source file
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/cprogress.c "// This file is just for CMake linking")

set_target_properties(cprogress PROPERTIES
    VERSION ${LIBCProgress_VERSION_MAJOR}.${LIBCProgress_VERSION_MINOR}
    SOVERSION ${LIBCProgress_VERSION_MAJOR}
    PUBLIC_HEADER cprogress.h
)

install(TARGETS cprogress
    EXPORT cprogressTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    PUBLIC_HEADER DESTINATION include
)

include(CMakePackageConfigHelpers)

configure_package_config_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cprogressConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/cprogressConfig.cmake
    INSTALL_DESTINATION lib/cmake/cprogress
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/cprogressConfigVersion.cmake
    VERSION ${LIBCProgress_VERSION_MAJOR}.${LIBCProgress_VERSION_MINOR}
    COMPATIBILITY AnyNewerVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/cprogressConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/cprogressConfigVersion.cmake
    DESTINATION lib/cmake/cprogress
)

install(EXPORT cprogressTargets
    FILE cprogressTargets.cmake
    NAMESPACE cprogress::
    DESTINATION lib/cmake/cprogress
)

enable_testing()

add_subdirectory(test)