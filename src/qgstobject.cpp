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

QGstValue QGstObject::property(const char *name) const
{
    GParamSpec *paramSpec = g_object_class_find_property(G_OBJECT_GET_CLASS(m_object), name);
    if ( !paramSpec ) {
        qWarning() << "QGstObject::property: No such property:" << name;
        return QGstValue();
    }
    g_param_spec_ref_sink(paramSpec);

    GValue *gvalue = g_slice_new0(GValue);
    g_value_init(gvalue, G_PARAM_SPEC_VALUE_TYPE(paramSpec));
    g_object_get_property(G_OBJECT(m_object), name, gvalue);

    QGstValue result = QGstValue::fromGValue(gvalue);

    g_slice_free(GValue, gvalue);
    g_param_spec_unref(paramSpec);
    return result;
}

void QGstObject::setProperty(const char *name, const QGstValue & value)
{
    g_object_set_property(G_OBJECT(m_object), name, value.toGValue());
}

}

#include "qgstobject.moc"
