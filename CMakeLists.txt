cmake_minimum_required(VERSION 3.25 FATAL_ERROR)


# Create project
project(sourcepp
        DESCRIPTION "Several modern C++20 libraries for sanely parsing Valve formats.")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# Add scripts
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(AddPrettyParser)
include(AddSourcePPLibrary)
include(FetchContent)
include(IncludeSubdirectory)
include(PrintOptions)
include(TargetOptimize)


# Options (update print_options at the bottom of this file when modifying)
option(SOURCEPP_LIBS_START_ENABLED "Libraries will all build by default"                             ON)
option(SOURCEPP_USE_BSPPP          "Build bsppp library"                 ${SOURCEPP_LIBS_START_ENABLED})
option(SOURCEPP_USE_DMXPP          "Build dmxpp library"                 ${SOURCEPP_LIBS_START_ENABLED})
option(SOURCEPP_USE_FSPP           "Build fspp library"                  ${SOURCEPP_LIBS_START_ENABLED})
option(SOURCEPP_USE_GAMEPP         "Build gamepp library"                ${SOURCEPP_LIBS_START_ENABLED})
option(SOURCEPP_USE_KVPP           "Build kvpp library"                  ${SOURCEPP_LIBS_START_ENABLED})
option(SOURCEPP_USE_MDLPP          "Build mdlpp library"                 ${SOURCEPP_LIBS_START_ENABLED})
option(SOURCEPP_USE_SNDPP          "Build sndpp library"                 ${SOURCEPP_LIBS_START_ENABLED})
option(SOURCEPP_USE_STEAMPP        "Build steampp library"               ${SOURCEPP_LIBS_START_ENABLED})
option(SOURCEPP_USE_TOOLPP         "Build toolpp library"                ${SOURCEPP_LIBS_START_ENABLED})
option(SOURCEPP_USE_VCRYPTPP       "Build vcryptpp library"              ${SOURCEPP_LIBS_START_ENABLED})
option(SOURCEPP_USE_VPKPP          "Build vpkpp library"                 ${SOURCEPP_LIBS_START_ENABLED})
option(SOURCEPP_USE_VTFPP          "Build vtfpp library"                 ${SOURCEPP_LIBS_START_ENABLED})

option(SOURCEPP_BUILD_BENCHMARKS      "Build benchmarks for supported libraries"      OFF)
option(SOURCEPP_BUILD_C_WRAPPERS      "Build C wrappers for supported libraries"      OFF)
option(SOURCEPP_BUILD_CSHARP_WRAPPERS "Build C# wrappers for supported libraries"     OFF)
option(SOURCEPP_BUILD_PYTHON_WRAPPERS "Build Python wrappers for supported libraries" OFF)
option(SOURCEPP_BUILD_WITH_TBB        "Build with support for std::execution"         OFF)
option(SOURCEPP_BUILD_WITH_THREADS    "Build with support for threading"               ON)
option(SOURCEPP_BUILD_TESTS           "Build tests for supported libraries"           OFF)
option(SOURCEPP_BUILD_TESTS_EXTRA     "Build extra tests that can't be run in CI"     OFF)
option(SOURCEPP_BUILD_WIN7_COMPAT     "Build with Windows 7 compatibility"             ON)

option(SOURCEPP_LINK_STATIC_MSVC_RUNTIME "Link to static MSVC runtime library" OFF)


# Options (library-specific)
option(SOURCEPP_VPKPP_SUPPORT_VPK_V54 "Support compressed v54 VPKs" ON)


# Option overrides
if(SOURCEPP_USE_BSPPP)
    set(SOURCEPP_USE_VPKPP ON CACHE INTERNAL "" FORCE)
