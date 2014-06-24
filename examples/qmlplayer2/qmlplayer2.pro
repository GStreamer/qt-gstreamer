# This is a qmake project file, provided as an example on how to use qmake with QtGStreamer.

TEMPLATE = app
TARGET = qmlplayer2

# produce nice compilation output
CONFIG += silent

# Tell qmake to use pkg-config to find QtGStreamer.
CONFIG += link_pkgconfig

# Now tell qmake to link to QtGStreamer and also use its include path and Cflags.
PKGCONFIG += Qt5GStreamerQuick-1.0

# Recommended if you are using g++ 4.5 or later. Must be removed for other compilers.
#QMAKE_CXXFLAGS += -std=c++0x

# Recommended, to avoid possible issues with the "emit" keyword
# You can otherwise also define QT_NO_EMIT, but notice that this is not a documented Qt macro.
DEFINES += QT_NO_KEYWORDS

# link against QtDeclarative and QtOpenGL
QT += declarative opengl

# Input
HEADERS += player.h
SOURCES += main.cpp player.cpp
RESOURCES += qmlplayer2.qrc
