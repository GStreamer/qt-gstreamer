project(QtGstreamer)
cmake_minimum_required(VERSION 2.6)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)
set(QT_DONT_USE_QTGUI 1)
set(QT_USE_QTTEST 1)

find_package(Qt4 REQUIRED)
find_package(Automoc4 REQUIRED)
find_package(Boost REQUIRED)
find_package(GStreamer REQUIRED)
find_package(GLIB2 REQUIRED)
find_package(GObject REQUIRED)
find_package(LibXml2 REQUIRED)

include(${QT_USE_FILE})
include_directories(${GSTREAMER_INCLUDE_DIR} ${GLIB2_INCLUDE_DIR}
                    ${LIBXML2_INCLUDE_DIR} ${Boost_INCLUDE_DIRS})

if (CMAKE_COMPILER_IS_GNUCXX)
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -Wnon-virtual-dtor -Wno-long-long -Wundef -Wcast-align -Wchar-subscripts -Wall -W -Wpointer-arith -Wformat-security -fno-check-new -fno-common")
endif (CMAKE_COMPILER_IS_GNUCXX)

add_subdirectory(src)
add_subdirectory(tests)
#add_subdirectory(examples)
