# CMakeList.txt : CMake project for vkConway, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

add_subdirectory(vendor/sdl EXCLUDE_FROM_ALL)
include_directories(vendor/sdl/include)
project ("vkConway")

# Add source to this project's executable.
add_executable (vkConway WIN32 "vkConway.cpp" "vkConway.h")
target_link_libraries(vkConway PRIVATE SDL2::SDL2main)
target_link_libraries(vkConway PRIVATE SDL2::SDL2)

add_custom_command(TARGET vkConway POST_BUILD        # Adds a post-build event to MyTest
    COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
        "${PROJECT_BINARY_DIR}/vendor/SDL/SDL2${SDL_CMAKE_DEBUG_POSTFIX}.dll"      # <--this is in-file
        $<TARGET_FILE_DIR:vkConway>)                 # <--this is out-file path

add_custom_command(TARGET vkConway POST_BUILD        # Adds a post-build event to MyTest
    COMMAND ${CMAKE_COMMAND} -E copy_if_different  # which executes "cmake - E copy_if_different..."
        "${PROJECT_BINARY_DIR}/vendor/SDL/README-SDL.txt"      # <--this is in-file
        $<TARGET_FILE_DIR:vkConway>)                 # <--this is out-file path

# TODO: Add tests and install targets if needed.