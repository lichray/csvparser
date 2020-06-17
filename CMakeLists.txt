cmake_minimum_required (VERSION 3.15)

project ("csvparser" LANGUAGES CXX)

set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory ("csvparser")
set_directory_properties (PROPERTIES VS_STARTUP_PROJECT csvparser)
set_target_properties (csvparser PROPERTIES
                       VS_DEBUGGER_COMMAND_ARGUMENTS "cars.csv"
                       VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
