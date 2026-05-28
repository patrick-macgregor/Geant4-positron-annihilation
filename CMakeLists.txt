#----------------------------------------------------------------------------
# Setup the project
#
cmake_minimum_required(VERSION 3.16...3.27)
project(positron_annihilation_13B_DH)

# Ensure IDE knows where definitions are
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

# Place output in the bin directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

#----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# See the documentation for a guide on how to enable/disable specific components
#
find_package(Geant4 REQUIRED ui_all vis_all)

#----------------------------------------------------------------------------
# Locate sources and headers for this project
# NB: headers are included so they will show up in IDEs
#
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

#----------------------------------------------------------------------------
# Add the executable, use our local headers, and link it to the Geant4 libraries
#
add_executable(positron_annihilation_13B_DH positron_annihilation_13B_DH.cc ${sources} ${headers})
target_include_directories(positron_annihilation_13B_DH PRIVATE include)
target_link_libraries(positron_annihilation_13B_DH PRIVATE ${Geant4_LIBRARIES})

#----------------------------------------------------------------------------
# Copy all scripts to the build directory, i.e. the directory in which we
# build B1. This is so that we can run the executable directly because it
# relies on these scripts being in the current working directory.
#
set(POSITRON_ANNIHILATION_SCRIPTS
    detector_geometry.mac
    init_vis.mac
    run.mac
    vis.mac
)

foreach(_script ${POSITRON_ANNIHILATION_SCRIPTS})
    configure_file(
        ${PROJECT_SOURCE_DIR}/${_script}
        ${PROJECT_BINARY_DIR}/${_script}
        COPYONLY
    )
endforeach()
