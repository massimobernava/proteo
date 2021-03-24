# - Try to find EJDB
# Once done this will define
#
# EJDB_FOUND         - system has jsonc
# EJDB_INCLUDE_DIRS - the jsonc include directories
# EJDB_LIBRARIES    - jsonc libraries directories

if(EJDB_INCLUDE_DIRS AND EJDB_LIBRARIES)
set(EJDB_FIND_QUIETLY TRUE)
endif(EJDB_INCLUDE_DIRS AND EJDB_LIBRARIES)

find_path(EJDB_INCLUDE_DIR ejdb2.h
                           HINTS
                           /usr/include/ejdb2/
                           /usr/local/include/ejdb2/
			   )
find_library(EJDB_LIBRARY ejdb2
			  HINTS
			  /usr/lib/
                          /usr/local/lib
			  )

set(EJDB_INCLUDE_DIRS ${EJDB_INCLUDE_DIR})
set(EJDB_LIBRARIES ${EJDB_LIBRARY})

# handle the QUIETLY and REQUIRED arguments and set EJDB_FOUND to TRUE if
# all listed variables are TRUE
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(ejdb2 DEFAULT_MSG EJDB_INCLUDE_DIRS EJDB_LIBRARIES)

mark_as_advanced(EJDB_INCLUDE_DIRS EJDB_LIBRARIES)
