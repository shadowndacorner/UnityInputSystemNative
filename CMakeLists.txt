cmake_minimum_required(VERSION 3.0)
project(UnityInputSystem)

set (CMAKE_CXX_STANDARD 11)
set(UINPUT_ROOT ${CMAKE_SOURCE_DIR})

set(LIBRARY_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(LIBRARY_SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(SAMPLE_SRC_DIR ${CMAKE_SOURCE_DIR}/sample)

file(GLOB_RECURSE LIBRARY_SRC ${LIBRARY_SRC_DIR}/*.cpp ${UINPUT_ROOT}/include/*.h)
file(GLOB_RECURSE SAMPLE_SRC ${SAMPLE_SRC_DIR}/*.cpp)

add_library(uinput SHARED ${LIBRARY_SRC})
if (EMSCRIPTEN)
    message(status Skipping sample)
    # add the data to the target, so it becomes visible in some IDE
    target_include_directories(uinput PUBLIC ${LIBRARY_INCLUDE_DIR})
else()
    add_executable(sample ${SAMPLE_SRC})
    target_include_directories(sample PUBLIC ${LIBRARY_INCLUDE_DIR})
    target_include_directories(sample PUBLIC include)
    # this lets me include files relative to the root src dir with a <> pair
endif()

if (UNIX)
    target_compile_definitions(uinput PUBLIC UINPUT_LINUX)
endif()
if (APPLE)
    target_compile_definitions(uinput PUBLIC UINPUT_OSX)
endif()

IF (UNIX OR APPLE)
    # find and target SDL
    if (EMSCRIPTEN)
        add_definitions("-DEMSCRIPTEN -DUINPUT_WEB")
        set (CMAKE_CXX_FLAGS "-s USE_SDL=2")
    else()
        find_package(SDL2 REQUIRED)
        string(STRIP ${SDL2_LIBRARIES} SDL2_LIBRARIES)
        include_directories(${SDL2_INCLUDE_DIRS})
        target_link_libraries(uinput ${SDL2_LIBRARIES})
        
        target_link_libraries(sample PUBLIC
            uinput
        )
    endif()
ENDIF()