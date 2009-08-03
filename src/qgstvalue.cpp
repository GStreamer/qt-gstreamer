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
#include "qgstvalue.h"
#include "qgstelement.h"
#include "qgstpad.h"
#include "qgstcaps.h"
#include <QtCore/QDebug>
#include <gst/gst.h>

namespace QtGstreamer {

QGstValue::QGstValue()
    : m_value(NULL)
{
}

#define FUNDAMENTAL_TYPE_FUNCTIONS(capsType, camelType, lowercaseType, T) \
    QGstValue::QGstValue(T val) \
        : m_value(NULL) \
    { \
        set ##camelType (val); \
    } \
    bool QGstValue::holds ##camelType () const \
    { \
        return G_VALUE_HOLDS_ ##capsType (m_value); \
    } \
    \
    T QGstValue::get ##camelType () const \
    { \
        return g_value_get_ ##lowercaseType (m_value); \
    } \
    \
    void QGstValue::set ##camelType (T val) \
    { \
        reinit(G_TYPE_ ##capsType); \
        g_value_set_ ##lowercaseType (m_value, val); \
    }

FUNDAMENTAL_TYPE_FUNCTIONS(CHAR, Char, char, char)
FUNDAMENTAL_TYPE_FUNCTIONS(UCHAR, UChar, uchar, uchar)
FUNDAMENTAL_TYPE_FUNCTIONS(INT, Int, int, int)
FUNDAMENTAL_TYPE_FUNCTIONS(UINT, UInt, uint, uint)
FUNDAMENTAL_TYPE_FUNCTIONS(INT64, Int64, int64, qint64)
FUNDAMENTAL_TYPE_FUNCTIONS(UINT64, UInt64, uint64, quint64)
FUNDAMENTAL_TYPE_FUNCTIONS(BOOLEAN, Boolean, boolean, bool)
FUNDAMENTAL_TYPE_FUNCTIONS(DOUBLE, Double, double, double)

#undef FUNDAMENTAL_TYPE_FUNCTIONS

QGstValue::QGstValue(const char *val)
    : m_value(NULL)
{
    setString(val);
}

QGstValue::QGstValue(const QByteArray & val)
    : m_value(NULL)
{
    setString(val.constData());
}

bool QGstValue::holdsString() const
{
    return G_VALUE_HOLDS_STRING(m_value);
}

QByteArray QGstValue::getString() const
{
    return QByteArray(g_value_get_string(m_value));
}

void QGstValue::setString(const char *val)
{
    reinit(G_TYPE_STRING);
    g_value_set_string(m_value, val);
}

QGstValue::QGstValue(const QGstObjectPtr & val)
{
    m_value = g_slice_new0(GValue);
    g_value_init(m_value, GST_TYPE_OBJECT);
    g_value_set_object(m_value, val->peekNativeObject());
}

QGstValue::QGstValue(const QGstElementPtr & val)
{
    m_value = g_slice_new0(GValue);
    g_value_init(m_value, GST_TYPE_ELEMENT);
    g_value_set_object(m_value, val->peekNativeObject());
}

QGstValue::QGstValue(const QGstPadPtr & val)
{
    m_value = g_slice_new0(GValue);
    g_value_init(m_value, GST_TYPE_PAD);
    g_value_set_object(m_value, val->peekNativeObject());
}

QGstValue::QGstValue(const QGstCapsPtr & val)
    : m_value(NULL)
{
    setCaps(val);
}

QGstValue::QGstValue(const QGstValue & other)
    : m_value(NULL)
{
    operator=(other);
}

QGstValue::~QGstValue()
{
    if ( m_value ) {
        g_value_unset(m_value);
        g_slice_free(GValue, m_value);
    }
}

QGstValue & QGstValue::operator=(const QGstValue & other)
{
    reinit(G_VALUE_TYPE(other.m_value));
    g_value_copy(other.m_value, m_value);
    return *this;
}

void QGstValue::reinit(unsigned long gtype)
{
    if ( m_value ) {
        g_value_unset(m_value);
    } else {
        m_value = g_slice_new0(GValue);
    }
    g_value_init(m_value, gtype);
}

bool QGstValue::isValid() const
{
    return !(m_value == NULL);
}

//static
QGstValue QGstValue::fromGValue(const GValue *val)
{
    QGstValue v;
    v.m_value = g_slice_new0(GValue);
    g_value_init(v.m_value, G_VALUE_TYPE(val));
    g_value_copy(val, v.m_value);
    return v;
}

GValue *QGstValue::toGValue() const
{
    return m_value;
}

//static
QGstValue QGstValue::fromQVariant(const QVariant & variant, quint32 gtype)
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
            qWarning("QGstValue::fromQVariant: Could not convert the type of the given QVariant to a GType");
            break;
        }
    }

    QGstValue v;
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

