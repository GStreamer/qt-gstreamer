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
#include "value.h"
#include "string.h"
#include <glib-object.h>
#include <QtCore/QDebug>

namespace QGlib {

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


//BEGIN ValueImpl internal helpers

uint ValueImpl_Flags::get(const ValueBase & value)
{
    return g_value_get_flags(value);
}

void ValueImpl_Flags::set(ValueBase & value, uint data)
{
    g_value_set_flags(value, data);
}

int ValueImpl_Enum::get(const ValueBase & value)
{
    return g_value_get_enum(value);
}

void ValueImpl_Enum::set(ValueBase & value, int data)
{
    g_value_set_enum(value, data);
}

void *ValueImpl_Boxed::get(const ValueBase & value)
{
    return g_value_get_boxed(value);
}

void ValueImpl_Boxed::set(ValueBase & value, void *data)
{
    g_value_set_boxed(value, data);
}

//END ValueImpl internal helpers

}


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

#define SHORT_VALUEIMPL_IMPLEMENTATION(T, NICK) \
    QGLIB_REGISTER_VALUEIMPL_IMPLEMENTATION(T, g_value_get_##NICK(value), g_value_set_##NICK(value, data))

SHORT_VALUEIMPL_IMPLEMENTATION(bool, boolean)
SHORT_VALUEIMPL_IMPLEMENTATION(char, char)
SHORT_VALUEIMPL_IMPLEMENTATION(unsigned char, uchar)
SHORT_VALUEIMPL_IMPLEMENTATION(int, int)
SHORT_VALUEIMPL_IMPLEMENTATION(unsigned int, uint)
SHORT_VALUEIMPL_IMPLEMENTATION(long, long)
SHORT_VALUEIMPL_IMPLEMENTATION(unsigned long, ulong)
SHORT_VALUEIMPL_IMPLEMENTATION(qint64, int64)
SHORT_VALUEIMPL_IMPLEMENTATION(quint64, uint64)
SHORT_VALUEIMPL_IMPLEMENTATION(float, float)
SHORT_VALUEIMPL_IMPLEMENTATION(double, double)
SHORT_VALUEIMPL_IMPLEMENTATION(void*, pointer)
SHORT_VALUEIMPL_IMPLEMENTATION(QGlib::Type, gtype)
SHORT_VALUEIMPL_IMPLEMENTATION(const char*, string)
SHORT_VALUEIMPL_IMPLEMENTATION(QByteArray, string)

QGLIB_REGISTER_VALUEIMPL_IMPLEMENTATION(QString, QString::fromUtf8(g_value_get_string(value)),
                                        g_value_set_string(value, data.toUtf8().constData()))
