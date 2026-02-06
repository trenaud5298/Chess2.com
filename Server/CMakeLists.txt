# Chess::Server CMakeLists.txt File

#--------------------------------------------------------------------
# Server Setup
#--------------------------------------------------------------------
set(CHESS_SERVER_DIR "${CMAKE_CURRENT_LIST_DIR}")
set(CHESS_SERVER_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/src")
set(CHESS_SERVER_INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/include")

#--------------------------------------------------------------------
# Server Dependencies
#--------------------------------------------------------------------
if(NOT TARGET Chess::Core)
    message(FATAL_ERROR "Chess::Server Requires Chess:Core But Chess:Core Target Does Not Exist")
endif()

#--------------------------------------------------------------------
# Create Executable
#--------------------------------------------------------------------
add_executable(chess_server)

#--------------------------------------------------------------------
# Sources
#--------------------------------------------------------------------
# Grab Sources & Add Source Group
file(GLOB_RECURSE CHESS_SERVER_SOURCES
    CONFIGURE_DEPENDS "${CHESS_SERVER_SOURCE_DIR}/*.cpp"
)
source_group(TREE ${CHESS_SERVER_SOURCE_DIR} PREFIX "src" FILES ${CHESS_SERVER_SOURCES})

# Grab Headers & Add Source Group
file(GLOB_RECURSE CHESS_SERVER_HEADERS
    CONFIGURE_DEPENDS "${CHESS_SERVER_INCLUDE_DIR}/*.h"
)
source_group(TREE ${CHESS_SERVER_INCLUDE_DIR} PREFIX "include" FILES ${CHESS_SERVER_HEADERS})


# Add Sources To Target
target_sources(chess_server
    PRIVATE
        ${CHESS_SERVER_SOURCES}
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS
            ${CHESS_SERVER_INCLUDE_DIR}
        FILES
            ${CHESS_SERVER_HEADERS}
)

#--------------------------------------------------------------------
# Additional Target Configuration
#--------------------------------------------------------------------
target_link_libraries(chess_server
    PRIVATE
        Chess::Core
)

target_include_directories(chess_server
    PRIVATE
        ${CHESS_SERVER_SOURCE_DIR}
    PUBLIC
        $<INSTALL_INTERFACE:include>
)

set_target_properties(chess_server PROPERTIES
    # C++ Standard
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

#RPATH Handles Dynamics Libs On Linux/Mac (See HelperFunctions.cmake For Details)
if(BUILD_SHARED_LIBS)
    chess_set_runtime_paths(chess_server chess_core)
endif()
#--------------------------------------------------------------------
# Target Install
#--------------------------------------------------------------------
install(
    TARGETS chess_server
    RUNTIME
        DESTINATION ${CMAKE_INSTALL_BINDIR}
        COMPONENT chess_server_component
)