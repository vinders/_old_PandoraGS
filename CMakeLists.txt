#*******************************************************************************
# Author  :     Romain Vinders
# License :     GPLv2
#*******************************************************************************
cmake_minimum_required(VERSION 3.14)
include("${CMAKE_CURRENT_SOURCE_DIR}/_libs/pandora_toolbox/_cmake/cwork.cmake")
cwork_read_version_from_file("${CMAKE_CURRENT_SOURCE_DIR}/build_version.txt" ON)
project("gpu_pandora_GS" VERSION ${CWORK_BUILD_VERSION} LANGUAGES C CXX)

# ┌──────────────────────────────────────────────────────────────────┐
# │  Solution settings                                               │
# └──────────────────────────────────────────────────────────────────┘
cwork_create_solution("${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/_libs/pandora_toolbox/_cmake")

# ┌──────────────────────────────────────────────────────────────────┐
# │  Toolbox dependencies                                            │
# └──────────────────────────────────────────────────────────────────┘
cwork_set_custom_libs("${CWORK_SOLUTION_PATH}/_libs" pandora_toolbox OFF
    system
    thread
    pattern
    memory
    logic
    time
    io
    hardware
    video
)

# ┌──────────────────────────────────────────────────────────────────┐
# │  Solution projects                                               │
# └──────────────────────────────────────────────────────────────────┘
cwork_set_solution_projects(
    config
    psx
)
