# - Try to find LibURCU
# Once done this will define
#  LIBURCU_FOUND - System has LibXml2
#  LIBURCU_INCLUDE_DIRS - The LibXml2 include directories
#  LIBURCU_LIBRARIES - The libraries needed to use LibXml2
#  LIBURCU_DEFINITIONS - Compiler switches required for using LibXml2

find_path(LIBURCU_ROOT_DIR NAMES include/urcu.h)
set(LIBURCU_NAME urcu)
find_library(LIBURCU_LIBRARIES NAMES ${LIBURCU_NAME} HINTS ${LIBURCU_ROOT_DIR}/lib)
find_path(LIBURCU_INCLUDE_DIRS NAMES urcu.h HINTS ${LIBURCU_ROOT_DIR}/include)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(liburcu REQUIRED_VARS LIBURCU_LIBRARIES LIBURCU_INCLUDE_DIRS)

mark_as_advanced(
  LIBURCU_ROOT_DIR
  LIBURCU_LIBRARIES
  LIBURCU_INCLUDE_DIRS)
