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
target_include_directories(SamuraiCiao PRIVATE vendor/glad/include)

set_target_properties(SamuraiCiao PROPERTIES CXX_STANDARD 20 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS YES)

# ASAN, UBSAN


# compiling assimp
add_subdirectory(vendor/assimp)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_NO_EXPORT ON CACHE BOOL "" FORCE)



add_subdirectory(vendor/glfw)

target_include_directories(SamuraiCiao PRIVATE vendor/glfw/include)
target_include_directories(SamuraiCiao PRIVATE vendor/stb)
target_include_directories(SamuraiCiao PRIVATE vendor/glm)
target_include_directories(SamuraiCiao PRIVATE vendor/assimp/include)
target_include_directories(SamuraiCiao PRIVATE vendor/imgui vendor/imgui/backends)
add_library(imgui STATIC 
    vendor/imgui/imgui.cpp
    vendor/imgui/imgui_draw.cpp
    vendor/imgui/imgui_tables.cpp
    vendor/imgui/imgui_widgets.cpp
    vendor/imgui/backends/imgui_impl_glfw.cpp
    vendor/imgui/backends/imgui_impl_opengl3.cpp
    vendor/imgui/imgui_demo.cpp
)
target_include_directories(imgui PRIVATE 
    vendor/glfw/include
    vendor/imgui
    vendor/imgui/backends)
target_link_directories(SamuraiCiao PRIVATE vendor/glfw/lib)
target_link_directories(SamuraiCiao PRIVATE vendor/assimp/lib)
target_link_libraries(SamuraiCiao glfw OpenGL::GL imgui assimp -fsanitize=address -fsanitize=undefined)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

