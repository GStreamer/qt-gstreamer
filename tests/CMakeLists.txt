include_directories(${CMAKE_CURRENT_BINARY_DIR}
                    ${CMAKE_CURRENT_SOURCE_DIR}/../src/)

macro(qgst_test target)
    automoc4_add_executable(${ARGV})
    target_link_libraries(${target} ${QT_QTTEST_LIBRARY} ${GSTREAMER_LIBRARIES}
                                    ${GOBJECT_LIBRARIES} QtGstreamer)
endmacro(qgst_test)

qgst_test(refpointertest refpointertest.cpp)
qgst_test(propertiestest propertiestest.cpp)
qgst_test(signalstest signalstest.cpp)
qgst_test(valuetest valuetest.cpp)
qgst_test(structuretest structuretest.cpp)
