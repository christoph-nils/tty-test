cmake_minimum_required(VERSION 3.20)

find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    message(STATUS "Ccache found")
  else()
    message(STATUS "Ccache not found")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g3 -O0 -w")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g3 -O0 -w")

project(tty_test CXX)
file(GLOB_RECURSE sources src/*.c src/*.cpp)
add_executable(tty_test "")
target_sources(tty_test PRIVATE ${sources})

target_include_directories(tty_test PRIVATE src)

IF (WIN32)
  # Windows
  target_link_libraries(tty_test setupapi)
  if(MINGW)
    set(CMAKE_EXE_LINKER_FLAGS " -static")
    target_link_libraries(tty_test -static-libgcc -static-libstdc++)
  endif()
ELSE()
  # Linux
ENDIF()