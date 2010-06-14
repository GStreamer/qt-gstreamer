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
#if !(defined(QGLIB_VALUEIMPL_H) && defined(QGLIB_VALUE_H))
# error "This header must not be included directly"
#endif

#include "assert.h"
#include "../type.h"
#include <string>

namespace QGlib {

//specialization for pointer types

template<typename T>
class ValueImpl<T*>
{
public:
    static T *get(const ValueBase & value);
    static void set(ValueBase & value, T *data);
};

template<typename T>
T *ValueImpl<T*>::get(const ValueBase & value)
{
    VALUE_ASSERT(value.type().fundamental() == Type::Pointer);
    return static_cast<T*>(value.getPointer());
}

template<typename T>
void ValueImpl<T*>::set(ValueBase & value, T *data)
{
    VALUE_ASSERT(value.type().fundamental() == Type::Pointer);
    value.setPointer(static_cast<void*>(data));
}

//specializations for native C++ types

#define QGLIB_NATIVE_TYPE_VALUEIMPL(NICK, TYPE, GTYPE) \
    template<> \
    class ValueImpl<TYPE> \
    { \
    public: \
        static TYPE get(const ValueBase & value) { \
            VALUE_ASSERT(value.type().fundamental() == GTYPE); \
            return value.get ##NICK (); \
        } \
        static void set(ValueBase & value, TYPE data) { \
            VALUE_ASSERT(value.type().fundamental() == GTYPE); \
            value.set ##NICK (data); \
        } \
    };

QGLIB_NATIVE_TYPE_VALUEIMPL(Bool, bool, Type::Boolean)
QGLIB_NATIVE_TYPE_VALUEIMPL(Char, char, Type::Char)
QGLIB_NATIVE_TYPE_VALUEIMPL(Uchar, unsigned char, Type::Uchar)
QGLIB_NATIVE_TYPE_VALUEIMPL(Int, int, Type::Int)
QGLIB_NATIVE_TYPE_VALUEIMPL(Uint, unsigned int, Type::Uint)
QGLIB_NATIVE_TYPE_VALUEIMPL(Long, long, Type::Long)
QGLIB_NATIVE_TYPE_VALUEIMPL(Ulong, unsigned long, Type::Ulong)
QGLIB_NATIVE_TYPE_VALUEIMPL(Int64, long long int, Type::Int64)
QGLIB_NATIVE_TYPE_VALUEIMPL(Uint64, unsigned long long int, Type::Uint64)
QGLIB_NATIVE_TYPE_VALUEIMPL(Float, float, Type::Float)
QGLIB_NATIVE_TYPE_VALUEIMPL(Double, double, Type::Double)
QGLIB_NATIVE_TYPE_VALUEIMPL(String, const char*, Type::String)

#undef QGLIB_NATIVE_TYPE_VALUEIMPL

//specializations for misc C++/Qt strings

template <>
class ValueImpl<std::string>
{
public:
    static std::string get(const ValueBase & value) {
        VALUE_ASSERT(value.type().fundamental() == Type::String);
        return std::string(value.getString());
    }

    static void set(ValueBase & value, const std::string & data) {
        VALUE_ASSERT(value.type().fundamental() == Type::String);
        value.setString(data.c_str());
    }
};

template <>
class ValueImpl<QString>
{
public:
    static QString get(const ValueBase & value) {
        VALUE_ASSERT(value.type().fundamental() == Type::String);
        return QString::fromUtf8(value.getString());
    }

    static void set(ValueBase & value, const QString & data) {
        VALUE_ASSERT(value.type().fundamental() == Type::String);
        value.setString(data.toUtf8().constData());
    }
};

template <>
class ValueImpl<QByteArray>
{
public:
    static QByteArray get(const ValueBase & value) {
        VALUE_ASSERT(value.type().fundamental() == Type::String);
        return value.getString();
    }

    static void set(ValueBase & value, const QByteArray & data) {
        VALUE_ASSERT(value.type().fundamental() == Type::String);
        value.setString(data.constData());
    }
};

template <>
class ValueImpl<QGlib::String>
{
public:
    static QGlib::String get(const ValueBase & value) {
        VALUE_ASSERT(value.type().fundamental() == Type::String);
        return value.getString();
    }

    static void set(ValueBase & value, const QGlib::String & data) {
        VALUE_ASSERT(value.type().fundamental() == Type::String);
        value.setString(data);
    }
};

}
