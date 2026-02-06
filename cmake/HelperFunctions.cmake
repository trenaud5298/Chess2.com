# cmake/HelperFunctions.cmake
# Provides a few helper functions
# aimed to help with various settings
# and configurations in cmake

# chess_set_runtime_paths(target core_target)
# Sets the RPATH Variables For Executables
# On Linux/Mac To Bin Directory
function(chess_set_runtime_paths exe_target core_target)
    message(STATUS "chess_set_runtime_paths called for ${exe_target}")
    message(STATUS "BUILD_SHARED_LIBS = ${BUILD_SHARED_LIBS}")

    if(NOT TARGET ${exe_target})
        message(FATAL_ERROR "Target '${exe_target}' Does Not Exist, But chess_set_runtime_paths() Requires It Be A Target")
    endif()
    if(NOT TARGET ${core_target})
        message(FATAL_ERROR "Target '${core_target}' Does Not Exist, But chess_set_runtime_paths() Requires It Be A Target")
    endif()

    # Platform Specific
    if(APPLE)
        set(_origin "@loader_path")
    elseif(UNIX)
        set(_origin "$ORIGIN")
    else()
        set(_origin "")
    endif()

    if(_origin)
        # Compute Build Tree RPATH
        file(RELATIVE_PATH _relpath
            "$<TARGET_FILE_DIR:${exe_target}>"
            "$<TARGET_FILE_DIR:${core_target}>"
        )

        # Set Build & Install Time RPATH
        set_target_properties(${exe_target} PROPERTIES
            BUILD_RPATH "${_origin}/${_relpath}"
            BUILD_RPATH_USE_ORIGIN TRUE
            INSTALL_RPATH "${_origin}"
        )
    endif()

    # Build Time Dll Copying (Windows Only)
    if(WIN32)
        add_custom_command(TARGET ${exe_target} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_FILE:${core_target}>
                $<TARGET_FILE_DIR:${exe_target}>
            COMMENT "Copying ${core_target} Runtime Next To ${exe_target}"
        )
    endif()



endfunction()