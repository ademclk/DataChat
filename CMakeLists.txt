cmake_minimum_required(VERSION 3.10)
project(DataChat)

set(CMAKE_CXX_STANDARD 17)

# Add include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Add source files
set(COMMON_FILES
    ${CMAKE_SOURCE_DIR}/common/Message.cpp
    ${CMAKE_SOURCE_DIR}/common/Message.hpp
)

set(SERVER_FILES
    ${COMMON_FILES}
    ${CMAKE_SOURCE_DIR}/src/ServerSocket.cpp
    ${CMAKE_SOURCE_DIR}/src/ChatServer.cpp
    ${CMAKE_SOURCE_DIR}/src/ClientHandler.cpp
    ${CMAKE_SOURCE_DIR}/src/UserManager.cpp
    ${CMAKE_SOURCE_DIR}/src/SocketUtils.cpp
)

set(CLIENT_FILES
    ${COMMON_FILES}
    ${CMAKE_SOURCE_DIR}/src/ChatClient.cpp
    ${CMAKE_SOURCE_DIR}/src/SocketUtils.cpp
)

# Create a library for common files
add_library(common ${COMMON_FILES})

# Create executable for server
add_executable(server ${SERVER_FILES} ${CMAKE_SOURCE_DIR}/main_server.cpp)
target_link_libraries(server common)
target_compile_definitions(server PRIVATE SERVER)

# Create executable for client
add_executable(client ${CLIENT_FILES} ${CMAKE_SOURCE_DIR}/main_client.cpp)
target_link_libraries(client common)

# Link client with additional libraries (you may need to adjust this based on your system)
target_link_libraries(client pthread)
