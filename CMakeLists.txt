cmake_minimum_required(VERSION 3.10)
project(RedCircle)

# --- Options ---

# Option to generate GLAD using the installed glad module
option(GENERATE_GLAD "Generate GLAD using the installed glad module" ON)

# Option to use ANGLE for rendering (Windows only)
option(USE_ANGLE "Use ANGLE for rendering (Windows only)" ON)

# --- Find GLFW ---

find_package(glfw3 3.3 REQUIRED)
set(GLFW_INCLUDE_ES3 ON)

# --- Find Python 3 ---

find_package(Python3 QUIET COMPONENTS Interpreter)
if(NOT Python3_Interpreter_FOUND)
    message(FATAL_ERROR "Python 3 interpreter not found. Cannot check for or generate GLAD.")
endif()

# --- Check if glad module is installed ---

execute_process(
  COMMAND "${Python3_EXECUTABLE}" -c "import glad; print(glad.__file__)"
  OUTPUT_VARIABLE GLAD_MODULE_PATH
  ERROR_QUIET
  RESULT_VARIABLE GLAD_MODULE_CHECK_RESULT
)

# --- GLAD Generation (if GENERATE_GLAD is ON) ---

if(GENERATE_GLAD)
  if(GLAD_MODULE_CHECK_RESULT EQUAL 0)
    # Define output directories (inside the build directory)
    set(GLAD_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/glad")
    set(GLAD_INCLUDE_DIR "${GLAD_OUTPUT_DIR}/include")
    set(GLAD_SOURCE_DIR "${GLAD_OUTPUT_DIR}/src")

    # Print debug info
    message(STATUS "Python executable: ${Python3_EXECUTABLE}")
    message(STATUS "Glad module path: ${GLAD_MODULE_PATH}")
    message(STATUS "GLAD_OUTPUT_DIR: ${GLAD_OUTPUT_DIR}")

    # Create a custom command to generate GLAD
    add_custom_command(
      OUTPUT "${GLAD_SOURCE_DIR}/gles2.c" "${GLAD_INCLUDE_DIR}/glad/gles2.h" "${GLAD_INCLUDE_DIR}/KHR/khrplatform.h"
      COMMAND "${Python3_EXECUTABLE}" -m glad "--out-path=${GLAD_OUTPUT_DIR}" --api gles2=3.0 c
      WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
      COMMENT "Generating GLAD sources"
      VERBATIM
    )

    # Create a custom target to trigger the generation
    add_custom_target(generate_glad
      DEPENDS "${GLAD_SOURCE_DIR}/gles2.c" "${GLAD_INCLUDE_DIR}/glad/gles2.h" "${GLAD_INCLUDE_DIR}/KHR/khrplatform.h"
    )

    # Create an interface library for GLAD sources (available after generation)
    add_library(glad INTERFACE)
    target_sources(glad INTERFACE "${GLAD_SOURCE_DIR}/gles2.c")
    target_include_directories(glad INTERFACE "${GLAD_INCLUDE_DIR}")

    # Add dependency so glad target is built before targets that need it
    add_dependencies(glad generate_glad)

  else()
    message(FATAL_ERROR "glad module not found. Please install it using pip (pip install glad).")
  endif()
endif()

# --- Check for ANGLE (Windows only) ---

if(WIN32 AND USE_ANGLE)
    find_path(
      ANGLE_INCLUDE_DIR
      NAMES EGL/egl.h
      PATH_SUFFIXES include
      DOC "Path to ANGLE include directory"
    )

    find_library(
      ANGLE_LIBRARY
      NAMES libEGL libEGL.lib
      PATH_SUFFIXES lib
      DOC "Path to ANGLE EGL library"
    )

    find_library(
      ANGLE_GLES_LIBRARY
      NAMES libGLESv2 libGLESv2.lib
      PATH_SUFFIXES lib
      DOC "Path to ANGLE GLESv2 library"
    )

    if(ANGLE_INCLUDE_DIR AND ANGLE_LIBRARY AND ANGLE_GLES_LIBRARY)
        message(STATUS "ANGLE found: ${ANGLE_INCLUDE_DIR}")
    else()
        message(FATAL_ERROR "ANGLE requested but not found. Please install it or turn USE_ANGLE OFF.")
    endif()
endif()

# --- Main Executable ---

add_executable(red_circle main.cpp rendering.cpp)
target_link_libraries(red_circle glfw)

# Link to GLAD if we generated it
if(GENERATE_GLAD)
  target_link_libraries(red_circle glad)
endif()

# --- Set up ANGLE (Windows only) ---

if(WIN32 AND USE_ANGLE)
    target_include_directories(red_circle PRIVATE "${ANGLE_INCLUDE_DIR}")
    target_link_libraries(red_circle "${ANGLE_LIBRARY}" "${ANGLE_GLES_LIBRARY}")
    target_compile_definitions(red_circle PRIVATE USING_ANGLE)

    # Set window hints for ANGLE
    set(GLFW_CONTEXT_CREATION_API "EGL" CACHE STRING "" FORCE)
    set(GLFW_CLIENT_API "OPENGL_ES" CACHE STRING "" FORCE)
    set(GLFW_CONTEXT_VERSION_MAJOR 3 CACHE STRING "" FORCE)
    set(GLFW_CONTEXT_VERSION_MINOR 0 CACHE STRING "" FORCE)
endif()

# --- macOS specific settings ---

if (APPLE)
    set_target_properties(red_circle PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_INFO_STRING "Red Circle"
        FRAMEWORK TRUE
    )
    target_link_libraries(red_circle "-framework Cocoa -framework OpenGL -framework IOKit")
endif()
