cmake_minimum_required (VERSION 2.6)
project (checkit_tiff)
include_directories("${PROJECT_SOURCE_DIR}/headers")
include_directories("${PROJECT_SOURCE_DIR}/validate_icc")
include (CheckSymbolExists)

file( GLOB checkit_tiff_SOURCES 
  "${PROJECT_SOURCE_DIR}/tagrules/*.c"
  "${PROJECT_SOURCE_DIR}/ifdrules/*.c"
  "${PROJECT_SOURCE_DIR}/helper/*.c"
  "${PROJECT_SOURCE_DIR}/parser/*parser.c"
  "${PROJECT_SOURCE_DIR}/validate_icc/*.c"
  )
#
# Make a version file containing the current version from git.
#
execute_process( COMMAND git rev-list HEAD --count OUTPUT_VARIABLE REPO_REVISION OUTPUT_STRIP_TRAILING_WHITESPACE )
execute_process( COMMAND git rev-parse --abbrev-ref HEAD OUTPUT_VARIABLE BUILD_BRANCH OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process( COMMAND git rev-parse HEAD OUTPUT_VARIABLE BUILD_REV_ID OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process( COMMAND git describe --long --tags --dirty --always OUTPUT_VARIABLE BUILD_REV_ID_SHORT OUTPUT_STRIP_TRAILING_WHITESPACE)
add_definitions(-DVERSION="${BUILD_BRANCH}")
add_definitions(-DREPO_REVISION="${REPO_REVISION}")

if(WIN32)
  SET(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
  SET(BUILD_SHARED_LIBRARIES OFF)
  SET(CMAKE_EXE_LINKER_FLAGS "-static")
  add_definitions(-DPCRE_STATIC)
  add_definitions(-D_POSIX_C_SOURCE=200809L)
  # FIXME: needed for MingW only
  add_definitions(-D_GNU_SOURCE=1)
endif(WIN32)

add_executable(checkit_tiff checkit_tiff.c ${checkit_tiff_SOURCES})
add_executable(checkit_check_config checkit_check_config.c ${checkit_tiff_SOURCES})
set(CMAKE_EXTRA_INCLUDE_FILES tiff.h)


## libtiff5
find_package(TIFF REQUIRED)
if(TIFF_FOUND)
  # Confirm required API is available
  #INCLUDE(CheckFunctionExists)
  SET(CMAKE_REQUIRED_LIBRARIES ${TIFF_LIBRARIES})
  #CHECK_FUNCTION_EXISTS(TIFFOpen HAVE_TIFFOPEN)
  #IF(NOT HAVE_TIFFOPEN)
  #  SET(TIFF_FOUND) # ReSET to NOT found for TIFF library
  #  MESSAGE(FATAL_ERROR "Failed to link with libtiff - TIFFOpen function not found")
  #ENDIF()

  # CHECK_FUNCTION_EXISTS(TIFFMergeFieldInfo HAVE_TIFFMERGEFIELDINFO)
  #IF(NOT HAVE_TIFFMERGEFIELDINFO)
  #  SET(TIFF_FOUND) # ReSET to NOT found for TIFF library
  #  MESSAGE(FATAL_ERROR "Failed to link with libtiff - TIFFMergeFieldInfo function not found. libtiff 3.6.0 Beta or later required. Please upgrade or use an older version of libgeotiff")
  #ENDIF()
  INCLUDE_DIRECTORIES(${TIFF_INCLUDE_DIR})
  ADD_DEFINITIONS(-DHAVE_TIFF=1)

  CHECK_SYMBOL_EXISTS(TIFFTAG_IMAGELAYER "tiff.h" HAVE_TIFFTAG_IMAGE_LAYER)
  IF(NOT HAVE_TIFFTAG_IMAGE_LAYER)
    MESSAGE(STATUS "old tiff library detected, -DTIFFTAG_IMAGELAYER=34732 added")
    ADD_DEFINITIONS(-DTIFFTAG_IMAGELAYER=34732)
  ENDIF()

  CHECK_SYMBOL_EXISTS(TIFF_UINT64_T "tiff.h" HAVE_TIFF_UINT64_T)
  IF(NOT HAVE_TIFF_UINT64_T)
    MESSAGE(STATUS "old tiff library detected, -Duint64='unsigned long long' added")
    ADD_DEFINITIONS(-Duint64=unsigned\ long\ long)
  ENDIF()

  CHECK_SYMBOL_EXISTS(TIFF_INT64_T "tiff.h" HAVE_TIFF_INT64_T)
  IF(NOT HAVE_TIFF_INT64_T)
    MESSAGE(STATUS "old tiff library detected, -Dint64='long long' added")
    ADD_DEFINITIONS(-Dint64=long\ long)
  ENDIF()

else(TIFF_FOUND)
  MESSAGE(FATAL_ERROR "No TIFF library found, add $TIFF_INCLUDE_DIR manually")
endif(TIFF_FOUND)

#libpcre
#find_package(LIBPCRE REQUIRED)
FIND_PATH(PCRE_INCLUDE_DIR NAMES pcre.h)
FIND_LIBRARY(PCRE_LIBRARY NAMES pcre)
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(PCRE DEFAULT_MSG PCRE_LIBRARY PCRE_INCLUDE_DIR)
if (PCRE_FOUND)
  SET(PCRE_LIBRARIES ${PCRE_LIBRARY})
  SET(PCRE_INCLUDE_DIRS ${PCRE_INCLUDE_DIR})
  INCLUDE_DIRECTORIES(${PCRE_INCLUDE_DIR})
endif(PCRE_FOUND)


TARGET_LINK_LIBRARIES(checkit_tiff ${TIFF_LIBRARIES}  ${PCRE_LIBRARIES})
TARGET_LINK_LIBRARIES(checkit_check_config ${TIFF_LIBRARIES}  ${PCRE_LIBRARIES})

install( TARGETS checkit_tiff
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
)
install(DIRECTORY 
  ../example_configs
  ../tiffs_should_fail
  ../tiffs_should_pass 
  DESTINATION share/checkit_tiff/
)

file( GLOB README_FILES "../README*" "../FAQ" "../LICENSE" "../TODO" "../BUGS")
install(FILES ${README_FILES} DESTINATION share/checkit_tiff/)
