cmake_minimum_required(VERSION 3.10)

# Set the project name
project(chatapp)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find libcurl
#find_package(CURL REQUIRED)
INCLUDE_DIRECTORIES(${CURLCPP_SOURCE_DIR}/include)

# Add the executable
add_executable(chatapp main.cpp linenoise/linenoise.c) # Replace main.cpp with your source files

# Include directories
target_include_directories(chatapp PRIVATE ${CMAKE_SOURCE_DIR}/linenoise)
#-lcurl
target_link_libraries(chatapp PRIVATE curl)


# Link libraries
#target_link_libraries(MyCppProject PRIVATE ${CURL_LIBRARIES})

