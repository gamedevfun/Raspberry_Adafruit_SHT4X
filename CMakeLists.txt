cmake_minimum_required(VERSION 3.16)
project(sht4x_cpp VERSION 0.1.0 LANGUAGES CXX)

option(SHT4X_ENABLE_LINUX_I2C "Enable the Linux i2c-dev backend" ON)
option(SHT4X_REQUIRE_LINUX_I2C "Fail configure if Linux i2c-dev is requested but unavailable" OFF)
option(SHT4X_BUILD_EXAMPLES "Build example programs" ON)
option(SHT4X_BUILD_TESTS "Build test programs" ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_library(sht4x
    src/sht4x.cpp
    src/hardware.cpp
)
add_library(sht4x::sht4x ALIAS sht4x)

target_include_directories(sht4x
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_compile_features(sht4x PUBLIC cxx_std_20)

if(SHT4X_ENABLE_LINUX_I2C)
    if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
        target_compile_definitions(sht4x PUBLIC SHT4X_ENABLE_LINUX_I2C)
        message(STATUS "Linux i2c-dev backend enabled")
    else()
        if(SHT4X_REQUIRE_LINUX_I2C)
            message(FATAL_ERROR "SHT4X_REQUIRE_LINUX_I2C is ON, but the target system is not Linux.")
        else()
            message(WARNING "Linux i2c-dev backend disabled because the target system is not Linux")
        endif()
    endif()
endif()

if(SHT4X_BUILD_EXAMPLES)
    add_executable(sht4x_read_once examples/read_once.cpp)
    target_link_libraries(sht4x_read_once PRIVATE sht4x)

    add_executable(sht4x_monitor examples/monitor.cpp)
    target_link_libraries(sht4x_monitor PRIVATE sht4x)
endif()

if(SHT4X_BUILD_TESTS)
    include(CTest)
    enable_testing()

    add_executable(sht4x_tests tests/test_main.cpp)
    target_link_libraries(sht4x_tests PRIVATE sht4x)
    add_test(NAME sht4x_tests COMMAND sht4x_tests)
endif()

include(GNUInstallDirs)

install(TARGETS sht4x
    EXPORT sht4xTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

if(SHT4X_BUILD_EXAMPLES)
    install(TARGETS
        sht4x_read_once
        sht4x_monitor
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
endif()
