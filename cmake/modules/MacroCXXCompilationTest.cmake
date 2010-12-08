# - Perform a C++ compilation test. The test passes if the given code compiles
# or fails to compile, depending on the second argument given.
#
# CXX_COMPILATION_TEST(<name> <expect_fail> <source>)
#  <name>        - name of the test & name of the variable to store if the test passes
#  <expect_fail> - indicates whether the given C++ code should compile or not
#  <source>      - source code to try to compile
#
# The following variables may be set before calling this macro to
# modify the way the check is run:
#
#  CMAKE_REQUIRED_FLAGS = string of compile command line flags
#  CMAKE_REQUIRED_DEFINITIONS = list of macros to define (-DFOO=bar)
#  CMAKE_REQUIRED_INCLUDES = list of include directories
#  CMAKE_REQUIRED_LIBRARIES = list of libraries to link

# Based on CheckCXXSourceCompiles.cmake
#=============================================================================
# Copyright 2005-2009 Kitware, Inc.
# Copyright 2010 Collabora Ltd.
#   @author George Kiagiadakis <george.kiagiadakis@collabora.co.uk>
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# * Redistributions of source code must retain the above copyright
#   notice, this list of conditions and the following disclaimer.
#
# * Redistributions in binary form must reproduce the above copyright
#   notice, this list of conditions and the following disclaimer in the
#   documentation and/or other materials provided with the distribution.
#
# * Neither the names of Kitware, Inc., the Insight Software Consortium,
#   nor the names of their contributors may be used to endorse or promote
#   products derived from this software without specific prior written
#   permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#=============================================================================


set(CXX_COMPILATION_TESTS_SUCCEEDED "")
set(CXX_COMPILATION_TESTS_FAILED "")

macro(cxx_compilation_test _name _expect_fail _source)
  if("${_name}" MATCHES "^${_name}$")
    if ("${_expect_fail}" STREQUAL "SHOULD_FAIL")
        set(${_name}_SHOULD_FAIL 1)
    elseif("${_expect_fail}" STREQUAL "SHOULD_COMPILE")
        set(${_name}_SHOULD_FAIL 0)
    else()
        message(FATAL_ERROR "Invalid argument passed to cxx_compilation_test()")
    endif()

    if(CMAKE_REQUIRED_LIBRARIES)
      set(CXX_COMPILATION_TEST_ADD_LIBRARIES
        "-DLINK_LIBRARIES:STRING=${CMAKE_REQUIRED_LIBRARIES}")
    else()
      set(CXX_COMPILATION_TEST_ADD_LIBRARIES)
    endif()

    if(CMAKE_REQUIRED_INCLUDES)
      set(CXX_COMPILATION_TEST_ADD_INCLUDES
        "-DINCLUDE_DIRECTORIES:STRING=${CMAKE_REQUIRED_INCLUDES}")
    else()
      set(CXX_COMPILATION_TEST_ADD_INCLUDES)
    endif()

    file(WRITE "${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeTmp/src.cxx" "${_source}\n")

    try_compile(${_name}_COMPILES
      ${CMAKE_BINARY_DIR}
      ${CMAKE_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/CMakeTmp/src.cxx
      COMPILE_DEFINITIONS ${CMAKE_REQUIRED_DEFINITIONS}
      CMAKE_FLAGS
      "${CXX_COMPILATION_TEST_ADD_LIBRARIES}"
      "${CXX_COMPILATION_TEST_ADD_INCLUDES}"
      OUTPUT_VARIABLE OUTPUT)

    if((${_name}_COMPILES AND NOT ${_name}_SHOULD_FAIL) OR (NOT ${_name}_COMPILES AND ${_name}_SHOULD_FAIL))
      list(APPEND CXX_COMPILATION_TESTS_SUCCEEDED ${_name})
      message("PASS   : ${_name}")
      file(APPEND ${CMAKE_BINARY_DIR}/CompilationTests.log
        "C++ compilation test ${_name} succeded with the following output:\n"
        "${OUTPUT}\n"
        "Should fail: ${${_name}_SHOULD_FAIL}\n"
        "Source file was:\n${_source}\n")
    else()
      message("FAIL   : ${_name}")
      list(APPEND CXX_COMPILATION_TESTS_FAILED ${_name})
      set(CXX_COMPILATION_TESTS_HAVE_FAILURE 1)
      file(APPEND ${CMAKE_BINARY_DIR}/CompilationTests.log
        "C++ compilation test ${_name} failed with the following output:\n"
        "${OUTPUT}\n"
        "Should fail: ${${_name}_SHOULD_FAIL}\n"
        "Source file was:\n${_source}\n")
    endif()
  endif()
endmacro()

macro(evaluate_cxx_compilation_test_results)
    list(LENGTH CXX_COMPILATION_TESTS_SUCCEEDED CXX_COMPILATION_TESTS_SUCCEEDED_COUNT)
    list(LENGTH CXX_COMPILATION_TESTS_FAILED CXX_COMPILATION_TESTS_FAILED_COUNT)

    message("Totals: ${CXX_COMPILATION_TESTS_SUCCEEDED_COUNT} passed, ${CXX_COMPILATION_TESTS_FAILED_COUNT} failed")

    if (CXX_COMPILATION_TESTS_FAILED_COUNT GREATER 0)
        message(SEND_ERROR "Some tests have failed")
    endif()
endmacro()
