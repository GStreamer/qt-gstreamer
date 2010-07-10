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
#include <QtCore/QtGlobal>

#define QGST_WRAPPER_GSTCLASS_DECLARATION(Class) \
    typedef struct _Gst##Class Gst##Class;

#define QGST_WRAPPER_REFPOINTER_DECLARATION(Class) \
    namespace QGst { \
        class Class; \
        typedef QGlib::RefPointer<Class> Class##Ptr; \
    }

#define QGST_WRAPPER_DECLARATION(Class) \
    QGST_WRAPPER_GSTCLASS_DECLARATION(Class) \
    QGST_WRAPPER_REFPOINTER_DECLARATION(Class)

QGST_WRAPPER_DECLARATION(Bin)
QGST_WRAPPER_DECLARATION(Bus)
QGST_WRAPPER_DECLARATION(Caps)
QGST_WRAPPER_DECLARATION(ChildProxy)
QGST_WRAPPER_DECLARATION(Element)
QGST_WRAPPER_DECLARATION(ElementFactory)
QGST_WRAPPER_DECLARATION(GhostPad)
QGST_WRAPPER_DECLARATION(Message)
QGST_WRAPPER_DECLARATION(MiniObject)
QGST_WRAPPER_DECLARATION(Object)
QGST_WRAPPER_DECLARATION(Pad)
QGST_WRAPPER_DECLARATION(Pipeline)
QGST_WRAPPER_DECLARATION(PluginFeature)
QGST_WRAPPER_GSTCLASS_DECLARATION(Structure)
namespace QGst {
    class StructureBase;
    class Structure;
    class SharedStructure;
}
QGST_WRAPPER_GSTCLASS_DECLARATION(URIHandler)
QGST_WRAPPER_REFPOINTER_DECLARATION(UriHandler)
typedef struct _xmlNode* xmlNodePtr;

#undef QGST_WRAPPER_DECLARATION
#undef QGST_WRAPPER_REFPOINTER_DECLARATION
#undef QGST_WRAPPER_GSTCLASS_DECLARATION


#define QGST_WRAPPER_DIFFERENT_C_CLASS(Class, CClass) \
    public: \
        typedef Gst##CClass CType; \
    protected: \
        Class() {} \
        Class(const Class &); \
        Class & operator=(const Class &); \
        ~Class() {} \
        template <class T> friend class QGlib::RefPointer;

#define QGST_WRAPPER(Class) \
        QGST_WRAPPER_DIFFERENT_C_CLASS(Class, Class)

namespace QGst {
    typedef quint64 ClockTime;
    typedef qint64 ClockTimeDiff;
}

namespace QGst {
    void init();
    void init(int *argc, char **argv[]);
    void cleanup();
}

#endif
