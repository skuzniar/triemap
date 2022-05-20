set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(basics basics.cpp)
target_include_directories(basics PUBLIC ../include)

add_executable(traversal traversal.cpp)
target_include_directories(traversal PUBLIC ../include)
