# Specify the minimum version of CMake that can run this script
cmake_minimum_required(VERSION 3.12)

# Project name and version
project(SynthPi VERSION "0.5")

# Project name and version macros
add_definitions(-DPROJECT_NAME="${CMAKE_PROJECT_NAME}")
add_definitions(-DPROJECT_VERSION="${CMAKE_PROJECT_VERSION}")

# Generate compile_commands.json for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Request C++ 11 standard features
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

### OUTPUT TARGET ###

add_executable(SynthPi ${PROJECT_SOURCE_DIR}/src/main.cxx)

# Set the output directory for executables
set_target_properties(SynthPi PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)



### SOURCE CODE ###

# Get a list of source files
file(GLOB SRCS src/*.cpp)

# Library of all source code
add_library(source ${SRCS})
set_target_properties(source PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

# Link source library to SynthPi
target_link_libraries(SynthPi source)

### LIBRARIES ###

# Create base library from dummy file

add_library(library ${PROJECT_SOURCE_DIR}/src/.dummy.cxx)
set_target_properties(library PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

find_package(ALSA REQUIRED)                                    
if (ALSA_FOUND)                                                
    include_directories(${ALSA_INCLUDE_DIRS})                  
    target_link_libraries (library ${ALSA_LIBRARIES})      
endif(ALSA_FOUND)  

## Add, include and link each library subdirector
# For pre-compiled libraries (e.g. Jack) only linking is needed
# Add

add_subdirectory(
    ${PROJECT_SOURCE_DIR}/lib/AudioFile
    ${PROJECT_SOURCE_DIR}/lib/DSP-Cpp-filters-master/lib
)

# Include
include_directories(
    ${PROJECT_SOURCE_DIR}/lib/AudioFile
    ${PROJECT_SOURCE_DIR}/lib/DSP-Cpp-filters-master/lib
)

find_package (Threads)

# Link
target_link_libraries(library
    jack     
    ${CMAKE_THREAD_LIBS_INIT}
)

# Link sum library to SynthPi
target_link_libraries(SynthPi library)

### UNIT TESTING ###

# Enable testing
enable_testing()

# Use Boost library for unit tests
find_package(Boost COMPONENTS unit_test_framework REQUIRED)

# Link to source directory
include_directories(${Boost_INCLUDE_DIRS} ${PROJECT_SOURCE_DIR}/src)

# Include test files
file(GLOB TEST_SRCS ${PROJECT_SOURCE_DIR}/test/*.cpp)
# Run through each test file
foreach(testSrc ${TEST_SRCS})
    # Get extension-less file name
    get_filename_component(testName ${testSrc} NAME_WE)
    # Add target
    add_executable(${testName} ${testSrc})
    # Link to Boost, source code and other external libraries
    target_link_libraries(${testName} ${Boost_LIBRARIES} source library)
    # Put test executables in their own directory
    set_target_properties(${testName} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/test/bin)

    # Add test to execution
    add_test(
        NAME ${testName}
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/test/bin
        COMMAND ${testName}
    )
endforeach(testSrc)

# Add definition for the project root directly for file loading
add_definitions(-DPROJECT_ROOT_DIR="${PROJECT_SOURCE_DIR}/")
