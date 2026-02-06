# FTXUI/CmakeLists.txt
# Uses FetchContent To Make FTXUI Available

# Store Current Build Shared Libs Value
set(_OLD_BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS})
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

include(FetchContent)

set(FTXUI_ENABLE_INSTALL OFF CACHE BOOL "Disable FTXUI Install" FORCE)
FetchContent_Declare(ftxui
    URL https://github.com/ArthurSonzogni/FTXUI/archive/refs/tags/v6.1.9.tar.gz
    URL_HASH SHA256=45819c1e54914783d4a1ca5633885035d74146778a1f74e1213cdb7b76340e71
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

FetchContent_MakeAvailable(ftxui)

# Restore Build Shared Libs
set(BUILD_SHARED_LIBS ${_OLD_BUILD_SHARED_LIBS} CACHE BOOL "" FORCE)