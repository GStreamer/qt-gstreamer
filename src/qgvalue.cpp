/*
    Copyright (C) 2009  George Kiagiadakis <kiagiadakis.george@gmail.com>

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
#include "qgvalue.h"
#include "qgstelement.h"
#include "qgstpad.h"
#include "qgstcaps.h"
#include <QtCore/QDebug>
#include <gst/gst.h>

namespace QtGstreamer {

QGValue::QGValue()
{
    m_value = NULL;
}

QGValue::QGValue(char val)
{
    m_value = g_slice_new0(GValue);
    g_value_init(m_value, G_TYPE_CHAR);
    g_value_set_char(m_value, val);
}

QGValue::QGValue(uchar val)
{
    m_value = g_slice_new0(GValue);
    g_value_init(m_value, G_TYPE_UCHAR);
    g_value_set_uchar(m_value, val);
}

QGValue::QGValue(int val)
{
    m_value = g_slice_new0(GValue);
    g_value_init(m_value, G_TYPE_INT);
    g_value_set_int(m_value, val);
}

QGValue::QGValue(uint val)
{
    m_value = g_slice_new0(GValue);
    g_value_init(m_value, G_TYPE_UINT);
    g_value_set_uint(m_value, val);
}

QGValue::QGValue(qint64 val)
{
    m_value = g_slice_new0(GValue);
    g_value_init(m_value, G_TYPE_INT64);
    g_value_set_int64(m_value, val);
}

QGValue::QGValue(quint64 val)
{
    m_value = g_slice_new0(GValue);
    g_value_init(m_value, G_TYPE_UINT64);
    g_value_set_uint64(m_value, val);
}

QGValue::QGValue(bool val)
{
    m_value = g_slice_new0(GValue);
    g_value_init(m_value, G_TYPE_BOOLEAN);
    g_value_set_boolean(m_value, val);
}

QGValue::QGValue(double val)
{
    m_value = g_slice_new0(GValue);
    g_value_init(m_value, G_TYPE_DOUBLE);
    g_value_set_double(m_value, val);
}

QGValue::QGValue(const char *val)
{
    m_value = g_slice_new0(GValue);
    g_value_init(m_value, G_TYPE_STRING);
    g_value_set_string(m_value, val);
}

QGValue::QGValue(const QByteArray & val)
{
    m_value = g_slice_new0(GValue);
    g_value_init(m_value, G_TYPE_STRING);
    g_value_set_string(m_value, val.constData());
}

QGValue::QGValue(const QGstObjectPtr & val)
{
    m_value = g_slice_new0(GValue);
    g_value_init(m_value, GST_TYPE_OBJECT);
    g_value_set_object(m_value, val->m_object);
}

QGValue::QGValue(const QGstElementPtr & val)
{
    m_value = g_slice_new0(GValue);
    g_value_init(m_value, GST_TYPE_ELEMENT);
    g_value_set_object(m_value, val->m_object);
}

QGValue::QGValue(const QGstPadPtr & val)
{
    m_value = g_slice_new0(GValue);
    g_value_init(m_value, GST_TYPE_PAD);
    g_value_set_object(m_value, val->m_object);
}

QGValue::QGValue(const QGstCapsPtr & val)
{
    m_value = g_slice_new0(GValue);
    g_value_init(m_value, GST_TYPE_CAPS);
    g_value_set_boxed(m_value, val->m_caps);
}

QGValue::QGValue(const QGValue & other)
{
    m_value = g_slice_new0(GValue);
    g_value_init(m_value, G_VALUE_TYPE(other.m_value));
    g_value_copy(other.m_value, m_value);
}

QGValue::~QGValue()
{
    if ( m_value ) {
        g_value_unset(m_value);
        g_slice_free(GValue, m_value);
    }
}

QGValue & QGValue::operator=(const QGValue & other)
{
    if ( m_value ) {
        g_value_unset(m_value);
    } else {
        m_value = g_slice_new0(GValue);
    }
    g_value_init(m_value, G_VALUE_TYPE(other.m_value));
    g_value_copy(other.m_value, m_value);
    return *this;
}

bool QGValue::isValid() const
{
    return !(m_value == NULL);
}

//static
QGValue QGValue::fromGValue(const GValue *val)
{
    QGValue v;
    v.m_value = g_slice_new0(GValue);
    g_value_init(v.m_value, G_VALUE_TYPE(val));
    g_value_copy(val, v.m_value);
    return v;
}

GValue *QGValue::toGValue() const
{
    return m_value;
}

//static
QGValue QGValue::fromQVariant(const QVariant & variant, quint32 gtype)
{
    if ( gtype == 0 ) {
        switch (variant.userType()) {
        case QMetaType::Bool:
            gtype = G_TYPE_BOOLEAN;
            break;
        case QMetaType::Short:
        case QMetaType::Int:
            gtype = G_TYPE_INT;
            break;
        case QMetaType::UShort:
        case QMetaType::UInt:
            gtype = G_TYPE_UINT;
            break;
        case QMetaType::Double:
            gtype = G_TYPE_DOUBLE;
            break;
        case QMetaType::Char:
            gtype = G_TYPE_CHAR;
            break;
        case QMetaType::UChar:
            gtype = G_TYPE_UCHAR;
            break;
        case QMetaType::Long:
            gtype = G_TYPE_LONG;
            break;
        case QMetaType::ULong:
            gtype = G_TYPE_ULONG;
            break;
        case QMetaType::LongLong:
            gtype = G_TYPE_INT64;
            break;
        case QMetaType::ULongLong:
            gtype = G_TYPE_UINT64;
            break;
        case QMetaType::QByteArray:
        case QMetaType::QString:
            gtype = G_TYPE_STRING;
            break;
        case QMetaType::VoidStar:
            gtype = G_TYPE_POINTER;
            break;
        default:
            qWarning("QGValue::fromQVariant: Could not convert the type of the given QVariant to a GType");
            break;
        }
    }

    QGValue v;
    v.m_value = g_slice_new0(GValue);
    g_value_init(v.m_value, gtype);

    switch (G_TYPE_FUNDAMENTAL(gtype)) {
    case G_TYPE_BOOLEAN:
        g_value_set_boolean(v.m_value, variant.value<gboolean>());
        break;
    case G_TYPE_CHAR:
        g_value_set_char(v.m_value, variant.value<gchar>());
        break;
    case G_TYPE_DOUBLE:
        g_value_set_double(v.m_value, variant.value<gdouble>());
        break;
    case G_TYPE_ENUM:
        g_value_set_enum(v.m_value, variant.value<gint>());
        break;
    case G_TYPE_FLAGS:
        g_value_set_flags(v.m_value, variant.value<guint>());
        break;
    case G_TYPE_FLOAT:
        g_value_set_float(v.m_value, variant.value<gfloat>());
        break;
    case G_TYPE_INT:
        g_value_set_int(v.m_value, variant.value<gint>());
        break;
    case G_TYPE_INT64:
        g_value_set_int64(v.m_value, variant.value<gint64>());
        break;
    case G_TYPE_LONG:
        g_value_set_long(v.m_value, variant.value<glong>());
        break;
    case G_TYPE_POINTER:
        g_value_set_pointer(v.m_value, variant.value<gpointer>());
        break;
    case G_TYPE_STRING:
        g_value_set_string(v.m_value, variant.toByteArray().constData());
        break;
    case G_TYPE_UCHAR:
        g_value_set_uchar(v.m_value, variant.value<guchar>());
        break;
    case G_TYPE_UINT:
        g_value_set_uint(v.m_value, variant.value<guint>());
        break;
    case G_TYPE_UINT64:
        g_value_set_uint64(v.m_value, variant.value<guint64>());
        break;
    case G_TYPE_ULONG:
        g_value_set_ulong(v.m_value, variant.value<gulong>());
        break;
/*
    case G_TYPE_GTYPE:
        g_value_set_gtype(v.m_value, variant.value<GType>());
        break;
    case G_TYPE_OBJECT:
        g_value_set_object(v.m_value, variant.value<GObject*>());
        break;
*/
    // skip G_TYPE_BOXED
    // skip G_TYPE_PARAM
    // skip custom types
    default:
        g_value_unset(v.m_value);
        g_slice_free(GValue, v.m_value);
    }

    return v;
}

