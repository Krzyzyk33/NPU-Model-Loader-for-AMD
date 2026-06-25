cmake_minimum_required(VERSION 3.15)
project(npu_chat CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include OGA headers
include_directories(${CMAKE_SOURCE_DIR}/include)

# Link directory for OGA lib + DLLs
link_directories(${CMAKE_SOURCE_DIR}/libs)

add_executable(npu_chat main.cpp)
target_link_libraries(npu_chat onnxruntime-genai ws2_32 crypt32)

# Copy all DLLs next to the executable after build
add_custom_command(TARGET npu_chat POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/libs"
    "$<TARGET_FILE_DIR:npu_chat>"
)
