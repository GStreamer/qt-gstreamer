# This is a qmake project file, provided as an example on how to use qmake with QtGStreamer.
# There is nothing special about it, the only difference with a normal qmake project
# is that you specify INCLUDEPATH and LIBS to point to QtGStreamer. Unfortunately, qmake
# does not provide features like searching for libraries, so both the include path and the
# library path have to be typed manually here. It is recommended to use cmake instead,
# unless you have a reason to use qmake.

TEMPLATE = app
TARGET = player

# Change this to point to the location where the QtGStreamer headers are installed
INCLUDEPATH += /usr/local/include/QtGStreamer

# Change this to point to the location where the QtGStreamer library is installed
LIBS += -L/usr/local/lib -lQtGStreamer

# Recommended if you are using g++ 4.3 or later. Must be removed for other compilers.
QMAKE_CXXFLAGS += -std=c++0x

# Recommended, to avoid possible issues with the "emit" keyword
DEFINES += QT_NO_KEYWORDS

# This example has no GUI
QT -= gui

# Input
SOURCES += main.cpp
