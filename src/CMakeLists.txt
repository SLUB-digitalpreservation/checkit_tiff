cmake_minimum_required (VERSION 2.6)
project (checkit_tiff)
include_directories("${PROJECT_SOURCE_DIR}/headers")
include_directories("${PROJECT_SOURCE_DIR}/validate_icc")
include (CheckSymbolExists)
include (CheckFunctionExists)

file( GLOB checkit_tiff_SOURCES 
  "${PROJECT_SOURCE_DIR}/tagrules/*.c"
  "${PROJECT_SOURCE_DIR}/ifdrules/*.c"
  "${PROJECT_SOURCE_DIR}/messages/*.c"
  "${PROJECT_SOURCE_DIR}/helper/*.c"
  "${PROJECT_SOURCE_DIR}/helper/renderer/*.c"
  "${PROJECT_SOURCE_DIR}/parser/*parser.c"
  "${PROJECT_SOURCE_DIR}/validate_icc/*.c"
  "${PROJECT_SOURCE_DIR}/risk_analyzer/*.c"
  )

file( GLOB checkit_tiff_risk_SOURCES
  "${PROJECT_SOURCE_DIR}/helper/check_helper.c"
  "${PROJECT_SOURCE_DIR}/helper/check_print.c"
  "${PROJECT_SOURCE_DIR}/helper/check_tiffparse.c"
  "${PROJECT_SOURCE_DIR}/messages/*.c"
  "${PROJECT_SOURCE_DIR}/risk_analyzer/*.c"
)

#
# Make a version file containing the current version from git.
#
execute_process( COMMAND git rev-list HEAD --count OUTPUT_VARIABLE REPO_REVISION OUTPUT_STRIP_TRAILING_WHITESPACE )
execute_process( COMMAND git rev-parse --abbrev-ref HEAD OUTPUT_VARIABLE BUILD_BRANCH OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process( COMMAND git describe --tags OUTPUT_VARIABLE BUILD_TAG OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process( COMMAND git rev-parse HEAD OUTPUT_VARIABLE BUILD_REV_ID OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process( COMMAND git describe --long --tags --dirty --always OUTPUT_VARIABLE BUILD_REV_ID_SHORT OUTPUT_STRIP_TRAILING_WHITESPACE)
add_definitions(-DVERSION="${BUILD_BRANCH}_${BUILD_TAG}")
add_definitions(-DREPO_REVISION="${REPO_REVISION}")

# Check for compatibility with gnu11
# Compile with gnu11 support.
include(CheckCCompilerFlag)
CHECK_C_COMPILER_FLAG("-std=gnu11" COMPILER_SUPPORTS_GNU11)
if(COMPILER_SUPPORTS_GNU11)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu11")
else()
    message(STATUS "Your compiler (${CMAKE_C_COMPILER}) has no gnu11 support. If you are using clang on Apple OS X, please switch to gcc (version > 4.7) which is compliant with the C11 standard.")
endif()

if(WIN32)
  add_definitions(-D__WIN32__=1)
  SET(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
  SET(BUILD_SHARED_LIBRARIES OFF)
  # needed to increase default stack, because Win7 has only 1MB
  SET(CMAKE_EXE_LINKER_FLAGS "-static -Wl,--stack,20000000")
  add_definitions(-DPCRE_STATIC)
  add_definitions(-D_POSIX_C_SOURCE=200809L)
  # FIXME: needed for MingW only
  add_definitions(-D_GNU_SOURCE=1)
endif(WIN32)

add_executable(checkit_tiff checkit_tiff.c ${checkit_tiff_SOURCES})
add_executable(checkit_check_config checkit_check_config.c ${checkit_tiff_SOURCES})
add_executable(checkit_tiff_risk checkit_tiff_risk.c ${checkit_tiff_risk_SOURCES})
set(CMAKE_EXTRA_INCLUDE_FILES tiff.h)


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
else(PCRE_FOUND)
  MESSAGE(FATAL_ERROR "No PCRE library found, add $PCRE_INCLUDE_DIR manually")
endif(PCRE_FOUND)

#find mmap
CHECK_FUNCTION_EXISTS(mmap   _HAVE_MMAP)
CHECK_FUNCTION_EXISTS(munmap _HAVE_MUNMAP)
IF(_HAVE_MMAP AND _HAVE_MUNMAP)
  MESSAGE(INFO "mmap found")
  SET(_HAVE__MMAP 1)
ENDIF()



TARGET_LINK_LIBRARIES(checkit_tiff ${PCRE_LIBRARIES})
TARGET_LINK_LIBRARIES(checkit_check_config ${PCRE_LIBRARIES})
TARGET_LINK_LIBRARIES(checkit_tiff_risk)

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
