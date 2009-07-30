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
#include "qgstcaps.h"
#include "qgststructure.h"
#include <gst/gstcaps.h>

namespace QtGstreamer {

QGstCaps::QGstCaps(GstCaps *gstCaps)
{
    m_caps = gst_caps_ref(gstCaps);
}

//static
QGstCapsPtr QGstCaps::newEmpty()
{
    return QGstCapsPtr(new QGstCaps(gst_caps_new_empty()));
}

//static
QGstCapsPtr QGstCaps::newAny()
{
    return QGstCapsPtr(new QGstCaps(gst_caps_new_any()));
}

//static
QGstCapsPtr QGstCaps::fromGstCaps(GstCaps *gstCaps)
{
    return QGstCapsPtr(new QGstCaps(gstCaps));
}

QGstCaps::~QGstCaps()
{
    gst_caps_unref(m_caps);
}

void QGstCaps::makeWritable()
{
    m_caps = gst_caps_make_writable(m_caps);
}

void QGstCaps::appendStructure(const QGstStructure & structure)
{
    gst_caps_append_structure(m_caps, gst_structure_copy(structure.m_structure));
}

uint QGstCaps::getSize() const
{
   return gst_caps_get_size(m_caps);
}

//static
QGstCapsPtr QGstCaps::fromString(const char *string)
{
    return QGstCapsPtr(new QGstCaps(gst_caps_from_string(string)));
}

QByteArray QGstCaps::toString() const
{
    char *str = gst_caps_to_string(m_caps);
    QByteArray result(str);
    g_free(str);
    return result;
}

}
