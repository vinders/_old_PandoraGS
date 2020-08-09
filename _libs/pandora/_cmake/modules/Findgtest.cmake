if(NOT TARGET gtest AND NOT IOS AND NOT ANDROID)
    set(gtest__FOUND ON)
    set(GTEST__PATH ${CWORK_SOLUTION_PATH}/_libs/gtest)

    # ┌──────────────────────────────────────────────────────────────────┐
    # │  Git submodules                                                  │
    # └──────────────────────────────────────────────────────────────────┘
    set(CWORK_SUBMODULE_PATH ${GTEST__PATH})
    include(${CMAKE_CURRENT_LIST_DIR}/git_submodules.cmake)
    unset(CWORK_SUBMODULE_PATH)

    # ┌──────────────────────────────────────────────────────────────────┐
    # │  Include project                                                 │
    # └──────────────────────────────────────────────────────────────────┘
    add_subdirectory(${GTEST__PATH} ${CMAKE_BINARY_DIR}/gtest)
    include_directories(${GTEST__PATH}/include ${GTEST__PATH}/src)
    
    if(TARGET gtest)
        set_target_properties(gtest PROPERTIES FOLDER "libs/test")
    endif()
    if(TARGET gtest_main)
        set_target_properties(gtest_main PROPERTIES FOLDER "libs/test")
    endif()
    if(TARGET gmock)
        set_target_properties(gmock PROPERTIES FOLDER "libs/test")
    endif()
    if(TARGET gmock_main)
        set_target_properties(gmock_main PROPERTIES FOLDER "libs/test")
    endif()
    
    set(gtest__LINKED gtest gtest_main)
    set(gmock__LINKED gmock gmock_main)
endif()
