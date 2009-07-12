add_executable(echo main.cpp)
target_link_libraries(echo ${QT_QTCORE_LIBRARY} QtGstreamer)
