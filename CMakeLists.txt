project(QtGstreamer)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)
set(QT_DONT_USE_QTGUI 1)
set(QT_USE_QTTEST 1)

find_package(Qt4 REQUIRED)
find_package(Automoc4 REQUIRED)
find_package(GStreamer REQUIRED)
find_package(GLIB2 REQUIRED)
find_package(GObject REQUIRED)
find_package(LibXml2 REQUIRED)

include(CheckCXXSourceCompiles)
set(CMAKE_REQUIRED_INCLUDES ${QT_INCLUDES})
check_cxx_source_compiles("
#include <QtCore/QtGlobal>
int main()
{
#if defined(QT_NO_GLIB)
#error \"Qt was compiled with Glib disabled\"
#endif
return 0;
}"
QT4_GLIB_SUPPORT)

if (NOT QT4_GLIB_SUPPORT)
    message(STATUS "WARNING: Qt4 is compiled without glib support. libQtGstreamer will not work properly!")
endif (NOT QT4_GLIB_SUPPORT)

include(${QT_USE_FILE})

add_subdirectory(src)
add_subdirectory(tests)
add_subdirectory(examples)
