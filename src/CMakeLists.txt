# Include every source file in source directory
file(GLOB_RECURSE SOURCES RELATIVE ${CMAKE_CURRENT_LIST_DIR} *.cpp *.hpp)
source_group(TREE ${CMAKE_CURRENT_LIST_DIR} PREFIX "Sources" FILES ${SOURCES})

add_library(colorize SHARED ${SOURCES})

target_link_libraries(colorize PRIVATE
    gmod::common
    gmod::detouring
    sourcesdk::common
    sourcesdk::tier0
    sourcesdk::tier1
)

target_include_directories(colorize PRIVATE ${CMAKE_CURRENT_LIST_DIR})

set_gmod_suffix_prefix(colorize)

# Autoinstall
set(AUTOINSTALL "" CACHE PATH "Autoinstall path")
if(IS_DIRECTORY ${AUTOINSTALL})
    autoinstall(colorize ${AUTOINSTALL})
endif()
