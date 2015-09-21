
# Custom CMake module for finding "TypographiaLib" files
# Written by Lukas Hermanns on 21/09/2015

# Macros

macro(_TYPOLIB_APPEND_LIBRARIES _list _release)
	set(_debug ${_release}_DEBUG)
	if(${_debug})
		set(${_list} ${${_list}} optimized ${${_release}} debug ${${_debug}})
	else()
		set(${_list} ${${_list}} ${${_release}})
	endif()
endmacro()

# Find library

find_path(TypoLib_INCLUDE_DIR NAMES Typo/Typo.h)

find_library(TypoLib_LIBRARY NAMES typolib)
find_library(TypoLib_LIBRARY_DEBUG NAMES typolibD)

# Setup package handle

include(FindPackageHandleStandardArgs)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(
	TypoLib
	DEFAULT_MSG
	TypoLib_INCLUDE_DIR
    TypoLib_LIBRARY
    TypoLib_LIBRARY_DEBUG
)

if(TYPOLIB_FOUND)
	set(TypoLib_FOUND TRUE)
	_TYPOLIB_APPEND_LIBRARIES(TypoLib_LIBRARIES TypoLib_LIBRARY)
endif(TYPOLIB_FOUND)