endif()
if(SOURCEPP_USE_FSPP)
    set(SOURCEPP_USE_BSPPP   ON CACHE INTERNAL "" FORCE)
    set(SOURCEPP_USE_KVPP    ON CACHE INTERNAL "" FORCE)
    set(SOURCEPP_USE_STEAMPP ON CACHE INTERNAL "" FORCE)
    set(SOURCEPP_USE_VPKPP   ON CACHE INTERNAL "" FORCE)
endif()
if(SOURCEPP_USE_STEAMPP)
    set(SOURCEPP_USE_KVPP ON CACHE INTERNAL "" FORCE)
endif()
if(SOURCEPP_USE_TOOLPP)
    set(SOURCEPP_USE_KVPP ON CACHE INTERNAL "" FORCE)
endif()
if(SOURCEPP_USE_VPKPP)
    set(SOURCEPP_USE_KVPP ON CACHE INTERNAL "" FORCE)
endif()

if(SOURCEPP_BUILD_CSHARP_WRAPPERS)
    # These binding(s) rely on CWrap, CWrap relies on kvpp
    set(SOURCEPP_USE_KVPP         ON CACHE INTERNAL "" FORCE)
    set(SOURCEPP_BUILD_C_WRAPPERS ON CACHE INTERNAL "" FORCE)
    include_subdirectory(lang/cwrap)
endif()

if(MSVC)
    # MSVC does not rely on tbb for std::execution policies, so we can force this on
    set(SOURCEPP_BUILD_WITH_TBB ON CACHE INTERNAL "" FORCE)
endif()


# Versioning published builds
if(PROJECT_IS_TOP_LEVEL AND NOT SOURCEPP_VERSION)
    set(SOURCEPP_VERSION "0.0.1")
    message(AUTHOR_WARNING "SOURCEPP_VERSION is not defined, do not release this build publicly! Defaulting it to ${SOURCEPP_VERSION}...")
endif()


# Set defaults after project call
if(PROJECT_IS_TOP_LEVEL)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

if(SOURCEPP_USE_STATIC_MSVC_RUNTIME)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()


# Include thirdparty libraries
include_subdirectory(ext)


# Shared code
include_subdirectory(src/sourcepp)
include_subdirectory(src/sourcepp/compression)
include_subdirectory(src/sourcepp/crypto)
include_subdirectory(src/sourcepp/parser)


# Shared C code
if(SOURCEPP_BUILD_C_WRAPPERS)
    include_subdirectory(lang/c/src/sourceppc PROPAGATE ${PROJECT_NAME}c_SOURCES)
endif()


# Python bindings, part 1
if(SOURCEPP_BUILD_PYTHON_WRAPPERS)
    set(SOURCEPP_PYTHON_NAME "${PROJECT_NAME}_python")
    find_package(Python REQUIRED
            COMPONENTS Interpreter Development.Module
            OPTIONAL_COMPONENTS Development.SABIModule)
    FetchContent_Declare(
            nanobind
            GIT_REPOSITORY "https://github.com/wjakob/nanobind.git"
            GIT_TAG "origin/master")
    FetchContent_MakeAvailable(nanobind)
    set(${SOURCEPP_PYTHON_NAME}_SOURCES "")
    set(${SOURCEPP_PYTHON_NAME}_DEFINES "")

    # Set the version and git commit hash here
    find_package(Git REQUIRED)
    execute_process(COMMAND ${GIT_EXECUTABLE} log -1 --format=%H
            WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
            OUTPUT_VARIABLE SOURCEPP_GIT_TAG
            RESULT_VARIABLE SOURCEPP_GIT_TAG_ERROR
            OUTPUT_STRIP_TRAILING_WHITESPACE)
    if(NOT SOURCEPP_GIT_TAG)
        message(FATAL_ERROR "Failed to retrieve git commit SHA: ${SOURCEPP_GIT_TAG_ERROR}")
    endif()
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/lang/python/cfg/CMakeLists.txt" "${CMAKE_CURRENT_SOURCE_DIR}/lang/python/CMakeLists.txt" @ONLY)
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/lang/python/cfg/pyproject.toml" "${CMAKE_CURRENT_SOURCE_DIR}/lang/python/pyproject.toml")
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/lang/python/cfg/__init__.py"    "${CMAKE_CURRENT_SOURCE_DIR}/lang/python/src/sourcepp/__init__.py")

    # These need to be inside the python directory, let's duplicate them!
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/LICENSE"                      "${CMAKE_CURRENT_SOURCE_DIR}/lang/python/LICENSE"                      COPYONLY)
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/README.md"                    "${CMAKE_CURRENT_SOURCE_DIR}/lang/python/README.md"                    COPYONLY)
    configure_file("${CMAKE_CURRENT_SOURCE_DIR}/THIRDPARTY_LEGAL_NOTICES.txt" "${CMAKE_CURRENT_SOURCE_DIR}/lang/python/THIRDPARTY_LEGAL_NOTICES.txt" COPYONLY)
