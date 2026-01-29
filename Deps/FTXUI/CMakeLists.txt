# FTXUI/CmakeLists.txt
# Uses FetchContent To Make FTXUI Available

include(FetchContent)

FetchContent_Declare(ftxui
    GIT_REPOSITORY https://github.com/ArthurSonzogni/FTXUI
    GIT_TAG v6.1.9
)

FetchContent_MakeAvailable(ftxui)