# Chess::Client CMakeLists.txt File

#--------------------------------------------------------------------
# Client Setup
#--------------------------------------------------------------------
set(CHESS_CLIENT_DIR "${CMAKE_CURRENT_LIST_DIR}")
set(CHESS_CLIENT_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/src")
set(CHESS_CLIENT_INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/include")

#--------------------------------------------------------------------
# Client Dependencies
#--------------------------------------------------------------------
if(NOT TARGET Chess::Core)
    message(FATAL_ERROR "Chess::Client Requires Chess:Core But Chess:Core Target Does Not Exist")
endif()
if(NOT TARGET asio::asio)
    message(FATAL_ERROR "Chess::Client Requires asio::asio But asio::asio Target Does Not Exist")
endif()

#--------------------------------------------------------------------
# Create Executable
#--------------------------------------------------------------------
add_executable(chess_client)

#--------------------------------------------------------------------
# Sources
#--------------------------------------------------------------------
# Grab Sources & Add Source Group
file(GLOB_RECURSE CHESS_CLIENT_SOURCES
    CONFIGURE_DEPENDS "${CHESS_CLIENT_SOURCE_DIR}/*.cpp"
)
source_group(TREE ${CHESS_CLIENT_SOURCE_DIR} PREFIX "src" FILES ${CHESS_CLIENT_SOURCES})

# Grab Headers & Add Source Group
file(GLOB_RECURSE CHESS_CLIENT_HEADERS
    CONFIGURE_DEPENDS "${CHESS_CLIENT_INCLUDE_DIR}/*.h"
)
source_group(TREE ${CHESS_CLIENT_INCLUDE_DIR} PREFIX "include" FILES ${CHESS_CLIENT_HEADERS})


# Add Sources To Target
target_sources(chess_client
    PRIVATE
        ${CHESS_CLIENT_SOURCES}
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS
            ${CHESS_CLIENT_INCLUDE_DIR}
        FILES
            ${CHESS_CLIENT_HEADERS}
)

#--------------------------------------------------------------------
# Additional Target Configuration
#--------------------------------------------------------------------
target_link_libraries(chess_client
    PRIVATE
        Chess::Core
        asio::asio
)

target_include_directories(chess_client
    PRIVATE
        ${CHESS_CLIENT_SOURCE_DIR}
    PUBLIC
        $<INSTALL_INTERFACE:include>
)

set_target_properties(chess_client PROPERTIES
    # C++ Standard
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
)

#--------------------------------------------------------------------
# Target Install
#--------------------------------------------------------------------
install(
    TARGETS chess_client
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    COMPONENT chess_client_component
)