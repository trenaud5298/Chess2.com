# tomlplusplus/CmakeLists.txt
# Uses FetchContent To Make toml++ Available

# Store Current Build Shared Libs Value
include(FetchContent)

FetchContent_Declare(tomlplusplus
    URL https://github.com/marzer/tomlplusplus/archive/refs/tags/v3.4.0.tar.gz
    URL_HASH SHA256=8517f65938a4faae9ccf8ebb36631a38c1cadfb5efa85d9a72e15b9e97d25155
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

FetchContent_MakeAvailable(tomlplusplus)

get_property(allTargets GLOBAL PROPERTY TARGETS)
message(STATUS "Targets: ${allTargets}")