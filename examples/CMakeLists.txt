macro(example_distcheck example)
    add_custom_target(example_${example}_distcheck
        ${CMAKE_COMMAND}
            -DCMAKE_BUILD_TOOL=${CMAKE_BUILD_TOOL}
            -DQT_QMAKE_EXECUTABLE=${QT_QMAKE_EXECUTABLE}
            -DQT_VERSION=${QT_VERSION}
            -DBINARY_DIR=${CMAKE_CURRENT_BINARY_DIR}
            -DSOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}
            -DEXAMPLE=${example}
            -P ${CMAKE_CURRENT_SOURCE_DIR}/RunExamplesDistCheck.cmake
    )

    get_target_property(EXAMPLES_DISTCHECK_TARGET examples_distcheck TYPE)
    if(NOT EXAMPLES_DISTCHECK_TARGET)
        add_custom_target(examples_distcheck)
    endif()

    add_dependencies(examples_distcheck example_${example}_distcheck)
endmacro()

add_subdirectory(player)
example_distcheck(player)

add_subdirectory(appsink-src)
example_distcheck(appsink-src)

add_subdirectory(recorder)
#example_distcheck(recorder)

add_subdirectory(voip)
example_distcheck(voip)

if (Qt4or5_Quick1_FOUND)
    add_subdirectory(qmlplayer)
    example_distcheck(qmlplayer)
endif()

if (Qt4or5_Quick2_FOUND)
    add_subdirectory(qmlplayer2)
    example_distcheck(qmlplayer2)
endif()
