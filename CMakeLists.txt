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

add_custom_target(assets
    COMMAND ${CMAKE_SOURCE_DIR}/compile.sh ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR}
    COMMAND ${CMAKE_SOURCE_DIR}/copy_assets.sh ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR}
    COMMENT "Compiling shaders and copying assets"
)

add_dependencies(svke assets)

install(TARGETS svke)
