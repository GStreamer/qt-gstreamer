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
#include "miniobject.h"
#include <gst/gst.h>

namespace QGst {

MiniObjectPtr MiniObject::copy() const
{
    return MiniObjectPtr::wrap(gst_mini_object_copy(GST_MINI_OBJECT(object())), false);
}

bool MiniObject::isWritable() const
{
    return gst_mini_object_is_writable(GST_MINI_OBJECT(object()));
}

MiniObjectFlags MiniObject::flags() const
{
    return QFlag((GST_MINI_OBJECT_FLAGS(object())));
}

bool MiniObject::flagIsSet(MiniObjectFlag flag) const
{
    return GST_MINI_OBJECT_FLAG_IS_SET(object(), flag);
}

void MiniObject::setFlag(MiniObjectFlag flag)
{
    GST_MINI_OBJECT_FLAG_SET(object(), flag);
}

void MiniObject::unsetFlag(MiniObjectFlag flag)
{
    GST_MINI_OBJECT_FLAG_UNSET(object(), flag);
}

void MiniObject::ref()
{
    gst_mini_object_ref(GST_MINI_OBJECT(m_object));
}

void MiniObject::unref()
{
    gst_mini_object_unref(GST_MINI_OBJECT(m_object));
}

void MiniObject::makeWritable()
{
    m_object = gst_mini_object_make_writable(GST_MINI_OBJECT(m_object));
}

}
