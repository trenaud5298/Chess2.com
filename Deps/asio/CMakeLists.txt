# Deps/asio CMakeLists.txt
#--------------------------------------------------------------------
# Asio Setup
#--------------------------------------------------------------------
set(ASIO_ROOT_DIR ${CMAKE_CURRENT_LIST_DIR})
set(ASIO_INCLUDE_DIR ${ASIO_ROOT_DIR}/include)

#--------------------------------------------------------------------
# Generate ASIO implementation TU
#--------------------------------------------------------------------
set(ASIO_IMPL_SRC "${CMAKE_CURRENT_BINARY_DIR}/asio_impl.cpp")
file(WRITE ${ASIO_IMPL_SRC}
    "#include <asio/impl/src.hpp>\n"
)

#--------------------------------------------------------------------
# Generate ASIO implementation TU
#--------------------------------------------------------------------
file(GLOB_RECURSE ASIO_HEADERS
    CONFIGURE_DEPENDS ${ASIO_INCLUDE_DIR}/*.hpp
)

# --------------------------------------------------------------------
# Create static library
# --------------------------------------------------------------------
add_library(asio STATIC)
add_library(asio::asio ALIAS asio)

#--------------------------------------------------------------------
# Target Configuration
#--------------------------------------------------------------------
target_sources(asio
    PRIVATE
        ${ASIO_IMPL_SRC}
    PUBLIC
        FILE_SET HEADERS
        BASE_DIRS
            ${ASIO_INCLUDE_DIR}
        FILES
            ${ASIO_HEADERS}
)

target_include_directories(asio
    PUBLIC
        $<INSTALL_INTERFACE:include>
)

target_compile_features(asio PUBLIC cxx_std_20)

target_compile_definitions(asio PUBLIC
    ASIO_STANDALONE
    ASIO_SEPARATE_COMPILATION
)

set_target_properties(asio PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)

# --------------------------------------------------------------------
# Platform-specific dependencies
# --------------------------------------------------------------------

if(WIN32)
    target_compile_definitions(asio PUBLIC
        _WIN32_WINNT=0x0601
    )
    target_link_libraries(asio PUBLIC
        ws2_32
        mswsock
        bcrypt
    )
endif()

#--------------------------------------------------------------------
# Target Install
#--------------------------------------------------------------------
# TODO: ADD Install Logic