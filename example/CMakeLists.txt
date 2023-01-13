
file(GLOB_RECURSE SRCS ${CMAKE_CURRENT_LIST_DIR}/*.cpp)
foreach (filename IN LISTS SRCS)
    string(REGEX MATCH "[0-9a-zA-Z_]+\\.cpp" target ${filename})
    string(REGEX REPLACE ".cpp$" "" target ${target})
    add_executable(${target} ${filename})
    target_include_directories(${target} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
    target_link_libraries(${target} sagalite)
endforeach ()
