/*
    Copyright (C) 2010  Collabora Multimedia.
      @author Mauricio Piacentini <mauricio.piacentini@collabora.co.uk>

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
#include "buffer.h"
#include "caps.h"
#include <QtCore/QDebug>
#include <gst/gst.h>

namespace QGst {

BufferPtr Buffer::create(uint size)
{
    return BufferPtr::wrap(gst_buffer_try_new_and_alloc(size), false);
}

quint8 * Buffer::data() const
{
    return GST_BUFFER_DATA(object<GstBuffer>());
}

quint32 Buffer::size() const
{
    return GST_BUFFER_SIZE(object<GstBuffer>());
}

ClockTime Buffer::timeStamp() const
{
    return GST_BUFFER_TIMESTAMP(object<GstBuffer>());
}

ClockTime Buffer::duration() const
{
    return GST_BUFFER_DURATION(object<GstBuffer>());
}

CapsPtr Buffer::caps() const
{
    //wrap increasing the refcount
    return QGst::CapsPtr::wrap(GST_BUFFER_CAPS(object<GstBuffer>()));
}

void Buffer::setCaps(const CapsPtr & caps)
{
    gst_buffer_set_caps(object<GstBuffer>(), caps);
}

quint64 Buffer::offset() const
{
    return GST_BUFFER_OFFSET(object<GstBuffer>());
}

quint64 Buffer::offsetEnd() const
{
    return GST_BUFFER_OFFSET_END(object<GstBuffer>());
}

BufferFlags Buffer::flags() const
{
    return BufferFlags(GST_BUFFER_FLAGS(object<GstBuffer>()));
}

void Buffer::setFlags(const BufferFlags flags)
{
    GST_BUFFER_FLAGS(object<GstBuffer>()) = flags;
}

BufferPtr Buffer::copy() const
{
    return BufferPtr::wrap(gst_buffer_copy(object<GstBuffer>()), false);
}

} //namespace QGst
