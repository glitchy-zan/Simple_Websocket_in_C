cmake_minimum_required(VERSION 3.10)

# Set the project name
project(SimpleWebSocket C)

include_directories(C:/MinGW/include)

link_directories(C:/MinGW/lib)

# search for .c files
aux_source_directory(client CLIENT_FILES)
aux_source_directory(server SERVER_FILES)
aux_source_directory(common COMMON_FILES)

# Create the client executable
add_executable(client_app ${CLIENT_FILES} ${COMMON_FILES})
# Create the server executable
add_executable(server_app ${SERVER_FILES} ${COMMON_FILES})

# On Windows, link against ws2_32 for Winsock
if(WIN32)
    target_link_libraries(client_app ws2_32)
    target_link_libraries(server_app ws2_32)
endif()
