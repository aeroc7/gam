# Source file storage
add_library(project_source INTERFACE)

# Add subdirectories here that append source files to the 'library' above.

add_subdirectory(parsers)

# Ex:
# add_subdirectory(util)
#
# -> Inside util directory "CMakeLists.txt" file
#
#    target_sources(project_source INTERFACE
#      ${CMAKE_CURRENT_LIST_DIR}/log.cpp
#    )

# Standalone executable
add_executable(${CMAKE_PROJECT_NAME}_standalone main.c)

# Link our project libraries
add_library(project_libraries INTERFACE)

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

target_link_libraries(${CMAKE_PROJECT_NAME}_standalone
    PRIVATE
        project_source
        project_options
        project_warnings
        project_libraries
)