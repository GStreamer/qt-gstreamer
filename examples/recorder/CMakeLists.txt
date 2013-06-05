project(qtgst-example-recorder)

if (NOT BUILDING_QTGSTREAMER)
    find_package(QtGStreamer REQUIRED)
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

include_directories(${QTGSTREAMER_INCLUDES})
add_definitions(${QTGSTREAMER_DEFINITIONS})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${QTGSTREAMER_FLAGS}")

qt_helper_wrap_ui(recorder_UI_SRCS recorder.ui)
add_executable(recorder main.cpp ${recorder_UI_SRCS})
target_link_libraries(recorder ${QTGSTREAMER_LIBRARIES} ${QT_QTGUI_LIBRARY})
