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

#define FUNDAMENTAL_GETSET_IMPLEMENTATION(NICK, TYPE, GETFUNC, SETFUNC) \
    TYPE ValueBase::get ##NICK () const \
    { \
        Q_ASSERT(isValid()); \
        return GETFUNC(m_value); \
    } \
    void ValueBase::set ##NICK (TYPE data) \
    { \
        Q_ASSERT(isValid()); \
        SETFUNC(m_value, data); \
    }

FUNDAMENTAL_GETSET_IMPLEMENTATION(Bool, bool, g_value_get_boolean, g_value_set_boolean)
FUNDAMENTAL_GETSET_IMPLEMENTATION(Char, char, g_value_get_char, g_value_set_char)
FUNDAMENTAL_GETSET_IMPLEMENTATION(Uchar, uchar, g_value_get_uchar, g_value_set_uchar)
FUNDAMENTAL_GETSET_IMPLEMENTATION(Int, int, g_value_get_int, g_value_set_int)
FUNDAMENTAL_GETSET_IMPLEMENTATION(Uint, uint, g_value_get_uint, g_value_set_uint)
FUNDAMENTAL_GETSET_IMPLEMENTATION(Long, long, g_value_get_long, g_value_set_long)
FUNDAMENTAL_GETSET_IMPLEMENTATION(Ulong, ulong, g_value_get_ulong, g_value_set_ulong)
FUNDAMENTAL_GETSET_IMPLEMENTATION(Int64, qint64, g_value_get_int64, g_value_set_int64)
FUNDAMENTAL_GETSET_IMPLEMENTATION(Uint64, quint64, g_value_get_uint64, g_value_set_uint64)
FUNDAMENTAL_GETSET_IMPLEMENTATION(Float, float, g_value_get_float, g_value_set_float)
FUNDAMENTAL_GETSET_IMPLEMENTATION(Double, double, g_value_get_double, g_value_set_double)
FUNDAMENTAL_GETSET_IMPLEMENTATION(Enum, int, g_value_get_enum, g_value_set_enum)
FUNDAMENTAL_GETSET_IMPLEMENTATION(Flags, uint, g_value_get_flags, g_value_set_flags)
FUNDAMENTAL_GETSET_IMPLEMENTATION(Pointer, void*, g_value_get_pointer, g_value_set_pointer)
FUNDAMENTAL_GETSET_IMPLEMENTATION(Boxed, void*, g_value_get_boxed, g_value_set_boxed)
FUNDAMENTAL_GETSET_IMPLEMENTATION(ParamSpec, GParamSpec*, g_value_get_param, g_value_set_param)
FUNDAMENTAL_GETSET_IMPLEMENTATION(Object, void*, g_value_get_object, g_value_set_object)
FUNDAMENTAL_GETSET_IMPLEMENTATION(String, const char*, g_value_get_string, g_value_set_string)
FUNDAMENTAL_GETSET_IMPLEMENTATION(GType, ulong, g_value_get_gtype, g_value_set_gtype)

#undef FUNDAMENTAL_GETSET_IMPLEMENTATION

//END ValueBase

//BEGIN Value

Value::Value()
    : ValueBase(NULL)
{
}

Value::Value(const GValue & gvalue)
    : ValueBase(NULL)
{
    init(G_VALUE_TYPE(&gvalue));
    g_value_copy(&gvalue, m_value);
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
        g_value_copy(other.peekGValue(), m_value);
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
        g_value_copy(other.peekGValue(), m_value);
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
        } else if (g_value_fits_pointer(value.peekGValue())) {
            quintptr ptr = reinterpret_cast<quintptr>(g_value_peek_pointer(value.peekGValue()));
            str = QString(QLatin1String("0x%1")).arg(ptr, sizeof(quintptr)*2, 16, QLatin1Char('0'));
        } else {
            str = QLatin1String("<unknown value>");
        }

        debug << "(" << value.type().name() << ", " << str << ")";
        return debug.space();
    }
}