endif()


# Tests, part 1
if(SOURCEPP_BUILD_TESTS)
    set(SOURCEPP_TEST_NAME "${PROJECT_NAME}_test")
    FetchContent_Declare(
            googletest
            GIT_REPOSITORY "https://github.com/google/googletest.git"
            GIT_TAG "v1.15.2")
    FetchContent_MakeAvailable(googletest)
    enable_testing()
    set(${SOURCEPP_TEST_NAME}_SOURCES "")
    list(APPEND ${SOURCEPP_TEST_NAME}_DEPS gtest_main)

    # Download assets
    FetchContent_Declare(
            ${PROJECT_NAME}_assets
            GIT_REPOSITORY "https://github.com/craftablescience/sourcepp_assets.git"
            GIT_TAG "origin/main"
            GIT_SHALLOW ON
            SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/test/res")
    FetchContent_MakeAvailable(${PROJECT_NAME}_assets)
endif()


# Benchmarks
if(SOURCEPP_BUILD_BENCHMARKS)
    set(SOURCEPP_BENCH_NAME "${PROJECT_NAME}_bench")
    FetchContent_Declare(
            benchmark
            GIT_REPOSITORY https://github.com/google/benchmark.git
            GIT_TAG v1.9.0)
    set(BENCHMARK_ENABLE_TESTING OFF CACHE INTERNAL "" FORCE)
    FetchContent_MakeAvailable(benchmark)

    add_library(${SOURCEPP_BENCH_NAME} STATIC "${CMAKE_CURRENT_SOURCE_DIR}/bench/_bench.cpp")
    target_link_libraries(${SOURCEPP_BENCH_NAME} PUBLIC benchmark::benchmark)
    target_compile_definitions(${SOURCEPP_BENCH_NAME} PUBLIC ASSET_ROOT="${CMAKE_CURRENT_SOURCE_DIR}/test/res/")
endif()


# Add libraries
add_sourcepp_library(bsppp             PYTHON           ) # sourcepp::bsppp
add_sourcepp_library(dmxpp                    TEST      ) # sourcepp::dmxpp
add_sourcepp_library(fspp                     TEST      ) # sourcepp::fspp
add_sourcepp_library(gamepp   C        PYTHON TEST      ) # sourcepp::gamepp
add_sourcepp_library(kvpp                     TEST BENCH) # sourcepp::kvpp
add_sourcepp_library(mdlpp                    TEST      ) # sourcepp::mdlpp
add_sourcepp_library(sndpp                    TEST      ) # sourcepp::sndpp
add_sourcepp_library(steampp  C        PYTHON TEST      ) # sourcepp::steampp
add_sourcepp_library(toolpp            PYTHON TEST      ) # sourcepp::toolpp
add_sourcepp_library(vcryptpp C CSHARP PYTHON TEST      ) # sourcepp::vcryptpp
add_sourcepp_library(vpkpp    C CSHARP        TEST      ) # sourcepp::vpkpp
add_sourcepp_library(vtfpp    C CSHARP PYTHON TEST BENCH) # sourcepp::vtfpp


# Tests, part 2
if(SOURCEPP_BUILD_TESTS)
    add_executable(${SOURCEPP_TEST_NAME} ${${SOURCEPP_TEST_NAME}_SOURCES})
    target_link_libraries(${SOURCEPP_TEST_NAME} PUBLIC ${${SOURCEPP_TEST_NAME}_DEPS})
    target_compile_definitions(${SOURCEPP_TEST_NAME} PUBLIC ASSET_ROOT="${CMAKE_CURRENT_SOURCE_DIR}/test/res/")
    if(SOURCEPP_BUILD_TESTS_EXTRA)
        target_compile_definitions(${SOURCEPP_TEST_NAME} PUBLIC SOURCEPP_BUILD_TESTS_EXTRA)
    endif()
    include(GoogleTest)
    gtest_discover_tests(${SOURCEPP_TEST_NAME})
endif()


# Python bindings, part 2
if(SOURCEPP_BUILD_PYTHON_WRAPPERS)
    nanobind_add_module(${SOURCEPP_PYTHON_NAME} NB_STATIC STABLE_ABI LTO
            "${CMAKE_CURRENT_SOURCE_DIR}/lang/python/src/sourcepp.cpp"
            ${${SOURCEPP_PYTHON_NAME}_SOURCES})
    set_target_properties(${SOURCEPP_PYTHON_NAME} PROPERTIES
            OUTPUT_NAME "_${PROJECT_NAME}_impl"
            LIBRARY_OUTPUT_DIRECTORY         "${CMAKE_CURRENT_SOURCE_DIR}/lang/python/src/sourcepp"
            LIBRARY_OUTPUT_DIRECTORY_DEBUG   "${CMAKE_CURRENT_SOURCE_DIR}/lang/python/src/sourcepp"
            LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/lang/python/src/sourcepp")
    target_compile_definitions(${SOURCEPP_PYTHON_NAME} PRIVATE ${${SOURCEPP_PYTHON_NAME}_DEFINES})
    target_link_libraries(${SOURCEPP_PYTHON_NAME} PRIVATE ${${SOURCEPP_PYTHON_NAME}_DEPS})
    install(TARGETS ${SOURCEPP_PYTHON_NAME} LIBRARY DESTINATION "./${PROJECT_NAME}")

    add_custom_target(${SOURCEPP_PYTHON_NAME}_all)
    add_dependencies(${SOURCEPP_PYTHON_NAME}_all ${SOURCEPP_PYTHON_NAME})
endif()


# Generate CWrap-generated bindings
if(SOURCEPP_BUILD_CSHARP_WRAPPERS)
    add_custom_target(sourcepp_generate_cwrap_bindings ALL
            COMMAND sourcepp_cwrap "${CMAKE_CURRENT_SOURCE_DIR}/lang/cwrap/sourcepp.kv"
            DEPENDS sourcepp_cwrap
            BYPRODUCTS "${CMAKE_CURRENT_SOURCE_DIR}/lang/cwrap/sourcepp.kv"
            COMMENT "Generating CWrap bindings"
            VERBATIM)
endif()


# Print options
print_options(OPTIONS
        USE_BSPPP USE_DMXPP USE_FSPP USE_GAMEPP USE_KVPP USE_MDLPP USE_SNDPP USE_STEAMPP USE_TOOLPP USE_VCRYPTPP USE_VPKPP USE_VTFPP
        BUILD_BENCHMARKS BUILD_C_WRAPPERS BUILD_CSHARP_WRAPPERS BUILD_PYTHON_WRAPPERS BUILD_WITH_TBB BUILD_WITH_THREADS BUILD_TESTS BUILD_TESTS_EXTRA BUILD_WIN7_COMPAT
        LINK_STATIC_MSVC_RUNTIME)
