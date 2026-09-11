cmake_minimum_required(VERSION 3.20)

project(SH_mousecam_workspace VERSION 3.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz)
FetchContent_MakeAvailable(json)

FetchContent_Declare(
    minhook
    GIT_REPOSITORY https://github.com/TsudaKageyu/minhook.git
    GIT_TAG v1.3.4
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(minhook)

add_library(SH_mousecam SHARED src/dllmain.cpp)
set_target_properties(SH_mousecam PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/../sonicheroes.devicemod.mousecam"
)

target_link_libraries(SH_mousecam PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(SH_mousecam PRIVATE minhook)

set_target_properties(SH_mousecam PROPERTIES OUTPUT_NAME "SH_mousecam")

target_include_directories(SH_mousecam PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)