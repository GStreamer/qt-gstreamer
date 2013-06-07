
function (run_check tool tool_name)

    execute_process(COMMAND ${CMAKE_COMMAND} -E remove_directory ${BINARY_DIR}/build-${EXAMPLE}-${tool_name})
    execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${BINARY_DIR}/build-${EXAMPLE}-${tool_name})

    if (${tool_name} STREQUAL "cmake")
        set(arguments "-DQT_VERSION=${QT_VERSION}")
    endif()

    execute_process(COMMAND ${tool} ${SOURCE_DIR}/${EXAMPLE} ${arguments}
                    WORKING_DIRECTORY ${BINARY_DIR}/build-${EXAMPLE}-${tool_name}
                    RESULT_VARIABLE ${tool_name}_RESULT
    )

    if (NOT (${${tool_name}_RESULT} EQUAL 0))
        execute_process(COMMAND ${CMAKE_COMMAND} -E remove_directory ${BINARY_DIR}/build-${EXAMPLE}-${tool_name})
        message(FATAL_ERROR "-- ${tool_name} failed --")
    else()
        execute_process(COMMAND ${CMAKE_BUILD_TOOL}
                        WORKING_DIRECTORY ${BINARY_DIR}/build-${EXAMPLE}-${tool_name}
                        RESULT_VARIABLE MAKE_RESULT
        )

        if (NOT (${MAKE_RESULT} EQUAL 0))
            execute_process(COMMAND ${CMAKE_COMMAND} -E remove_directory ${BINARY_DIR}/build-${EXAMPLE}-${tool_name})
            message(FATAL_ERROR "-- make (${tool_name}) failed --")
        endif()
    endif()

    execute_process(COMMAND ${CMAKE_COMMAND} -E remove_directory ${BINARY_DIR}/build-${EXAMPLE}-${tool_name})

endfunction()

message("##### Running CMAKE on ${EXAMPLE} example #####")
run_check(${CMAKE_COMMAND} cmake)

message("##### Running QMAKE on ${EXAMPLE} example #####")
run_check(${QT_QMAKE_EXECUTABLE} qmake)
