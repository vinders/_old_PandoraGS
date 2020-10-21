#*******************************************************************************
# Author  :     Romain Vinders
# License :     GPLv2
# ------------------------------------------------------------------------------
# PlayStation GPU emulation: status register, GPU commands, primitives...
#*******************************************************************************
cmake_minimum_required(VERSION 3.14)
include("${CMAKE_CURRENT_SOURCE_DIR}/../_libs/pandora_toolbox/_cmake/cwork.cmake")
cwork_set_default_solution("gpu_pandora_GS" "${CMAKE_CURRENT_SOURCE_DIR}/..")

cwork_read_version_from_file("${CMAKE_CURRENT_SOURCE_DIR}/../build_version.txt" OFF)
project("${CWORK_SOLUTION_NAME}.psx" VERSION ${CWORK_BUILD_VERSION} LANGUAGES C CXX)

# ┌──────────────────────────────────────────────────────────────────┐
# │  External dependencies                                           │
# └──────────────────────────────────────────────────────────────────┘
cwork_set_custom_libs("${CWORK_SOLUTION_PATH}/_libs" pandora_toolbox ON
    system
    thread
    memory
)

# ┌──────────────────────────────────────────────────────────────────┐
# │  Project settings                                                │
# └──────────────────────────────────────────────────────────────────┘
cwork_create_project("static" "${CWORK_SOLUTION_PATH}/_libs/pandora_toolbox/_cmake" 
                     "${CWORK_SOLUTION_PATH}/_libs/pandora_toolbox/_cmake/modules"
                     "include" "src" "test")
