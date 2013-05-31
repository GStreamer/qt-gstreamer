# This script executes the compilation tests defined in CompilationTests.cmake

# Clean up and re-create a build directory for the tests project
execute_process(COMMAND ${CMAKE_COMMAND} -E remove_directory ${BINARY_DIR})
execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${BINARY_DIR})

# Copy the necessary files
execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${SOURCE_DIR}/CompilationTests_CMakeLists.txt
                                                 ${BINARY_DIR}/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${SOURCE_DIR}/CompilationTests.cmake
                                                 ${BINARY_DIR}/CompilationTests.cmake)

# Run cmake to execute the tests
execute_process(COMMAND ${CMAKE_COMMAND}
                    -G "${GENERATOR}"
                    -DQT_VERSION=${QT_VERSION}
                    -DQTGLIB_LIBRARY=${QTGLIB_LIBRARY}
                    -DQTGSTREAMER_LIBRARY=${QTGSTREAMER_LIBRARY}
                    -DQTGSTREAMER_UI_LIBRARY=${QTGSTREAMER_UI_LIBRARY}
                    -DQTGSTREAMER_UTILS_LIBRARY=${QTGSTREAMER_UTILS_LIBRARY}
                    -DQTGSTREAMER_INCLUDE_DIR=${QTGSTREAMER_INCLUDE_DIR}
                    -DCMAKE_MODULE_PATH=${CMAKE_MODULE_PATH}
                    -DQTGSTREAMER_STATIC=${QTGSTREAMER_STATIC}
                WORKING_DIRECTORY ${BINARY_DIR}
                OUTPUT_QUIET
                RESULT_VARIABLE TESTS_RESULT
)

if (NOT (${TESTS_RESULT} EQUAL 0))
    message(FATAL_ERROR "Compilation tests failed!")
endif()
