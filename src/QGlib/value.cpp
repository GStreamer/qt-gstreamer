/*
    Copyright (C) 2009-2010  George Kiagiadakis <kiagiadakis.george@gmail.com>
    Copyright (C) 2010 Collabora Ltd.
      @author George Kiagiadakis <george.kiagiadakis@collabora.co.uk>

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
#include "value.h"
#include "string.h"
#include <glib-object.h>
#include <QtCore/QDebug>
#include <QtCore/QReadWriteLock>

namespace QGlib {
namespace Private {

class Dispatcher
{
public:
    Dispatcher();

    ValueVTable getVTable(Type t) const;
    void setVTable(Type t, const ValueVTable & vtable);

private:
    mutable QReadWriteLock lock;
    QHash<Type, ValueVTable> dispatchTable;
};

Dispatcher::Dispatcher()
{
#define DECLARE_VTABLE(T, NICK, GTYPE) \
    struct ValueVTable_##NICK \
    { \
        static void get(const ValueBase & value, void *data) \
        { \
            *reinterpret_cast<T*>(data) = g_value_get_##NICK(value); \
        }; \
        \
        static void set(ValueBase & value, const void *data) \
        { \
            g_value_set_##NICK(value, *reinterpret_cast<T const *>(data)); \
        }; \
    }; \
    setVTable(GTYPE, ValueVTable(ValueVTable_##NICK::set, ValueVTable_##NICK::get));

    DECLARE_VTABLE(char, char, Type::Char)
    DECLARE_VTABLE(unsigned char, uchar, Type::Uchar)
    DECLARE_VTABLE(bool, boolean, Type::Boolean)
    DECLARE_VTABLE(int, int, Type::Int)
    DECLARE_VTABLE(unsigned int, uint, Type::Uint)
    DECLARE_VTABLE(long, long, Type::Long)
    DECLARE_VTABLE(unsigned long, ulong, Type::Ulong)
    DECLARE_VTABLE(qint64, int64, Type::Int64)
    DECLARE_VTABLE(quint64, uint64, Type::Uint64)
    DECLARE_VTABLE(int, enum, Type::Enum);
    DECLARE_VTABLE(uint, flags, Type::Flags)
    DECLARE_VTABLE(float, float, Type::Float)
    DECLARE_VTABLE(double, double, Type::Double)
    DECLARE_VTABLE(QByteArray, string, Type::String)
    DECLARE_VTABLE(void*, pointer, Type::Pointer)
    DECLARE_VTABLE(void*, boxed, Type::Boxed)
    DECLARE_VTABLE(GParamSpec*, param, Type::Param)
    DECLARE_VTABLE(void*, object, Type::Object)
    DECLARE_VTABLE(QGlib::Type, gtype, GetType<QGlib::Type>())

#undef DECLARE_VTABLE
}

ValueVTable Dispatcher::getVTable(Type t) const
{
    QReadLocker l(&lock);

    if (dispatchTable.contains(t)) {
        return dispatchTable[t];
    }

    while (t.isDerived()) {
        t = t.parent();
        if (dispatchTable.contains(t)) {
            return dispatchTable[t];
        }
    }

    return ValueVTable();
}

void Dispatcher::setVTable(Type t, const ValueVTable & vtable)
{
    QWriteLocker l(&lock);
    dispatchTable[t] = vtable;
}

} //namespace Private

Q_GLOBAL_STATIC(Private::Dispatcher, s_dispatcher);

//BEGIN ValueBase

ValueBase::ValueBase(GValue* val)
    : m_value(val)
{
}

ValueBase::~ValueBase()
{
}

bool ValueBase::isValid() const
{
    return m_value != NULL;
}

void ValueBase::reset()
{
    if (m_value) {
        g_value_reset(m_value);
    }
}

Type ValueBase::type() const
{
    Q_ASSERT(isValid());
    return G_VALUE_TYPE(m_value);
}

bool ValueBase::canTransformTo(Type t) const
{
    return m_value ? g_value_type_transformable(type(), t) : false;
}

Value ValueBase::transformTo(Type t) const
{
    Q_ASSERT(isValid());
    Value dest;
    dest.init(t);
    g_value_transform(m_value, dest.m_value);
    return dest;
}

//static
void ValueBase::registerValueVTable(Type type, const ValueVTable & vtable)
{
    s_dispatcher()->setVTable(type, vtable);
}

void ValueBase::getData(Type dataType, void *data) const
{
    if (!isValid()) {
        throw InvalidValueException();
    } else if (g_value_type_compatible(type(), dataType)) {
        ValueVTable vtable = s_dispatcher()->getVTable(dataType);
        if (vtable.get != NULL) {
            vtable.get(*this, data);
        } else {
            throw std::logic_error("Unable to handle the given type. Type is not registered");
        }
    } else if (dataType.isValueType() && g_value_type_transformable(type(), dataType)) {
        Value v;
        v.init(dataType);

        if (!g_value_transform(m_value, v.m_value)) {
            throw InvalidTypeException();
        }

        try {
            v.getData(dataType, data);
        } catch (const InvalidTypeException &) {
            Q_ASSERT(false); //This must never happen
        }
    } else {
        throw InvalidTypeException();
    }
}

void ValueBase::setData(Type dataType, const void *data)
{
    if (!isValid()) {
        throw InvalidValueException();
    } else if (g_value_type_compatible(dataType, type())) {
        ValueVTable vtable = s_dispatcher()->getVTable(dataType);
        if (vtable.set != NULL) {
            vtable.set(*this, data);
        } else {
            throw std::logic_error("Unable to handle the given type. Type is not registered");
        }
    } else if (dataType.isValueType() && g_value_type_transformable(dataType, type())) {
        Value v;
        v.init(dataType);

        try {
            v.setData(dataType, data);
        } catch (const InvalidTypeException &) {
            Q_ASSERT(false); //This must never happen
        }

        if (!g_value_transform(v.m_value, m_value)) {
            throw InvalidTypeException();
        }
    } else {
        throw InvalidTypeException();
    }
}

//END ValueBase

//BEGIN Value

Value::Value()
    : ValueBase(NULL)
{
}

Value::Value(const GValue *gvalue)
    : ValueBase(NULL)
{
    if (gvalue) {
        init(G_VALUE_TYPE(gvalue));
        g_value_copy(gvalue, m_value);
    }
}

Value::Value(const SharedValue & other)
    : ValueBase(NULL)
{
    operator=(other);
}

Value::Value(const Value & other)
    : ValueBase(NULL)
{
    operator=(other);
}

Value::~Value()
{
    if (m_value) {
        g_value_unset(m_value);
        g_slice_free(GValue, m_value);
    }
}

Value & Value::operator=(const SharedValue & other)
{
    if (other.isValid()) {
        init(other.type());
        g_value_copy(other, m_value);
    } else if (m_value) {
        g_value_unset(m_value);
        g_slice_free(GValue, m_value);
        m_value = NULL;
    }
    return *this;
}

Value & Value::operator=(const Value & other)
{
    if (other.isValid()) {
        init(other.type());
        g_value_copy(other, m_value);
    } else if (m_value) {
        g_value_unset(m_value);
        g_slice_free(GValue, m_value);
        m_value = NULL;
    }
    return *this;
}

void Value::init(Type type)
{
    if (m_value) {
        g_value_unset(m_value);
    } else {
        m_value = g_slice_new0(GValue);
    }
    g_value_init(m_value, type);
}

//END Value

//BEGIN SharedValue

SharedValue::SharedValue(GValue *gvalue)
    : ValueBase(gvalue)
{
}

SharedValue::SharedValue(const SharedValue & other)
    : ValueBase(other.m_value)
{
}

SharedValue::~SharedValue()
{
}

SharedValue & SharedValue::operator=(const SharedValue & other)
{
    m_value = other.m_value;
    return *this;
}

//END SharedValue

} //namespace QGlib


QDebug & operator<<(QDebug debug, const QGlib::ValueBase & value)
{
    debug.nospace() << "QGlib::ValueBase";
    if(!value.isValid()) {
        debug << "(<invalid>)";
        return debug.space();
    } else {
        QString str;
        if (value.type().fundamental() == QGlib::Type::String) {
            str = value.get<QString>();
        } else if (value.canTransformTo(QGlib::Type::String)) {
            str = value.transformTo(QGlib::Type::String).get<QString>();
        } else if (g_value_fits_pointer(value)) {
            quintptr ptr = reinterpret_cast<quintptr>(g_value_peek_pointer(value));
            str = QString(QLatin1String("0x%1")).arg(ptr, sizeof(quintptr)*2, 16, QLatin1Char('0'));
        } else {
            str = QLatin1String("<unknown value>");
        }

        debug << "(" << value.type().name() << ", " << str << ")";
        return debug.space();
    }
}
