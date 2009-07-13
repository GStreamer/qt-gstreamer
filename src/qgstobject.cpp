/*
    Copyright (C) 2009  George Kiagiadakis <kiagiadakis.george@gmail.com>
    Copyright (C) 2009  Collabora Ltd. <http://www.collabora.co.uk/>
    Copyright (C) 2009  Nokia Corporation

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
#include "qgstobject.h"
#include <QtCore/QtDebug>
#include <gst/gstobject.h>

namespace QtGstreamer {

QGstObject::QGstObject(GstObject *gstObject)
    : QObject(), m_object(NULL)
{
    Q_ASSERT(gstObject);
    m_object = gstObject;
    if ( GST_OBJECT_IS_FLOATING(m_object) ) {
        gst_object_ref(m_object);
        gst_object_sink(m_object);
    } else {
        gst_object_ref(m_object);
    }
}

//static
QGstObjectPtr QGstObject::fromGstObject(GstObject *gstObject)
{
    return QGstObjectPtr(new QGstObject(gstObject));
}

QGstObject::~QGstObject()
{
    gst_object_unref(m_object);
}

/* These two static functions are based on code from the TelepathyQt4 library */
static GValue *gValueFromQVariant(const QVariant & variant, GType gType)
{
    GValue *v = g_slice_new0(GValue);
    g_value_init(v, gType);

    switch (gType) {
    case G_TYPE_BOOLEAN:
        g_value_set_boolean(v, variant.value<gboolean>());
        break;
    case G_TYPE_CHAR:
        g_value_set_char(v, variant.value<char>());
        break;
    // skip G_TYPE_BOXED
    case G_TYPE_DOUBLE:
        g_value_set_double(v, variant.value<gdouble>());
        break;
    // skip G_TYPE_ENUM
    // skip G_TYPE_FLAGS
    case G_TYPE_FLOAT:
        g_value_set_float(v, variant.value<gfloat>());
        break;
    // skip G_TYPE_GTYPE
    case G_TYPE_INT:
        g_value_set_int(v, variant.value<gint>());
        break;
    case G_TYPE_INT64:
        g_value_set_int64(v, variant.value<gint64>());
        break;
    case G_TYPE_LONG:
        g_value_set_long(v, variant.value<glong>());
        break;
    // skip G_TYPE_OBJECT
    // skip G_TYPE_PARAM
    // skip G_TYPE_POINTER:
    case G_TYPE_STRING:
        g_value_set_string(v, variant.value<QString>().toLocal8Bit().constData());
        break;
    case G_TYPE_UCHAR:
        g_value_set_uchar(v, variant.value<guchar>());
        break;
    case G_TYPE_UINT:
        g_value_set_uint(v, variant.value<guint>());
        break;
    case G_TYPE_UINT64:
        g_value_set_uint64(v, variant.value<guint64>());
        break;
    case G_TYPE_ULONG:
        g_value_set_ulong(v, variant.value<gulong>());
        break;
    // skip custom types
    default:
        g_slice_free(GValue, v);
        return 0;
    }

    return v;
}

static QVariant gValueToQVariant(GValue *gvalue)
{
    switch(G_VALUE_TYPE(gvalue)) {
    case G_TYPE_BOOLEAN:
        return QVariant::fromValue<bool>(g_value_get_boolean(gvalue));
    case G_TYPE_CHAR:
        return QVariant::fromValue<char>(g_value_get_char(gvalue));
    // skip G_TYPE_BOXED
    case G_TYPE_DOUBLE:
        return QVariant::fromValue<double>(g_value_get_double(gvalue));
    // skip G_TYPE_ENUM
    // skip G_TYPE_FLAGS
    case G_TYPE_FLOAT:
        return QVariant::fromValue<float>(g_value_get_float(gvalue));
    // skip G_TYPE_GTYPE
    case G_TYPE_INT:
        return QVariant::fromValue<int>(g_value_get_int(gvalue));
    case G_TYPE_INT64:
        return QVariant::fromValue<qint64>(g_value_get_int64(gvalue));
    case G_TYPE_LONG:
        return QVariant::fromValue<long>(g_value_get_long(gvalue));
    // skip G_TYPE_OBJECT
    // skip G_TYPE_PARAM
    // skip G_TYPE_POINTER:
    case G_TYPE_STRING:
        return QVariant(QByteArray(g_value_get_string(gvalue)));
    case G_TYPE_UCHAR:
        return QVariant::fromValue<uchar>(g_value_get_uchar(gvalue));
    case G_TYPE_UINT:
        return QVariant::fromValue<uint>(g_value_get_uint(gvalue));
    case G_TYPE_UINT64:
        return QVariant::fromValue<quint64>(g_value_get_uint64(gvalue));
    case G_TYPE_ULONG:
        return QVariant::fromValue<unsigned long>(g_value_get_ulong(gvalue));
    // skip custom types
    default:
        return QVariant();
    }
}

QVariant QGstObject::property(const char *name) const
{
    GParamSpec *paramSpec = g_object_class_find_property(G_OBJECT_GET_CLASS(m_object), name);
    if ( !paramSpec ) {
        qWarning() << "No such property:" << name;
        return QVariant();
    }
    g_param_spec_ref_sink(paramSpec);

    GValue *gvalue = g_slice_new0(GValue);
    g_value_init(gvalue, G_PARAM_SPEC_VALUE_TYPE(paramSpec));

    g_object_get_property(G_OBJECT(m_object), name, gvalue);

    QVariant result = gValueToQVariant(gvalue);
    if ( !result.isValid() ) {
        qWarning() << "Could not convert GValue to QVariant"
                    << "for the property:" << name << ", using the GType:"
                    << G_PARAM_SPEC_VALUE_TYPE(paramSpec);
    }

    g_slice_free(GValue, gvalue);
    g_param_spec_unref(paramSpec);
    return result;
}

void QGstObject::setProperty(const char *name, const QVariant & value)
{
    if ( !value.isValid() ) {
        qWarning() << "Invalid argument";
        return;
    }

    GParamSpec *paramSpec = g_object_class_find_property(G_OBJECT_GET_CLASS(m_object), name);
    if ( !paramSpec ) {
        qWarning() << "No such property:" << name;
        return;
    }
    g_param_spec_ref_sink(paramSpec);

    GValue *gvalue = gValueFromQVariant(value, G_PARAM_SPEC_VALUE_TYPE(paramSpec));
    if ( !gvalue ) {
        qWarning() << "Could not convert QVariant (" << value << ") to GValue"
                    << "for the property:" << name << ", using the GType:"
                    << G_PARAM_SPEC_VALUE_TYPE(paramSpec);
        g_param_spec_unref(paramSpec);
        return;
    }

    g_object_set_property(G_OBJECT(m_object), name, gvalue);
    g_slice_free(GValue, gvalue);
    g_param_spec_unref(paramSpec);
}

}

#include "qgstobject.moc"
