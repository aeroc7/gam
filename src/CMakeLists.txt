set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
# Source file storage
add_library(project_source INTERFACE)

# Include util directory
target_include_directories(project_source INTERFACE "${CMAKE_CURRENT_LIST_DIR}/")

# Add subdirectories here that append source files to the 'library' above.

add_subdirectory(gam)
add_subdirectory(graphics)
add_subdirectory(parsers)
add_subdirectory(utils)

# Ex:
# add_subdirectory(util)
#
# -> Inside util directory "CMakeLists.txt" file
#
#    target_sources(project_source INTERFACE
#      ${CMAKE_CURRENT_LIST_DIR}/log.cpp
#    )

# Standalone executable
add_executable(${CMAKE_PROJECT_NAME} main.c)

# Link our project libraries
add_library(project_libraries INTERFACE)

# X-Plane path
option(XPLANE_ROOT "Path to your X-Plane root directory" REQUIRED)
if(NOT XPLANE_ROOT)
    message(FATAL_ERROR "Please set an X-Plane root directory with XPLANE_ROOT=[dir]")
else()
    target_compile_definitions(project_options INTERFACE -DUSER_XPLANE_ROOT="${XPLANE_ROOT}")
endif()

# Various library definitions
target_compile_definitions(project_options INTERFACE -DGLEW_STATIC -DM_PI=3.1415926535897932)

if(WIN32)
    target_link_libraries(project_libraries
        INTERFACE
            -static
            -static-libgcc
            -static-libstdc++
    )
elseif(UNIX AND NOT APPLE)
   target_link_libraries(project_libraries
        INTERFACE
            -static-libgcc
            -static-libstdc++
   )
elseif(APPLE)
endif()

target_link_libraries(${CMAKE_PROJECT_NAME}
    PRIVATE
        project_source
        project_options
        project_warnings
        project_libraries
        GL
        cairo
        freetype
        pixman
        glew
        glfw
        xpsdk
        -lm
)