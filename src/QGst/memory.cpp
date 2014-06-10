/*
    Copyright (C) 2013  Diane Trout <diane@ghic.org>

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
#include "allocator.h"
#include "memory.h"
#include "buffer.h"

#include <gst/gstobject.h>
#include <gst/gstminiobject.h>
#include <gst/gstallocator.h>
#include <gst/gstmemory.h>
#include <gst/gstbuffer.h>

namespace QGst {

MapInfo::MapInfo()
{
    m_object = g_slice_new0(GstMapInfo);
}

MapInfo::~MapInfo()
{
    g_slice_free(GstMapInfo, m_object);
}

MapFlags MapInfo::flags() const
{
    return static_cast<MapFlag>(static_cast<GstMapInfo*>(m_object)->flags);
}

quint8 *MapInfo::data() const
{
    return static_cast<GstMapInfo*>(m_object)->data;
}

size_t MapInfo::size() const
{
    return static_cast<GstMapInfo*>(m_object)->size;
}

size_t MapInfo::maxSize() const
{
    return static_cast<GstMapInfo*>(m_object)->maxsize;
}

//-----------------------

MemoryPtr Memory::create(size_t size)
{
    return MemoryPtr::wrap(gst_allocator_alloc(NULL, size, NULL));
}

MemoryPtr Memory::create(MemoryFlags flags, AllocatorPtr allocator, MemoryPtr parent,
                         size_t maxsize, size_t align, size_t offset, size_t size)
{
    MemoryPtr mem;

    gst_memory_init(mem, static_cast<GstMemoryFlags>(static_cast<int>(flags)),
                    allocator->object<GstAllocator>(), parent, maxsize, align, offset, size);
    return mem;
}

size_t Memory::getSizes(size_t &offset, size_t &maxsize)
{
    return gst_memory_get_sizes(object<GstMemory>(), &offset, &maxsize);
}

bool Memory::map(MapInfo &info, MapFlags flags)
{
    return gst_memory_map(object<GstMemory>(), static_cast<GstMapInfo*>(info.m_object),
                          static_cast<GstMapFlags>(static_cast<int>(flags)));
}

void Memory::unmap(MapInfo &info)
{
    gst_memory_unmap(object<GstMemory>(), static_cast<GstMapInfo*>(info.m_object));
}

} // namespace QGst

