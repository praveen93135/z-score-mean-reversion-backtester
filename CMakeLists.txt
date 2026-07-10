cmake_minimum_required(VERSION 3.16)

project(ZScoreMeanReversionBacktester VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(backtester
    src/main.cpp
    src/CsvReader.cpp
    src/Strategy.cpp
    src/Backtester.cpp
    src/Metrics.cpp
    src/ResultWriter.cpp
)

target_include_directories(backtester PRIVATE include)

if(MSVC)
    target_compile_options(backtester PRIVATE /W4)
else()
    target_compile_options(backtester PRIVATE -Wall -Wextra -pedantic)
endif()
