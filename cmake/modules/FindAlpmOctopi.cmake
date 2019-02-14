# - Try to find the alpm library
# Once done this will define

# use pkg-config to get the directories and then use these values
# in the FIND_PATH() and FIND_LIBRARY() calls

# Copyright (c) 2019, Angelo Scarnà, <angelo.scarna@codelinsoft.it>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.


find_package(PkgConfig)
pkg_check_modules(ALPMOCTOPI_LIB QUIET alpm)

set(ALPMOCTOPI_DEFINITIONS ${ALPMOCTOPI_LIB_CFLAGS_OTHER})

find_path(ALPMOCTOPI_INCLUDE_DIR
  NAMES alpm_octopi_utils.h
  HINTS ${ALPMOCTOPI_LIB_INCLUDEDIR}
  PATH_SUFFIXES alpm
)

find_library(ALPMOCTOPI_LIBRARY
  NAMES alpm_octopi_utils
  HINTS ${ALPMOCTOPI_LIB_LIBDIR}
)

set(ALPMOCTOPI_LIBRARIES ${ALPMOCTOPI_LIBRARY})

if (ALPMOCTOPI_INCLUDE_DIR AND ALPMOCTOPI_LIBRARIES)
  set(ALPMOCTOPI_FOUND TRUE)
else (ALPMOCTOPI_INCLUDE_DIR AND ALPMOCTOPI_LIBRARIES)
  set(ALPMOCTOPI_FOUND FALSE)
endif (ALPMOCTOPI_INCLUDE_DIR AND ALPMOCTOPI_LIBRARIES)
  
if (ALPMOCTOPI_FOUND)
    message(STATUS "AlpmOctopiUtils: library: ${ALPMOCTOPI_LIBRARIES}, include path: ${ALPMOCTOPI_INCLUDE_DIR}")
endif (ALPMOCTOPI_FOUND)
  
mark_as_advanced(ALPMOCTOPI_INCLUDE_DIR ALPMOCTOPI_LIBRARIES)
