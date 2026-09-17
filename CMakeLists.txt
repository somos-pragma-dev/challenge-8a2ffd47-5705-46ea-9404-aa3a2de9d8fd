cmake_minimum_required(VERSION 3.28)
project(RiskEngineLmax VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(ENABLE_TESTING "Enable unit tests" ON)
option(ENABLE_COVERAGE "Enable code coverage" OFF)

if(CMAKE_BUILD_TYPE STREQUAL "")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
endif()

set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -Wextra -Wpedantic")

include(FetchContent)
FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.2
)
FetchContent_Declare(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.12.0
)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
)
FetchContent_Declare(
    catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.5.0
)
FetchContent_Declare(
    disruptor
    GIT_REPOSITORY https://github.com/LMAX-Exchange/disruptor-cpp.git
    GIT_TAG 4.0.0
)

set(json_BuildTests OFF CACHE INTERNAL "")
set(spdlog_BuildTests OFF CACHE INTERNAL "")
set(gtest_force_shared_crt ON CACHE INTERNAL "")
set(DISRUPTOR_BUILD_TESTS OFF CACHE INTERNAL "")
set(DISRUPTOR_BUILD_EXAMPLES OFF CACHE INTERNAL "")

FetchContent_MakeAvailable(json spdlog googletest catch2 disruptor)

include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/domain
    ${CMAKE_SOURCE_DIR}/src/application
    ${CMAKE_SOURCE_DIR}/src/adapters
    ${CMAKE_SOURCE_DIR}/src/infrastructure
    ${CMAKE_SOURCE_DIR}/src/core
    ${CMAKE_SOURCE_DIR}/config
)

find_package(Threads REQUIRED)

set(CORE_SOURCES
    src/core/RiskEngine.cpp
)

set(DOMAIN_SOURCES
)

set(APPLICATION_SOURCES
)

set(ADAPTER_SOURCES
    src/adapters/MarketDataFeedAdapter.cpp
    src/adapters/OrderManagementAdapter.cpp
)

set(INFRASTRUCTURE_SOURCES
    src/infrastructure/DisruptorConfig.cpp
    src/infrastructure/ShardingStrategy.cpp
)

set(ALL_LIB_SOURCES
    ${CORE_SOURCES}
    ${DOMAIN_SOURCES}
    ${APPLICATION_SOURCES}
    ${ADAPTER_SOURCES}
    ${INFRASTRUCTURE_SOURCES}
)

add_executable(risk-engine
    src/main.cpp
    ${ALL_LIB_SOURCES}
)

target_link_libraries(risk-engine
    PRIVATE
    Threads::Threads
    spdlog::spdlog
    nlohmann_json::nlohmann_json
    LMAX::Disruptor
)

target_compile_options(risk-engine PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:-Wno-unused-parameter>
    $<$<CXX_COMPILER_ID:Clang>:-Wno-unused-parameter>
)

if(ENABLE_TESTING)
    enable_testing()
    
    set(TEST_SOURCES
        test/RiskEngineTest.cpp
        test/CircuitBreakerTest.cpp
        test/ReplayEngineTest.cpp
    )
    
    add_executable(RiskEngineTest ${TEST_SOURCES} ${ALL_LIB_SOURCES})
    
    target_link_libraries(RiskEngineTest PRIVATE
        Threads::Threads
        spdlog::spdlog
        nlohmann_json::nlohmann_json
        LMAX::Disruptor
        GTest::gtest_main
        Catch2::Catch2WithMain
    )
    
    include(CTest)
    add_test(NAME RiskEngineTests COMMAND RiskEngineTest)
    
    add_executable(benchmark
        test/benchmark.cpp
        ${ALL_LIB_SOURCES}
    )
    
    target_link_libraries(benchmark PRIVATE
        Threads::Threads
        spdlog::spdlog
        nlohmann_json::nlohmann_json
        LMAX::Disruptor
    )
endif()

if(ENABLE_COVERAGE)
    include(CodeCoverage)
    append_coverage_compiler_flags()
    setup_target_for_coverage(
        NAME coverage
        EXECUTABLE RiskEngineTest
        DEPENDENCIES RiskEngineTest
    )
endif()

install(TARGETS risk-engine
    RUNTIME DESTINATION bin
)

configure_file(
    ${CMAKE_SOURCE_DIR}/config/risk_config.json
    ${CMAKE_BINARY_DIR}/config/risk_config.json
    COPYONLY
)

add_custom_command(TARGET risk-engine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/config
    $<TARGET_FILE_DIR:risk-engine>/config
)

message(STATUS "Risk Engine Build Configuration")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "  Testing: ${ENABLE_TESTING}")
message(STATUS "  Coverage: ${ENABLE_COVERAGE}")