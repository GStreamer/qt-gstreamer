/*
    Copyright (C) 2009-2010  George Kiagiadakis <kiagiadakis.george@gmail.com>

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
#ifndef QGLIB_GLOBAL_H
#define QGLIB_GLOBAL_H

/** \file
 This file directly or indirectly includes the following:
 - Glib/GObject forward declarations (gdeclarations.h)
 - QGlib forward declarations
 - The helper classes: Quark, Type, RefPointer, RefCountedObject
 - QtCore basics: QtGlobal / QString / QList (at least)
*/

#include "refpointer.h"
#include <QtCore/QList>

namespace QGlib {

class Value;
class ParamSpec;
typedef RefPointer<ParamSpec> ParamSpecPtr;
class Closure;
typedef RefPointer<Closure> ClosurePtr;
class Object;
typedef RefPointer<Object> ObjectPtr;


#define QGLIB_GTYPE_WRAPPER(GlibClass) \
    public: \
    typedef GlibClass CType; \
    static QGlib::Type type(); \
    private:

#define QGLIB_GTYPE_WRAPPER_IMPL(QGlibClass, GTYPE) \
    QGlib::Type QGlibClass::type() { return QGlib::Type(GTYPE); }


template <class T>
QList< RefPointer<T> > arrayToList(typename T::CType **array, uint n)
{
    QList< RefPointer<T> > result;
    for(uint i = 0; i<n; ++i) {
        result.append(RefPointer<T>::wrap(array[i]));
    }
    return result;
}

}

#endif
