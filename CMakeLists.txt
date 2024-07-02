cmake_minimum_required(VERSION 3.22)
enable_language(CXX C)

# Require C++ 17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable -fPIC flag
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Enable IDE folders
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Force x86_64 architecture on MacOSX
if(APPLE)
    set(CMAKE_OSX_ARCHITECTURES "x86_64")

    # Disable deprecation warnings
    add_compile_options(
        -Wno-deprecated-declarations
    )
endif()

# Debug build is unsupported on MSVC
if(MSVC)
    list(REMOVE_ITEM CMAKE_CONFIGURATION_TYPES "Debug")
endif()

# Force old ABI for Linux, since Garry's Mod ABI is used
if(UNIX AND NOT APPLE)
    add_compile_definitions(_GLIBCXX_USE_CXX11_ABI=0)
endif()

add_subdirectory(cmake)

# Include garrysmod_common
find_garrysmod_common()
if(NOT GARRYSMOD_COMMON_FOUND) # Check if garrysmod_common has been found
    message(FATAL_ERROR "garrysmod_common not found")
endif()

file(STRINGS "VERSION" GMSV_COLORIZE_VERSION)
project(gmsv_colorize 
    VERSION ${GMSV_COLORIZE_VERSION}
    LANGUAGES CXX 
    HOMEPAGE_URL "https://github.com/Pika-Software/gmsv_colorize"
)

add_subdirectory(src)
