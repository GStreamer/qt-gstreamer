# - Try to find OpenGLES2
# Once done this will define
#
#  OPENGLES2_FOUND           - system has OpenGLES2
#  OPENGLES2_INCLUDE_DIR     - the GLES2 include directory
#  OPENGLES2_LIBRARY	    - the GLES2 library
#
# Copyright (c) 2012, George Kiagiadakis <kiagiadakis.george@gmail.com>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

find_path(OPENGLES2_INCLUDE_DIR GLES2/gl2.h)
find_library(OPENGLES2_LIBRARY NAMES GLESv2)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(OpenGLES2 DEFAULT_MSG OPENGLES2_INCLUDE_DIR OPENGLES2_LIBRARY)

mark_as_advanced(OPENGLES2_INCLUDE_DIR OPENGLES2_LIBRARY)

