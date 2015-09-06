
# Custom CMake module for finding "FreeType" files
# Written by Lukas Hermanns on 06/09/2015

# Macros

macro(_FREETYPE_APPEND_LIBRARIES _list _release)
   set(_debug ${_release}_DEBUG)
   if(${_debug})
      set(${_list} ${${_list}} optimized ${${_release}} debug ${${_debug}})
   else()
      set(${_list} ${${_list}} ${${_release}})
   endif()
endmacro()

# Find library

find_path(FreeType_INCLUDE_DIR NAMES ft2build.h)

find_library(FreeType_LIBRARY NAMES freetype26)
find_library(FreeType_LIBRARY_DEBUG NAMES freetype26d)

# Setup package handle

include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(
	FreeType
	DEFAULT_MSG
	FreeType_INCLUDE_DIR
    FreeType_LIBRARY
    FreeType_LIBRARY_DEBUG
)

if(FREETYPE_FOUND)
	set(FreeType_FOUND TRUE)
	_FREETYPE_APPEND_LIBRARIES(FreeType_LIBRARIES FreeType_LIBRARY)
endif(FREETYPE_FOUND)
