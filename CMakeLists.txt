cmake_minimum_required(VERSION 2.6)

project(Nefarious-Overpowered-Tactical-Hot-Imperfect-Nail-Gun)

execute_process(COMMAND python2.7 gl3w_gen.py WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake_modules" ${CMAKE_MODULE_PATH})

set(HEADER_DIRECTORY "include")
set(SOURCE_DIRECTORY "source")
set(RESOURCES_DIRECTORY "resources")
set(SHADERS_DIRECTORY "shaders")

if (WIN32)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17 -DNOMINMAX")
else()
  set(CMAKE_CXX_STANDARD 17)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  set(CMAKE_CXX_EXTENSIONS OFF)
  set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -W -fdiagnostics-color=always -Wall -Wcast-align -Wcast-qual -Wconversion -Wctor-dtor-privacy -Wdisabled-optimization -Wduplicated-cond -Wextra -Wformat=2 -Winit-self -Wlogical-op -Wmissing-declarations -Wmissing-include-dirs -Wno-sign-conversion -Wnoexcept -Wnull-dereference -Wold-style-cast -Woverloaded-virtual -Wredundant-decls -Wstrict-aliasing=1 -Wstrict-null-sentinel -Wstrict-overflow=5 -Wswitch-default -Wno-unknown-pragmas -Wuseless-cast -Wno-parentheses -Wno-unused-local-typedefs")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g3")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
endif(WIN32)

message("Flags used: '${CMAKE_CXX_FLAGS}'")

file(GLOB_RECURSE SOURCES_FILE "${SOURCE_DIRECTORY}/*.cpp" "${SOURCE_DIRECTORY}/*.c")
file(GLOB_RECURSE HEADERS_FILE "${HEADER_DIRECTORY}/*.hpp" "${HEADER_DIRECTORY}/*.h")

add_executable(
  ${PROJECT_NAME}
  ${SOURCES_FILE}
  ${HEADERS_FILE}
)

find_package(OpenGL REQUIRED)
if (WIN32)
  include_directories (
    "C:/glfw-3.2.1.bin.WIN32/include"
    "C:/freetype-windows-binaries-2.7.1/include"
    "C:/SFML-2.4.2/include"
    "C:/include"
  )

else()
  find_package(PkgConfig REQUIRED)
  find_package(Freetype REQUIRED)
  find_package(PNG REQUIRED)
  pkg_search_module(GLFW REQUIRED glfw3)
endif(WIN32)

include_directories(
  ${GLFW_INCLUDE_DIRS}
  ${HEADER_DIRECTORY}
  ${FREETYPE_INCLUDE_DIRS}
  ${OPENGL_INCLUDE_DIRS}
  ${PNG_INCLUDE_DIRS}
)

if (WIN32)
  target_link_libraries(${PROJECT_NAME}
                        C:/glfw-3.2.1.bin.WIN32/lib-vc2015/glfw3.lib
                        C:/freetype-windows-binaries-2.7.1/win32/freetype271.lib
                        C:/SFML-2.4.2/lib/sfml-audio.lib
						C:/lib/libpng16.lib
						C:/lib/zlib.lib
                        opengl32.lib
  )
else()
  target_link_libraries(${PROJECT_NAME}
			${OPENGLES3_LIBRARY}
			${PNG_LIBRARIES}
			sfml-audio
			freetype
			pthread
  )
endif(WIN32)

target_link_libraries(${PROJECT_NAME} ${OPENGL_gl_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${GLFW_STATIC_LIBRARIES})
file(COPY ${SHADERS_DIRECTORY} DESTINATION .)
file(COPY ${RESOURCES_DIRECTORY} DESTINATION .)