QVariant QGValue::toQVariant() const
{
    switch(G_TYPE_FUNDAMENTAL(G_VALUE_TYPE(m_value))) {
    case G_TYPE_BOOLEAN:
        return QVariant::fromValue<bool>(g_value_get_boolean(m_value));
    case G_TYPE_CHAR:
        return QVariant::fromValue<char>(g_value_get_char(m_value));
    case G_TYPE_DOUBLE:
        return QVariant::fromValue<double>(g_value_get_double(m_value));
    case G_TYPE_ENUM:
        return QVariant::fromValue<int>(g_value_get_enum(m_value));
    case G_TYPE_FLAGS:
        return QVariant::fromValue<uint>(g_value_get_flags(m_value));
    case G_TYPE_FLOAT:
        return QVariant::fromValue<float>(g_value_get_float(m_value));
    case G_TYPE_INT:
        return QVariant::fromValue<int>(g_value_get_int(m_value));
    case G_TYPE_INT64:
        return QVariant::fromValue<qint64>(g_value_get_int64(m_value));
    case G_TYPE_LONG:
        return QVariant::fromValue<long>(g_value_get_long(m_value));
    case G_TYPE_POINTER:
        return QVariant::fromValue<gpointer>(g_value_get_pointer(m_value));
    case G_TYPE_STRING:
        return QVariant(QByteArray(g_value_get_string(m_value)));
    case G_TYPE_UCHAR:
        return QVariant::fromValue<uchar>(g_value_get_uchar(m_value));
    case G_TYPE_UINT:
        return QVariant::fromValue<uint>(g_value_get_uint(m_value));
    case G_TYPE_UINT64:
        return QVariant::fromValue<quint64>(g_value_get_uint64(m_value));
    case G_TYPE_ULONG:
        return QVariant::fromValue<unsigned long>(g_value_get_ulong(m_value));
/*
    case G_TYPE_GTYPE:
        return QVariant::fromValue<GType>(g_value_get_gtype(m_value));
    case G_TYPE_OBJECT:
        return QVariant::fromValue<GObject*>(g_value_get_gobject(m_value));
*/
    // skip G_TYPE_BOXED
    // skip G_TYPE_PARAM
    // skip custom types
    default:
        return QVariant();
    }
}

QDebug operator<<(QDebug debug, const QGValue & qgvalue)
{
    char *str = g_strdup_value_contents(qgvalue.m_value);
    debug.nospace() << "(QGValue: " << str << ")";
    g_free(str);
    return debug.space();
}

}
