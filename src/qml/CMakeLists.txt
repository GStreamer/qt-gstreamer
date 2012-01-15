set(QtGStreamerQML_SRCS
    plugin.cpp
    videoitem.cpp
)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
automoc4_add_library(QtGStreamerQML MODULE ${QtGStreamerQML_SRCS})
target_link_libraries(QtGStreamerQML
    ${QTGSTREAMER_UI_LIBRARIES}
    ${QT_QTDECLARATIVE_LIBRARY}
)

install(TARGETS QtGStreamerQML DESTINATION ${IMPORTS_INSTALL_DIR}/QtGStreamer)
install(FILES qmldir DESTINATION ${IMPORTS_INSTALL_DIR}/QtGStreamer)

# create a layout similar to the one in ${IMPORTS_INSTALL_DIR} for testing
add_custom_command(TARGET QtGStreamerQML POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/QtGStreamer
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:QtGStreamerQML> ${CMAKE_CURRENT_BINARY_DIR}/QtGStreamer/
    COMMAND ${CMAKE_COMMAND} -E copy qmldir ${CMAKE_CURRENT_BINARY_DIR}/QtGStreamer/
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
