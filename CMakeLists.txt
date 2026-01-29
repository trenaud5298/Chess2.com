cmake_minimum_required(VERSION 3.23)

# ------------------------------------------------------------
# Project Configuration
# ------------------------------------------------------------
project(Chess
    VERSION 0.1.0
    DESCRIPTION "Chess.com The Sequal You Never Knew You Needed"
    HOMEPAGE_URL "https://github.com/trenaud5298/Chess.com"
    LANGUAGES CXX
)


# Is Chess Top Level Project
string(COMPARE EQUAL "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_SOURCE_DIR}" CHESS2_STANDALONE)

#--------------------------------------------------------------------
# Options
#--------------------------------------------------------------------
# Chess Build Options
option(BUILD_SHARED_LIBS "Build Shared Libs" OFF)

# Chess General Options
option(CHESS_BUILD_EXAMPLES "Build the Chess example programs" ${CHESS_STANDALONE})
option(CHESS_BUILD_TESTS "Build the Chess test programs" ${CHESS_STANDALONE})
option(CHESS_BUILD_DOCS "Build the Chess documentation" ON)
option(CHESS_INSTALL "Generate installation target" ON)
option(CHESS_PACKAGING "Generate Packaging Through CPack" OFF)

# Chess Backend Options
option(CHESS_BUILD_CLIENT "Build the Chess client" ON)
option(CHESS_BUILD_SERVER "Build the Chess server" ON)

#--------------------------------------------------------------------
# Create generated files
#--------------------------------------------------------------------
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

set(CHESS_CONFIG_PATH "${CMAKE_INSTALL_LIBDIR}/cmake/Chess")

#configure_package_config_file(
#        CMake/ChessConfig.cmake.in
#        ${CMAKE_CURRENT_BINARY_DIR}/ChessConfig.cmake
#        INSTALL_DESTINATION "${CHESS_CONFIG_PATH}"
#)
#
#write_basic_package_version_file(
#        ${CMAKE_CURRENT_BINARY_DIR}/ChessConfigVersion.cmake
#        VERSION ${PROJECT_VERSION}
#        COMPATIBILITY SameMajorVersion
#)

#--------------------------------------------------------------------
# Sub-Directories
#--------------------------------------------------------------------
# Required Core Lib
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/Core)

# Required Networking Lib
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/Deps/asio)

# Required FTXUI Lib
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/Deps/FTXUI)

# Optional Components
if(CHESS_BUILD_CLIENT)
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/Client)
endif()

if(CHESS_BUILD_SERVER)
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/Server)
endif()


#--------------------------------------------------------------------
# Install
#--------------------------------------------------------------------
if(CHESS_INSTALL)
#    include(${CMAKE_CURRENT_LIST_DIR}/cmake/ChessInstall.cmake)
endif()

#--------------------------------------------------------------------
# Packaging
#--------------------------------------------------------------------
if(CHESS_PACKAGING)
#    include(${CMAKE_CURRENT_LIST_DIR}/cmake/ChessPackaging.cmake)
endif()