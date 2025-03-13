cmake_minimum_required(VERSION 3.10)
project(DeribitTradingSystem)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -O2")

# Find Boost
find_package(Boost REQUIRED COMPONENTS system thread)
include_directories(${Boost_INCLUDE_DIRS})

# Add sources
add_executable(trading_system src/main.cpp)

# Link libraries
target_link_libraries(trading_system PRIVATE Boost::system Boost::thread pthread ssl crypto)
