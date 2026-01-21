# Chess::Core CMakeLists.txt File

#--------------------------------------------------------------------
# Core Setup
#--------------------------------------------------------------------
set(CHESS_CORE_DIR "${CMAKE_CURRENT_LIST_DIR}")
set(CHESS_CORE_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/src")
set(CHESS_CORE_INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/include")

#--------------------------------------------------------------------
# Core Dependencies
#--------------------------------------------------------------------
# No Explicit Dependencies

#--------------------------------------------------------------------
# File Setup
#--------------------------------------------------------------------
# Generated header setup
include(GenerateExportHeader)
set(_chess_core_generated_directory "${CMAKE_CURRENT_BINARY_DIR}/generated")
set(_chess_core_generated_export_header "${CMAKE_CURRENT_BINARY_DIR}/generated/Chess/Core/Export.h")

# Grab Sources & Add Source Group
file(GLOB_RECURSE CHESS_CORE_SOURCES
        CONFIGURE_DEPENDS "${CHESS_CORE_SOURCE_DIR}/*.cpp"
)
source_group(TREE ${CHESS_CORE_SOURCE_DIR} PREFIX "src" FILES ${CHESS_CORE_SOURCES})

# Grab Headers & Add Source Group
file(GLOB_RECURSE CHESS_CORE_HEADERS
        CONFIGURE_DEPENDS "${CHESS_CORE_INCLUDE_DIR}/*.h"
)
source_group(TREE ${CHESS_CORE_INCLUDE_DIR} PREFIX "include" FILES ${CHESS_CORE_HEADERS})

# Add Generated Header To Header List & Add To Source Group
list(APPEND CHESS_CORE_HEADERS ${_chess_core_generated_export_header})
source_group("include/Chess/Core" FILES ${_chess_core_generated_export_header})

#--------------------------------------------------------------------
# Create Library
#--------------------------------------------------------------------
set(CHESS_CORE_TARGETS)

add_library(chess_core)
add_library(Chess::Core ALIAS chess_core)
set_target_properties(chess_core PROPERTIES EXPORT_NAME Core)
#--------------------------------------------------------------------
# Export Header
#--------------------------------------------------------------------
generate_export_header(chess_core
    BASE_NAME CHESS_CORE
    EXPORT_FILE_NAME ${_chess_core_generated_export_header}
)

#--------------------------------------------------------------------
# Target Configuration
#--------------------------------------------------------------------
target_sources(chess_core
    PRIVATE
        ${CHESS_CORE_SOURCES}
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS
            ${CHESS_CORE_INCLUDE_DIR}
            ${_chess_core_generated_directory}
        FILES
            ${CHESS_CORE_HEADERS}
)

target_include_directories(chess_core
    PRIVATE
        ${CHESS_CORE_SOURCE_DIR}
    PUBLIC
        $<INSTALL_INTERFACE:include>
)

set_target_properties(chess_core PROPERTIES
    # C++ Standard
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
    # Compiler Options
    POSITION_INDEPENDENT_CODE ON
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN YES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
)

target_compile_features(chess_core PUBLIC cxx_std_20)

#--------------------------------------------------------------------
# Target Install
#--------------------------------------------------------------------
install(TARGETS chess_core
    EXPORT ChessCoreTargets
    RUNTIME
        DESTINATION ${CMAKE_INSTALL_BINDIR}
        COMPONENT chess_core_component
    LIBRARY
        DESTINATION ${CMAKE_INSTALL_LIBDIR}
        COMPONENT chess_core_component
    ARCHIVE
        DESTINATION ${CMAKE_INSTALL_LIBDIR}
        COMPONENT chess_core_component
    FILE_SET HEADERS
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
        COMPONENT chess_core_component
)

