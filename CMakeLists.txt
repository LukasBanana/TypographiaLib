
# === CMake lists for the TypographiaLib - (11/08/2015) ===

cmake_minimum_required(VERSION 2.8)
project(TypographiaLib)


# === Build path ===

set(OUTPUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/build)
set(EXECUTABLE_OUTPUT_PATH ${OUTPUT_DIR} CACHE PATH "Build directory" FORCE)
set(LIBRARY_OUTPUT_PATH ${OUTPUT_DIR} CACHE PATH "Build directory" FORCE)
set(PROJECT_INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")
set(PROJECT_TEST_DIR "${PROJECT_SOURCE_DIR}/test")


# === Options ===

option(TYPOLIB_BUILD_AS_STATIC_LIB "Builds the TypographiaLib as static library" ON)


# === Global files ===

file(GLOB Headers "${PROJECT_SOURCE_DIR}/include/Typo/*.*")
file(GLOB Sources "${PROJECT_SOURCE_DIR}/sources/*.*")

if(WIN32)
	file(GLOB SourcesPlatform "${PROJECT_SOURCE_DIR}/sources/Platform/Win32/*.*")
elseif(APPLE)
	file(GLOB SourcesPlatform "${PROJECT_SOURCE_DIR}/sources/Platform/MacOS/*.*")
elseif(UNIX)
	file(GLOB SourcesPlatform "${PROJECT_SOURCE_DIR}/sources/Platform/Linux/*.*")
endif()

set(FilesAll ${Headers} ${Sources} ${SourcesPlatform})


# === Include directories ===

include_directories("${PROJECT_INCLUDE_DIR}")


# === Source groups ===

source_group("Include\\Typo" FILES ${Headers})
source_group("Sources" FILES ${Sources})
source_group("Sources\\Platform" FILES ${SourcesPlatform})


# === Binaries ===

if(TYPOLIB_BUILD_AS_STATIC_LIB)
	add_library(typolib STATIC ${FilesAll})
else()
	add_library(typolib SHARED ${FilesAll})
endif()

set_target_properties(typolib PROPERTIES LINKER_LANGUAGE CXX DEBUG_POSTFIX "D")
target_compile_features(typolib PRIVATE cxx_range_for)


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
target_compile_features(test1 PRIVATE cxx_range_for)

find_package(OpenGL)
find_package(GLUT)
if(OpenGL_FOUND AND GLUT_FOUND)
	include_directories(${GLUT_INCLUDE_DIR})
	
	add_executable(test2 "${PROJECT_TEST_DIR}/test2.cpp")
	set_target_properties(test2 PROPERTIES LINKER_LANGUAGE CXX DEBUG_POSTFIX "D")
	target_link_libraries(test2 ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} typolib)
	target_compile_features(test2 PRIVATE cxx_range_for)
	
	add_executable(matrix_digital_rain "${PROJECT_TEST_DIR}/matrix_digital_rain.cpp")
	set_target_properties(matrix_digital_rain PROPERTIES LINKER_LANGUAGE CXX DEBUG_POSTFIX "D")
	target_link_libraries(matrix_digital_rain ${OPENGL_LIBRARIES} ${GLUT_LIBRARY} typolib)
	target_compile_features(matrix_digital_rain PRIVATE cxx_range_for)
else()
	message("excluded optional projects 'test2' and 'matrix_digital_rain' (OpenGL and GLUT required but not available)")
endif()


