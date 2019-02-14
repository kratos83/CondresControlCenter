# - Try to find the alpm library
# Once done this will define

# use pkg-config to get the directories and then use these values
# in the FIND_PATH() and FIND_LIBRARY() calls

# Copyright (c) 2019, Angelo Scarnà, <angelo.scarna@codelinsoft.it>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.


find_package(PkgConfig)
pkg_check_modules(ALPM_LIB QUIET alpm)

set(ALPM_DEFINITIONS ${ALPM_LIB_CFLAGS_OTHER})

find_path(ALPM_INCLUDE_DIR
  NAMES alpm.h
  HINTS ${ALPM_LIB_INCLUDEDIR}
  PATH_SUFFIXES alpm
)

find_library(ALPM_LIBRARY
  NAMES alpm
  HINTS ${ALPM_LIB_LIBDIR}
)

set(ALPM_LIBRARIES ${ALPM_LIBRARY})

if (ALPM_INCLUDE_DIR AND ALPM_LIBRARIES)
  set(ALPM_FOUND TRUE)
else (ALPM_INCLUDE_DIR AND ALPM_LIBRARIES)
  set(ALPM_FOUND FALSE)
endif (ALPM_INCLUDE_DIR AND ALPM_LIBRARIES)
  
if (ALPM_FOUND)
    message(STATUS "Alpm: library: ${ALPM_LIBRARIES}, include path: ${ALPM_INCLUDE_DIR}")
endif (ALPM_FOUND)
  
mark_as_advanced(ALPM_INCLUDE_DIR ALPM_LIBRARIES)
