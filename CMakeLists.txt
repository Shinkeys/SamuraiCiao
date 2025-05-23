# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.18)


project (SamuraiCiao)

find_package(OpenGL REQUIRED)


file(GLOB_RECURSE GLSL_SOURCE_FILES "shaders/*.glsl")
file(GLOB_RECURSE HEADER_FILES "headers/*.h")
file(GLOB_RECURSE CPP_SOURCE_FILES "src/*.cpp")

# MAKING EXECUTABLE(INCLUDING ALL CPP FILES AND GLAD)
add_executable(SamuraiCiao ${CPP_SOURCE_FILES}
${GLSL_SOURCE_FILES} ${HEADER_FILES} vendor/glad/src/glad.c)

set_target_properties(SamuraiCiao PROPERTIES CXX_STANDARD 23 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS YES)
# # need to turn off rtti, otherwise jolt won't compile
# target_compile_options(SamuraiCiao PRIVATE -fno-rtti)

# Assimp compile flags
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_NO_EXPORT ON CACHE BOOL "" FORCE)


# GLFW compile flags
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

# defines for Jolt to compile
add_definitions(-DJPH_PROFILE_ENABLED -DJPH_DEBUG_RENDERER -DJPH_OBJECT_STREAM)


target_include_directories(SamuraiCiao PRIVATE 
                vendor/stb
                vendor/glm
                vendor/assimp/include
                vendor/imgui
                vendor/imgui/backends
                vendor/glad/include
                vendor/JoltPhysics
)
# Imgui building library
add_library(imgui STATIC 
                vendor/imgui/imgui.cpp
                vendor/imgui/imgui_draw.cpp
                vendor/imgui/imgui_tables.cpp
                vendor/imgui/imgui_widgets.cpp
                vendor/imgui/backends/imgui_impl_glfw.cpp
                vendor/imgui/backends/imgui_impl_opengl3.cpp
                vendor/imgui/imgui_demo.cpp
)
target_include_directories(imgui PUBLIC
                vendor/imgui
                vendor/glfw/include)


# dll should be in ROOT(in the same folder as exe)
add_subdirectory(vendor/assimp)
add_subdirectory(vendor/glfw)

# Jolt wants to compile as Dynamic Library somehow so would use this
if(WIN32)
    set(USE_STATIC_MSVC_RUNTIME_LIBRARY OFF)
endif(WIN32)
add_subdirectory(vendor/JoltPhysics/Build)


target_link_libraries(SamuraiCiao glfw OpenGL::GL imgui assimp Jolt)
#-fsanitize=address -fsanitize=undefined remove that to make it possible to work with renderdoc, otherwise place in target libs


