/*
    Copyright (C) 2011 Collabora Ltd. <info@collabora.co.uk>
      @author George Kiagiadakis <george.kiagiadakis@collabora.co.uk>

    This library is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published
    by the Free Software Foundation; either version 2.1 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "bufferlist.h"
#include <gst/gstbufferlist.h>

namespace QGst {

BufferListPtr BufferList::create()
{
    return BufferListPtr::wrap(gst_buffer_list_new(), false);
}

uint BufferList::groupsCount() const
{
    return gst_buffer_list_n_groups(object<GstBufferList>());
}

BufferPtr BufferList::bufferAt(uint group, uint index) const
{
    return BufferPtr::wrap(gst_buffer_list_get(object<GstBufferList>(), group, index));
}


BufferListIterator::BufferListIterator(const BufferListPtr & list)
{
    m_it = gst_buffer_list_iterate(list);
}

BufferListIterator::~BufferListIterator()
{
    gst_buffer_list_iterator_free(m_it);
}

uint BufferListIterator::buffersInCurrentGroup() const
{
    return gst_buffer_list_iterator_n_buffers(m_it);
}

BufferPtr BufferListIterator::next()
{
    return BufferPtr::wrap(gst_buffer_list_iterator_next(m_it));
}

void BufferListIterator::insert(const BufferPtr & buffer)
{
    gst_buffer_list_iterator_add(m_it, gst_buffer_ref(buffer));
}

void BufferListIterator::remove()
{
    gst_buffer_list_iterator_remove(m_it);
}

BufferPtr BufferListIterator::take()
{
    BufferPtr buf = BufferPtr::wrap(gst_buffer_list_iterator_steal(m_it), false);
    gst_buffer_list_iterator_remove(m_it);
    return buf;
}

void BufferListIterator::replace(const BufferPtr & other)
{
    gst_buffer_list_iterator_take(m_it, gst_buffer_ref(other));
}

void BufferListIterator::addGroup()
{
    gst_buffer_list_iterator_add_group(m_it);
}

bool BufferListIterator::nextGroup()
{
    return gst_buffer_list_iterator_next_group(m_it);
}

BufferPtr BufferListIterator::mergeGroup() const
{
    return BufferPtr::wrap(gst_buffer_list_iterator_merge_group(m_it), false);
}

} //namespace QGst

