cmake_minimum_required(VERSION 3.8)
project(sagalite)

set(CMAKE_CXX_STANDARD 14)

find_package(Poco REQUIRED COMPONENTS Net)
find_package(Boost REQUIRED COMPONENTS system thread)

include_directories(include)
file(GLOB_RECURSE SRCS ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp)
add_library(${PROJECT_NAME} SHARED ${SRCS} src/transaction/Transaction.cpp)
target_include_directories(${PROJECT_NAME} INTERFACE $<INSTALL_INTERFACE:include>)
target_link_libraries(${PROJECT_NAME} INTERFACE $<INSTALL_INTERFACE:lib>)
target_link_libraries(${PROJECT_NAME} PRIVATE Poco::Net)

install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}_cmake)
install(DIRECTORY include/ TYPE INCLUDE)
install(EXPORT ${PROJECT_NAME}_cmake
        FILE ${PROJECT_NAME}-config.cmake
        DESTINATION lib/cmake/${PROJECT_NAME})

add_subdirectory(example)