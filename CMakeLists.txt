cmake_minimum_required(VERSION 3.23)

# ------------------------------------------------------------
# Project Configuration
# ------------------------------------------------------------
project(Chess
    VERSION 0.0.0
    DESCRIPTION "Chess.com The Sequal You Never Knew You Needed"
    HOMEPAGE_URL "https://github.com/trenaud5298/Chess.com"
    LANGUAGES CXX
)

# Is Chess Top Level Project (Prepared For Potential Future Use)
string(COMPARE EQUAL "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_SOURCE_DIR}" CHESS_STANDALONE)

# HelperFunctions.cmake File
include(${CMAKE_CURRENT_LIST_DIR}/cmake/HelperFunctions.cmake)

#--------------------------------------------------------------------
# Options
#--------------------------------------------------------------------
# Chess Build Options
option(BUILD_SHARED_LIBS "Build Shared Libs" OFF)

#--------------------------------------------------------------------
# Top-Level Install Setup (Subdirectories Handle Their Own Installation Rules)
#--------------------------------------------------------------------
include(GNUInstallDirs)

install(
    FILES
        ${PROJECT_SOURCE_DIR}/LICENSE
        ${PROJECT_SOURCE_DIR}/README.md
    DESTINATION ${CMAKE_INSTALL_DOCDIR}
    COMPONENT chess_docs_component
)

#--------------------------------------------------------------------
# Sub-Directories
#--------------------------------------------------------------------
# Dependencies
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/Deps/asio)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/Deps/FTXUI)

# Core Lib
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/Core)

# Client Executable
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/Client)

# Server Executable
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/Server)

#--------------------------------------------------------------------
# Packaging
#--------------------------------------------------------------------
# Packaging Rules
include(${CMAKE_CURRENT_LIST_DIR}/cmake/ChessPackaging.cmake)