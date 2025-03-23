cmake_minimum_required(VERSION 3.21)
project(flare LANGUAGES CXX C)

add_executable(flare src/main.cpp)
add_subdirectory(src/)
add_subdirectory(externals/glfw)
add_subdirectory(externals/glm)

target_include_directories(flare PRIVATE
    include/
    externals/glfw
    externals/glm
    externals/VulkanMemoryAllocator
)

target_compile_features(flare PRIVATE cxx_std_17 c_std_99)

target_link_libraries(flare PRIVATE vulkan glfw glm shaderc)

add_custom_target(copy_assets
    COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
        "${CMAKE_SOURCE_DIR}/shaders/"
        "${CMAKE_BINARY_DIR}/shaders"
    COMMENT "Copying shaders folder"
)

add_dependencies(flare copy_assets)

install(TARGETS flare)
