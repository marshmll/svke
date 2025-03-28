cmake_minimum_required(VERSION 3.21)
project(svke LANGUAGES CXX C)

add_executable(svke src/main.cpp)
add_subdirectory(src/)
add_subdirectory(externals/glfw)
add_subdirectory(externals/glm)

target_include_directories(svke PRIVATE
    include/
    externals/glfw
    externals/glm
    externals/VulkanMemoryAllocator
    externals/tinyobjloader
)

target_compile_features(svke PRIVATE cxx_std_17 c_std_99)

target_link_libraries(svke PRIVATE vulkan glfw glm shaderc)

add_custom_target(copy_assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
        "${CMAKE_SOURCE_DIR}/assets/"
        "${CMAKE_BINARY_DIR}/assets"
    COMMENT "Copying assets folder"
)

add_dependencies(svke copy_assets)

install(TARGETS svke)
