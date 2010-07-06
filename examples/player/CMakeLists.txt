include_directories(${CMAKE_CURRENT_BINARY_DIR}
                    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/)
add_definitions(-DQT_NO_KEYWORDS)
add_executable(player main.cpp)
target_link_libraries(player ${QT_QTCORE_LIBRARY} QtGstreamer)
