set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(json json.cpp)
target_include_directories(json PUBLIC ../include)

add_executable(feature-flags feature-flags.cpp)
target_include_directories(feature-flags PUBLIC ../include)

add_executable(aggregation aggregation.cpp)
target_include_directories(aggregation PUBLIC ../include)

add_executable(reduction reduction.cpp)
target_include_directories(reduction PUBLIC ../include)
