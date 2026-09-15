cmake_minimum_required(VERSION 3.15)

project(vein_miner VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(ENDSTONE_API_VERSION "0.11" CACHE STRING "Endstone API version")

include(FetchContent)
FetchContent_Declare(
        endstone
        GIT_REPOSITORY https://github.com/EndstoneMC/endstone.git
        GIT_TAG v${ENDSTONE_API_VERSION}
)
FetchContent_MakeAvailable(endstone)

string(TOUPPER ${PROJECT_NAME} PROJECT_NAME_UPPER)
configure_file(include/version.h.in include/version.h)

endstone_add_plugin(veinminer src/vein_miner_plugin.cpp)
target_include_directories(veinminer PRIVATE include ${CMAKE_CURRENT_BINARY_DIR}/include)
