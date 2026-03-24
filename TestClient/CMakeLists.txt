# Chess::Client CMakeLists.txt File

#--------------------------------------------------------------------
# Client Setup
#--------------------------------------------------------------------
set(CHESS_TESTCLIENT_DIR "${CMAKE_CURRENT_LIST_DIR}")
set(CHESS_TESTCLIENT_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/src")
set(CHESS_TESTCLIENT_INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/include")

#--------------------------------------------------------------------
# Client Dependencies
#--------------------------------------------------------------------
if(NOT TARGET Chess::Core)
    message(FATAL_ERROR "Chess::Client Requires Chess:Core But Chess:Core Target Does Not Exist")
endif()

#--------------------------------------------------------------------
# Create Executable
#--------------------------------------------------------------------
add_executable(chess_testclient)

#--------------------------------------------------------------------
# Sources
#--------------------------------------------------------------------
# Grab Sources & Add Source Group
file(GLOB_RECURSE CHESS_TESTCLIENT_SOURCES
    CONFIGURE_DEPENDS "${CHESS_TESTCLIENT_SOURCE_DIR}/*.cpp"
)
source_group(TREE ${CHESS_TESTCLIENT_SOURCE_DIR} PREFIX "src" FILES ${CHESS_TESTCLIENT_SOURCES})

# Grab Headers & Add Source Group
file(GLOB_RECURSE CHESS_TESTCLIENT_HEADERS
    CONFIGURE_DEPENDS "${CHESS_TESTCLIENT_INCLUDE_DIR}/*.h"
)
source_group(TREE ${CHESS_TESTCLIENT_INCLUDE_DIR} PREFIX "include" FILES ${CHESS_TESTCLIENT_HEADERS})


# Add Sources To Target
target_sources(chess_testclient
    PRIVATE
        ${CHESS_TESTCLIENT_SOURCES}
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS
        ${CHESS_TESTCLIENT_INCLUDE_DIR}
    FILES
        ${CHESS_TESTCLIENT_HEADERS}
)

#--------------------------------------------------------------------
# Additional Target Configuration
#--------------------------------------------------------------------
target_link_libraries(chess_testclient
    PRIVATE
        Chess::Core
)

target_include_directories(chess_testclient
    PRIVATE
        ${CHESS_TESTCLIENT_SOURCE_DIR}
    PUBLIC
        $<INSTALL_INTERFACE:include>
)

set_target_properties(chess_testclient PROPERTIES
    # C++ Standard
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

#RPATH Handles Dynamics Libs On Linux/Mac (See HelperFunctions.cmake For Details)
if(BUILD_SHARED_LIBS)
    chess_set_runtime_paths(chess_testclient chess_core)
endif()