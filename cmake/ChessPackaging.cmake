# cmake/ChessPackaging.cmake
set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${PROJECT_NAME} - A Chess Engine With A Client And Server.")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_CONTACT "Tyler Renaud <trenaud_5298@email.ric.edu>, Thomas O'Neil <toneil_9771@email.ric.edu>")
set(CPACK_PACKAGE_HOMEPAGE_URL "${PROJECT_HOMEPAGE_URL}")

# Set packaging name to <name>-<version>-<system>
string(TOLOWER "${CMAKE_SYSTEM_NAME}" _cpack_system)
set(CPACK_PACKAGE_FILE_NAME "${PROJECT_NAME}-${PROJECT_VERSION}-${_cpack_system}")

# Add LICENSE and README
set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${PROJECT_SOURCE_DIR}/README.md")

# Shared Generator
set(CPACK_GENERATOR "ZIP")

# Windows Specific Logic
if(WIN32)
    # NSIS Installer Setup
    list(APPEND CPACK_GENERATOR "NSIS")
    set(CPACK_NSIS_DISPLAY_NAME "${PROJECT_NAME} ${PROJECT_VERSION}")
    set(CPACK_NSIS_CONTACT "${CPACK_PACKAGE_CONTACT}")
    set(CPACK_NSIS_MODIFY_PATH OFF)
    set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
    set(CPACK_NSIS_COMPONENT_INSTALL ON)

    # LICENSE Formatting Correction On Windows
    set(CPACK_RESOURCE_FILE_LICENSE ${PROJECT_SOURCE_DIR}/cmake/LICENSE.rtf)
endif()

# UNIX (Non Apple) Specific Logic
if(UNIX AND NOT APPLE)
    list(APPEND CPACK_GENERATOR "TGZ" "DEB")

    # Debian Installer Setup
    set(CPACK_DEB_COMPONENT_INSTALL ON)
    set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)

    # Client Package
    set(CPACK_DEBIAN_CHESS_CLIENT_COMPONENT_PACKAGE_NAME "${PROJECT_NAME}-client")
    set(CPACK_DEBIAN_CHESS_CLIENT_COMPONENT_PACKAGE_SECTION "games")
    set(CPACK_DEBIAN_CHESS_CLIENT_COMPONENT_PACKAGE_DEPENDS
        "${PROJECT_NAME}-core (>= ${PROJECT_VERSION}), ${PROJECT_NAME}-docs (>= ${PROJECT_VERSION})"
    )

    # Server Package
    set(CPACK_DEBIAN_CHESS_SERVER_COMPONENT_PACKAGE_NAME "${PROJECT_NAME}-server")
    set(CPACK_DEBIAN_CHESS_SERVER_COMPONENT_PACKAGE_SECTION "games")
    set(CPACK_DEBIAN_CHESS_SERVER_COMPONENT_PACKAGE_DEPENDS
        "${PROJECT_NAME}-core (>= ${PROJECT_VERSION}), ${PROJECT_NAME}-docs (>= ${PROJECT_VERSION})"
    )

    # Core Package
    set(CPACK_DEBIAN_CHESS_CORE_COMPONENT_PACKAGE_NAME "${PROJECT_NAME}-core")
    set(CPACK_DEBIAN_CHESS_CORE_COMPONENT_PACKAGE_SECTION "libs")

    # Docs Package
    set(CPACK_DEBIAN_CHESS_DOCS_COMPONENT_PACKAGE_NAME "${PROJECT_NAME}-docs")
    set(CPACK_DEBIAN_CHESS_DOCS_COMPONENT_PACKAGE_SECTION "doc")
endif()


#--------------------------------------------------------------------
# Component Setup
#--------------------------------------------------------------------
include(CPackComponent)

#--------------------------------------------------------------------
# Install Types
#--------------------------------------------------------------------
cpack_add_install_type(full_install DISPLAY_NAME "Full Install" DEFAULT)
cpack_add_install_type(client_install DISPLAY_NAME "Client Only")
cpack_add_install_type(server_install DISPLAY_NAME "Server Only")

#--------------------------------------------------------------------
# Runtime Components
#--------------------------------------------------------------------
cpack_add_component(chess_docs_component
    DISPLAY_NAME "Documentation"
    DESCRIPTION "Required Documentation Components For Chess Application Like LICENSE and README"
    REQUIRED
    HIDDEN
)

if(BUILD_SHARED_LIBS)
    cpack_add_component(chess_core_component
        DISPLAY_NAME "Chess Core Runtime"
        DESCRIPTION "Required Runtime For Chess Application"
        REQUIRED
    )
endif()

cpack_add_component(chess_client_component
    DISPLAY_NAME "Chess Client"
    DESCRIPTION "Chess Client Application"
    INSTALL_TYPES full_install client_install
)

cpack_add_component(chess_server_component
    DISPLAY_NAME "Chess Server"
    DESCRIPTION "Chess Server Application"
    INSTALL_TYPES full_install server_install
)

include(CPack)

