project(qtgst-example-voip)

if (NOT BUILDING_QTGSTREAMER)
    find_package(QtGStreamer REQUIRED)
    set(CMAKE_AUTOMOC ON)
endif()

include_directories(${QTGSTREAMER_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR})
add_definitions(${QTGSTREAMER_DEFINITIONS})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${QTGSTREAMER_FLAGS}")

qt4_wrap_ui(voip_UI_SRCS voip.ui)
add_executable(voip main.cpp ${voip_UI_SRCS})
target_link_libraries(voip ${QTGSTREAMER_UI_LIBRARIES})
