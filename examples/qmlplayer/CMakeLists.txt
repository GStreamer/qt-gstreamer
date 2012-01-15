include_directories(${QTGSTREAMER_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${QTGSTREAMER_FLAGS}")

add_definitions(
    ${QTGSTREAMER_DEFINITIONS}
    -DCMAKE_CURRENT_SOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}"
    -DUNINSTALLED_IMPORTS_DIR="${CMAKE_BINARY_DIR}/src/qml"
)

if (${QT_QTOPENGL_FOUND})
    set(qmlplayer_gl_libs ${QT_QTOPENGL_LIBRARY})
else()
    add_definitions(-DQMLPLAYER_NO_OPENGL)
endif()

set(qmlplayer_SOURCES main.cpp player.cpp)
automoc4_add_executable(qmlplayer ${qmlplayer_SOURCES})
target_link_libraries(qmlplayer
    ${QTGSTREAMER_UI_LIBRARIES}
    ${QT_QTDECLARATIVE_LIBRARY}
    ${qmlplayer_gl_libs}
)
