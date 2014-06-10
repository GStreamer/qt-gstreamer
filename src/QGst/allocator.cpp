/*
    Copyright (C) 2014  Diane Trout <diane@ghic.org>

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
#include <gst/gstobject.h>
#include <gst/gstminiobject.h>
#include <gst/gstallocator.h>

namespace QGst {

struct AllocationParamsPrivate : GstAllocationParams
{
};

AllocationParams::AllocationParams()
  : d_ptr(new AllocationParamsPrivate)
{
    Q_D(AllocationParams);
    gst_allocation_params_init(d);
}

AllocationParams::AllocationParams(AllocationParams &other)
  : d_ptr(static_cast<AllocationParamsPrivate *>(gst_allocation_params_copy(other.d_ptr)))
{
}

AllocationParams::~AllocationParams()
{
    gst_allocation_params_free(d_ptr);
}

MemoryFlags AllocationParams::flags() const
{
    Q_D(const AllocationParams);
    return static_cast<QGst::MemoryFlags>(static_cast<unsigned int>(d->flags));
}

void AllocationParams::setFlags(MemoryFlags flags)
{
    Q_D(AllocationParams);
    d->flags = static_cast<GstMemoryFlags>(static_cast<unsigned int>(flags));
}

size_t AllocationParams::align() const
{
    Q_D(const AllocationParams);
    return d->align;
}

void AllocationParams::setAlign(size_t align)
{
    Q_D(AllocationParams);
    d->align = align;
}

size_t AllocationParams::prefix() const
{
    Q_D(const AllocationParams);
    return d->prefix;
}

void AllocationParams::setPrefix(size_t align)
{
    Q_D(AllocationParams);
    d->prefix = align;
}

size_t AllocationParams::padding() const
{
    Q_D(const AllocationParams);
    return d->padding;
}

void AllocationParams::setPadding(size_t padding)
{
    Q_D(AllocationParams);
    d->padding = padding;
}

Allocator::Allocator(GstAllocator *g_alloc)
{
    m_object = static_cast<void *>(g_alloc);
}

Allocator::operator GstAllocator *()
{
    return static_cast<GstAllocator *>(m_object);
}

Allocator::operator GstAllocator *() const
{
    return static_cast<GstAllocator *>(m_object);
}


AllocatorPtr Allocator::find(const char *name)
{
    return AllocatorPtr(new Allocator(gst_allocator_find(name)));
}

void Allocator::registerAllocator(const char *name)
{
    gst_allocator_register(name, object<GstAllocator>());
}

void Allocator::setDefault()
{
    gst_allocator_set_default(object<GstAllocator>());
}

MemoryPtr Allocator::alloc(size_t size, AllocationParams &params)
{
    return MemoryPtr::wrap(gst_allocator_alloc(object<GstAllocator>(), size, params.d_ptr));
}

void Allocator::free(MemoryPtr memory)
{
    gst_allocator_free(object<GstAllocator>(), memory);
}

} /* QGst */
