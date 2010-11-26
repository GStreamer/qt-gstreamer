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
#include "objectstore_p.h"
#include <gst/gst.h>

namespace QGst {

MiniObjectPtr MiniObject::copy() const
{
    return MiniObjectPtr::wrap(gst_mini_object_copy(object<GstMiniObject>()), false);
}

bool MiniObject::isWritable() const
{
    return gst_mini_object_is_writable(object<GstMiniObject>());
}

MiniObjectFlags MiniObject::flags() const
{
    return QFlag((GST_MINI_OBJECT_FLAGS(object<GstMiniObject>())));
}

bool MiniObject::flagIsSet(MiniObjectFlag flag) const
{
    return GST_MINI_OBJECT_FLAG_IS_SET(object<GstMiniObject>(), flag);
}

void MiniObject::setFlag(MiniObjectFlag flag)
{
    GST_MINI_OBJECT_FLAG_SET(object<GstMiniObject>(), flag);
}

void MiniObject::unsetFlag(MiniObjectFlag flag)
{
    GST_MINI_OBJECT_FLAG_UNSET(object<GstMiniObject>(), flag);
}

void MiniObject::ref(bool increaseRef)
{
    if (Private::ObjectStore::put(m_object)) {
        if (increaseRef) {
            gst_mini_object_ref(GST_MINI_OBJECT(m_object));
        }
    }
}

void MiniObject::unref()
{
    if (Private::ObjectStore::take(m_object)) {
        gst_mini_object_unref(GST_MINI_OBJECT(m_object));
    }
}

void MiniObject::makeWritable()
{
    if (!isWritable()) {
        //m_object will change, need to deal with the reference count properly
        unref();

        /*
        * Calling gst_*_make_writable() below is tempting but wrong, as the above unref() might have
        * dropped the gst refcount from 2 to 1 temporarily. When this happens gst_*_make_writable()
        * will do nothing, return the same object, and the refcount will go back to 2 when we ref()
        * it again below.
        * So the right thing to do is to copy() here to make sure we get a new object in this case.
        *
        * Note that if the external refCount is 1 then the gst_*_make_writable() semantics is
        * preserved (nothing is copied, same object is used) as we tested for this condition
        * before entering this code path.
        */
        m_object = gst_mini_object_copy(GST_MINI_OBJECT(m_object));

        //Manage our reference count for the new m_object
        ref(false);
    }
}

} //namespace QGst