QVariant QGstValue::toQVariant() const
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

bool QGstValue::holdsFourcc() const
{
    return GST_VALUE_HOLDS_FOURCC(m_value);
}

quint32 QGstValue::getFourcc() const
{
    return gst_value_get_fourcc(m_value);
}

void QGstValue::setFourcc(quint32 fourcc)
{
    reinit(GST_TYPE_FOURCC);
    gst_value_set_fourcc(m_value, fourcc);
}

void QGstValue::setFourcc(char a, char b, char c, char d)
{
    reinit(GST_TYPE_FOURCC);
    gst_value_set_fourcc(m_value, GST_MAKE_FOURCC(a, b, c, d));
}

void QGstValue::setFourcc(const char fourccStr[4])
{
    reinit(GST_TYPE_FOURCC);
    gst_value_set_fourcc(m_value, GST_STR_FOURCC(fourccStr));
}

bool QGstValue::holdsIntRange() const
{
    return GST_VALUE_HOLDS_INT_RANGE(m_value);
}

int QGstValue::getIntRangeMin() const
{
    return gst_value_get_int_range_min(m_value);
}

int QGstValue::getIntRangeMax() const
{
    return gst_value_get_int_range_max(m_value);
}

void QGstValue::setIntRange(int start, int end)
{
    reinit(GST_TYPE_INT_RANGE);
    gst_value_set_int_range(m_value, start, end);
}

bool QGstValue::holdsDoubleRange() const
{
    return GST_VALUE_HOLDS_DOUBLE_RANGE(m_value);
}

double QGstValue::getDoubleRangeMin() const
{
    return gst_value_get_double_range_min(m_value);
}

double QGstValue::getDoubleRangeMax() const
{
    return gst_value_get_double_range_max(m_value);
}

void QGstValue::setDoubleRange(double start, double end)
{
    reinit(GST_TYPE_DOUBLE_RANGE);
    gst_value_set_double_range(m_value, start, end);
}

bool QGstValue::holdsList() const
{
    return GST_VALUE_HOLDS_LIST(m_value);
}

uint QGstValue::listGetSize() const
{
    return gst_value_list_get_size(m_value);
}

QGstValue QGstValue::listGetValue(uint index) const
{
    return fromGValue(gst_value_list_get_value(m_value, index));
}

bool QGstValue::holdsArray() const
{
    return GST_VALUE_HOLDS_ARRAY(m_value);
}

uint QGstValue::arrayGetSize() const
{
    return gst_value_array_get_size(m_value);
}

QGstValue QGstValue::arrayGetValue(uint index) const
{
    return fromGValue(gst_value_array_get_value(m_value, index));
}

bool QGstValue::holdsFraction() const
{
    return GST_VALUE_HOLDS_FRACTION(m_value);
}

int QGstValue::getFractionNumerator() const
{
    return gst_value_get_fraction_numerator(m_value);
}

int QGstValue::getFractionDenominator() const
{
    return gst_value_get_fraction_denominator(m_value);
}

void QGstValue::setFraction(int numerator, int denominator)
{
    reinit(GST_TYPE_FRACTION);
    gst_value_set_fraction(m_value, numerator, denominator);
}

