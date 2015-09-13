
# === CMake lists for the TypographiaLib - (11/08/2015) ===

cmake_minimum_required(VERSION 2.8)
project(TypographiaLib)


# === Build path ===

set(OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/build)
set(EXECUTABLE_OUTPUT_PATH ${OUTPUT_DIR} CACHE PATH "Build directory" FORCE)
set(LIBRARY_OUTPUT_PATH ${OUTPUT_DIR} CACHE PATH "Build directory" FORCE)
set(PROJECT_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")
set(PROJECT_TEST_DIR "${PROJECT_SOURCE_DIR}/test")


# === Global files ===

file(GLOB_RECURSE HeadersAll "${PROJECT_SOURCE_DIR}/include/*.*")
file(GLOB_RECURSE SourcesAll "${PROJECT_SOURCE_DIR}/sources/*.*")

set(FilesAll ${HeadersAll} ${SourcesAll})


# === Include directories ===

include_directories("${PROJECT_INCLUDE_DIR}")


# === Binaries ===

add_library(typolib STATIC ${FilesAll})
set_target_properties(typolib PROPERTIES LINKER_LANGUAGE CXX DEBUG_POSTFIX "D")


# === Dependencies ===

# FreeType
include(FindFreeType.cmake)
if(FreeType_FOUND)
	message("found freetype library")
	include_directories(${FreeType_INCLUDE_DIR})
	target_link_libraries(typolib ${FreeType_LIBRARIES})
else(FreeType_FOUND)
	message("missing freetype library")
endif(FreeType_FOUND)


# === Test Projects ===

add_executable(test1 "${PROJECT_TEST_DIR}/test1.cpp")
set_target_properties(test1 PROPERTIES LINKER_LANGUAGE CXX DEBUG_POSTFIX "D")

target_link_libraries(test1 typolib)

find_package(OpenGL)
find_package(GLUT)
if(OpenGL_FOUND AND GLUT_FOUND)
	include_directories(${GLUT_INCLUDE_DIR})
	add_executable(test2 "${PROJECT_TEST_DIR}/test2.cpp")
	set_target_properties(test2 PROPERTIES LINKER_LANGUAGE CXX DEBUG_POSTFIX "D")
	target_link_libraries(test2 ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} typolib)
else()
	message("excluded optional project 'test2' (OpenGL and GLUT required)")
endif()


