set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(json json.cpp)
target_include_directories(json PUBLIC ..)

add_executable(feature-flags feature-flags.cpp)
target_include_directories(feature-flags PUBLIC ..)

add_executable(aggregation aggregation.cpp)
target_include_directories(aggregation PUBLIC ..)

add_executable(reduction reduction.cpp)
target_include_directories(reduction PUBLIC ..)

add_executable(2d-triemap 2d-triemap.cpp)
target_include_directories(2d-triemap PUBLIC ..)
