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
#include "qgstminiobject.h"
#include <gst/gst.h>

namespace QtGstreamer {

QGST_WRAPPER_GTYPE(QGstMiniObject, GST_TYPE_MINI_OBJECT)

QGstMiniObject::QGstMiniObject(GstMiniObject *gstMiniObject)
{
    m_object = gstMiniObject;
}

//static
QGstMiniObjectPtr QGstMiniObject::fromGstMiniObject(GstMiniObject *gstMiniObject)
{
    return QGstMiniObjectPtr(new QGstMiniObject(gst_mini_object_ref(gstMiniObject)));
}

QGstMiniObject::~QGstMiniObject()
{
    gst_mini_object_unref(m_object);
}

QGstMiniObjectPtr QGstMiniObject::copy() const
{
    return QGstMiniObjectPtr(new QGstMiniObject(gst_mini_object_copy(m_object)));
}

bool QGstMiniObject::isWritable() const
{
    return gst_mini_object_is_writable(m_object);
}

void QGstMiniObject::makeWritable()
{
    m_object = gst_mini_object_make_writable(m_object);
}

QGstMiniObject::Flags QGstMiniObject::flags() const
{
    return QFlag((GST_MINI_OBJECT_FLAGS(m_object)));
}

bool QGstMiniObject::flagIsSet(Flag flag) const
{
    return GST_MINI_OBJECT_FLAG_IS_SET(m_object, flag);
}

void QGstMiniObject::flagSet(Flag flag)
{
    GST_MINI_OBJECT_FLAG_SET(m_object, flag);
}

void QGstMiniObject::flagUnset(Flag flag)
{
    GST_MINI_OBJECT_FLAG_UNSET(m_object, flag);
}

GstMiniObject *QGstMiniObject::getNativeObject()
{
    return gst_mini_object_ref(m_object);
}

}
