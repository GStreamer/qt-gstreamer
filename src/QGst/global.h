/*
    Copyright (C) 2010  George Kiagiadakis <kiagiadakis.george@gmail.com>

    This library is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published
    by the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef QGST_GLOBAL_H
#define QGST_GLOBAL_H

#include "enums.h"

#define QGST_WRAPPER_DECLARATION(Class) \
    typedef struct _Gst##Class Gst##Class; \
    namespace QGst { \
        class Class; \
        typedef QGlib::RefPointer<Class> Class##Ptr; \
    }

QGST_WRAPPER_DECLARATION(Bin)
QGST_WRAPPER_DECLARATION(Bus)
QGST_WRAPPER_DECLARATION(Caps)
QGST_WRAPPER_DECLARATION(Element)
QGST_WRAPPER_DECLARATION(ElementFactory)
QGST_WRAPPER_DECLARATION(GhostPad)
QGST_WRAPPER_DECLARATION(Message)
QGST_WRAPPER_DECLARATION(MiniObject)
QGST_WRAPPER_DECLARATION(Object)
QGST_WRAPPER_DECLARATION(Pad)
QGST_WRAPPER_DECLARATION(Pipeline)
typedef struct _GstStructure GstStructure;

#undef QGST_WRAPPER_DECLARATION


#define QGST_WRAPPER(Class) \
    public: \
        typedef Gst##Class CType; \
    protected: \
        Class() {} \
        Class(const Class &); \
        Class & operator=(const Class &); \
        ~Class() {} \
        template <class T> friend class QGlib::RefPointer;

#endif
