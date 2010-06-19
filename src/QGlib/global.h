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

#include <QtCore/QtGlobal>
#include <boost/static_assert.hpp>

typedef struct _GValue GValue;
typedef struct _GParamSpec GParamSpec;
typedef struct _GClosure GClosure;
typedef struct _GObject GObject;

namespace QGlib {

class ValueBase;
class Value;
class SharedValue;
class String;
class Quark;
class Type;
template <class T> class RefPointer;
class ParamSpec;
typedef RefPointer<ParamSpec> ParamSpecPtr;
class Closure;
typedef RefPointer<Closure> ClosurePtr;
class Object;
typedef RefPointer<Object> ObjectPtr;

} //namespace QGlib

#define QGLIB_WRAPPER(Class) \
    public: \
        typedef G##Class CType; \
    protected: \
        Class() {} \
        Class(const Class &); \
        Class & operator=(const Class &); \
        ~Class() {} \
        template <class T> friend class QGlib::RefPointer;


#if defined(BOOST_HAS_STATIC_ASSERT) //we have c++0x static_assert
# define QGLIB_STATIC_ASSERT(expr, message) static_assert(expr, message)
# define QGLIB_HAVE_CXX0x_STATIC_ASSERT 1
#else
# define QGLIB_STATIC_ASSERT(expr, message) BOOST_STATIC_ASSERT(expr)
#endif

#endif
