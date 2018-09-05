cmake_minimum_required(VERSION 3.0)
project(UnityInputSystem)

set(UINPUT_ROOT ${CMAKE_SOURCE_DIR})

set(LIBRARY_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(LIBRARY_SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(SAMPLE_SRC_DIR ${CMAKE_SOURCE_DIR}/sample)

file(GLOB_RECURSE LIBRARY_SRC ${LIBRARY_SRC_DIR}/*.cpp ${UINPUT_ROOT}/include/*.h)
file(GLOB_RECURSE SAMPLE_SRC ${SAMPLE_SRC_DIR}/*.cpp)

message(STATUS SOMETHING SOMETHING)
message(STATUS ${LIBRARY_SRC_DIR})
message(STATUS DONE)

add_library(uinput SHARED ${LIBRARY_SRC})
add_executable(sample ${SAMPLE_SRC})

target_include_directories(uinput PUBLIC ${LIBRARY_INCLUDE_DIR})
target_include_directories(sample PUBLIC ${LIBRARY_INCLUDE_DIR})

# add the data to the target, so it becomes visible in some IDE

# this lets me include files relative to the root src dir with a <> pair
target_include_directories(sample PUBLIC include)

IF (UNIX)
    # find and target SDL
    target_link_libraries(sample PUBLIC
        uinput
    )
ENDIF()