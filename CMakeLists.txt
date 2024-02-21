cmake_minimum_required(VERSION 3.26)

project(GPU-accelerated-vector-graphics)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_OPENGL "GPU-accelerated-vector-graphics/vendor/OpenGL_startup_project/OpenGL_startup_project")


set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Add source files
file(GLOB_RECURSE SOURCES 
    "GPU-accelerated-vector-graphics/src/*.cpp"
)

# Add executable
add_executable(VectorGraphics ${SOURCES})

add_subdirectory(GPU-accelerated-vector-graphics/vendor/OpenGL_startup_project)
target_link_libraries(VectorGraphics glfw)

target_include_directories(VectorGraphics PRIVATE ${SOURCE_OPENGL})
target_include_directories(VectorGraphics PRIVATE ${SOURCE_OPENGL}/source)
target_link_libraries(VectorGraphics OpenGL_startup_project)

# glad
target_include_directories(VectorGraphics PRIVATE ${SOURCE_OPENGL}/vendor/glad/include)

# glm
target_include_directories(VectorGraphics PRIVATE ${SOURCE_OPENGL}/vendor/glm)

# stb
target_include_directories(VectorGraphics PRIVATE ${SOURCE_OPENGL}/vendor/stb)

# imgui
target_include_directories(VectorGraphics PRIVATE ${SOURCE_OPENGL}/vendor/imgui)


source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})