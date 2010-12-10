# This is a qmake project file, provided as an example on how to use qmake with QtGStreamer.

TEMPLATE = app
TARGET = player

# Tell qmake to use pkg-config to find QtGStreamer.
CONFIG += link_pkgconfig

# Now tell qmake to link to QtGStreamer-0.10 and also use its include path and Cflags.
# Also add QtGStreamerUi-0.10 here if you are using classes from QGst::Ui
PKGCONFIG += QtGStreamer-0.10

# Recommended if you are using g++ 4.5 or later. Must be removed for other compilers.
#QMAKE_CXXFLAGS += -std=c++0x

# Recommended, to avoid possible issues with the "emit" keyword
# You can otherwise also define QT_NO_EMIT, but notice that this is not a documented Qt macro.
DEFINES += QT_NO_KEYWORDS

# This example has no GUI
QT -= gui

# Input
HEADERS += movieplayer.h player.h
SOURCES += main.cpp movieplayer.cpp player.cpp
