set(display_monitor_libs__FOUND ON)

if(NOT IOS AND NOT ANDROID)
    if(APPLE)
      #TODO

    elseif(WIN32 OR WIN64 OR _WIN32 OR _WIN64 OR CMAKE_SYSTEM_NAME STREQUAL "Windows")
        if(CWORK_WINDOWS_VERSION AND (CWORK_WINDOWS_VERSION STREQUAL "10" OR CWORK_WINDOWS_VERSION STREQUAL "8"))
            set(display_monitor_libs__LINKED user32 shcore)
        else()
            set(display_monitor_libs__LINKED user32)
        endif()   
        
    else()
        set(display_monitor_libs__LINKED X11)
    endif()
endif()