bool QGstValue::fractionMultiply(const QGstValue & factor1, const QGstValue & factor2)
{
    reinit(GST_TYPE_FRACTION);
    return gst_value_fraction_multiply(m_value, factor1.m_value, factor2.m_value);
}

bool QGstValue::fractionSubtract(const QGstValue & minuend, const QGstValue & subtrahend)
{
    reinit(GST_TYPE_FRACTION);
    return gst_value_fraction_subtract(m_value, minuend.m_value, subtrahend.m_value);
}

bool QGstValue::holdsFractionRange() const
{
    return GST_VALUE_HOLDS_FRACTION_RANGE(m_value);
}

QGstValue QGstValue::getFractionRangeMin() const
{
    return fromGValue(gst_value_get_fraction_range_min(m_value));
}

QGstValue QGstValue::getFractionRangeMax() const
{
    return fromGValue(gst_value_get_fraction_range_max(m_value));
}

void QGstValue::setFractionRange(const QGstValue & start, const QGstValue & end)
{
    reinit(GST_TYPE_FRACTION_RANGE);
    gst_value_set_fraction_range(m_value, start.m_value, end.m_value);
}

void QGstValue::setFractionRange(int numerator_start, int denominator_start,
                               int numerator_end, int denominator_end)
{
    reinit(GST_TYPE_FRACTION_RANGE);
    gst_value_set_fraction_range_full(m_value, numerator_start, denominator_start,
                                               numerator_end, denominator_end);
}

bool QGstValue::holdsCaps() const
{
    return GST_VALUE_HOLDS_CAPS(m_value);
}

QGstCapsPtr QGstValue::getCaps() const
{
    GstCaps *caps = gst_caps_copy(gst_value_get_caps(m_value));
    QGstCapsPtr result = QGstCaps::fromGstCaps(caps);
    gst_caps_unref(caps);
    return result;
}

void QGstValue::setCaps(const QGstCapsPtr & val)
{
    reinit(GST_TYPE_CAPS);
    gst_value_set_caps(m_value, val->m_caps);
}

bool QGstValue::holdsStructure() const
{
    return GST_VALUE_HOLDS_STRUCTURE(m_value);
}

bool QGstValue::isFixed() const
{
    return gst_value_is_fixed(m_value);
}

QByteArray QGstValue::serialize() const
{
    char *str = gst_value_serialize(m_value);
    if ( !str ) {
        return QByteArray();
    } else {
        QByteArray result(str);
        g_free(str);
        return result;
    }
}

bool QGstValue::deserialize(const char *src)
{
    return gst_value_deserialize(m_value, src);
}

//static
bool QGstValue::canCompare(const QGstValue & val1, const QGstValue & val2)
{
    return gst_value_can_compare(val1.m_value, val2.m_value);
}

bool QGstValue::operator<(const QGstValue & val2) const
{
    return (gst_value_compare(m_value, val2.m_value) == GST_VALUE_LESS_THAN);
}

bool QGstValue::operator<=(const QGstValue & val2) const
{
    int result = gst_value_compare(m_value, val2.m_value);
    return (result == GST_VALUE_LESS_THAN || result == GST_VALUE_EQUAL);
}

bool QGstValue::operator>(const QGstValue & val2) const
{
    return (gst_value_compare(m_value, val2.m_value) == GST_VALUE_GREATER_THAN);
}

bool QGstValue::operator>=(const QGstValue & val2) const
{
    int result = gst_value_compare(m_value, val2.m_value);
    return (result == GST_VALUE_GREATER_THAN || result == GST_VALUE_EQUAL);
}

bool QGstValue::operator==(const QGstValue & val2) const
{
    return (gst_value_compare(m_value, val2.m_value) == GST_VALUE_EQUAL);
}

QDebug operator<<(QDebug debug, const QGstValue & qgvalue)
{
    char *str = g_strdup_value_contents(qgvalue.m_value);
    debug.nospace() << "(QGstValue: " << str << ")";
    g_free(str);
    return debug.space();
}

}
