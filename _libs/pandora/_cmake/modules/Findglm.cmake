if(NOT TARGET glm)
    set(glm__FOUND ON)
    set(GLM__PATH ${CWORK_SOLUTION_PATH}/_libs/glm)

    # ┌──────────────────────────────────────────────────────────────────┐
    # │  Git submodules                                                  │
    # └──────────────────────────────────────────────────────────────────┘
    set(CWORK_SUBMODULE_PATH ${GLM__PATH})
    include(${CMAKE_CURRENT_LIST_DIR}/git_submodules.cmake)
    unset(CWORK_SUBMODULE_PATH)

    # ┌──────────────────────────────────────────────────────────────────┐
    # │  Include project                                                 │
    # └──────────────────────────────────────────────────────────────────┘
    #add_subdirectory(${GLM__PATH} ${CMAKE_BINARY_DIR}/glm)
    include_directories(${GLM__PATH} ${CMAKE_BINARY_DIR}/glm)
    
    if (TARGET glm)
        set_target_properties(glm PROPERTIES FOLDER libs)
    endif()
endif()
