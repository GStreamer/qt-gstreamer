include_directories(${CMAKE_CURRENT_BINARY_DIR}
                    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/)

add_executable(echo main.cpp)
target_link_libraries(echo ${QT_QTCORE_LIBRARY} QtGstreamer)
