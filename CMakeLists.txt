#*******************************************************************************
# Author  :     Romain Vinders
# License :     GPLv2
#*******************************************************************************
cmake_minimum_required(VERSION 3.14)
include("${CMAKE_CURRENT_SOURCE_DIR}/_libs/pandora/_cmake/cwork.cmake")
cwork_read_version_from_file("${CMAKE_CURRENT_SOURCE_DIR}/build_version.txt" ON)
project("pandoraGS" VERSION ${CWORK_BUILD_VERSION} LANGUAGES C CXX)

# ┌──────────────────────────────────────────────────────────────────┐
# │  Solution settings                                               │
# └──────────────────────────────────────────────────────────────────┘
cwork_create_solution("${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/_libs/pandora/_cmake")

# ┌──────────────────────────────────────────────────────────────────┐
# │  Dependencies                                                    │
# └──────────────────────────────────────────────────────────────────┘
cwork_set_custom_libs("${CWORK_SOLUTION_PATH}/_libs" pandora
    system
    thread
    pattern
    memory
    time
    io
    hardware
    video
)

# ┌──────────────────────────────────────────────────────────────────┐
# │  Project settings                                                │
# └──────────────────────────────────────────────────────────────────┘
cwork_generate_version_header()
cwork_create_project("library" "${CMAKE_CURRENT_SOURCE_DIR}/_libs/pandora/_cmake" "${CMAKE_CURRENT_SOURCE_DIR}/_libs/pandora/_cmake/modules"
                     "include" "src" "test")
