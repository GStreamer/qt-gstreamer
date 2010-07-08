include_directories(${CMAKE_CURRENT_BINARY_DIR}
                    ${CMAKE_CURRENT_SOURCE_DIR}/../src/)

add_definitions(-DQT_NO_KEYWORDS)

macro(qgst_test target)
    automoc4_add_executable(${target} "${target}.cpp")
    target_link_libraries(${target} ${QT_QTTEST_LIBRARY} ${GSTREAMER_LIBRARIES}
                                    ${GOBJECT_LIBRARIES} QtGstreamer)
    add_test(NAME ${target} COMMAND ${target})
endmacro(qgst_test)

qgst_test(refpointertest)
qgst_test(propertiestest)
qgst_test(signalstest)
qgst_test(valuetest)
qgst_test(structuretest)
qgst_test(capstest)
qgst_test(childproxytest)
qgst_test(structstest)
qgst_test(parsetest)
qgst_test(urihandlertest)
