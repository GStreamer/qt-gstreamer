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

#ifndef QGST_ALLOCATOR_H
#define QGST_ALLOCATOR_H

#include "global.h"
#include "object.h"
#include "enums.h"
#include "memory.h"
#include <qglobal.h>

namespace QGst {

class AllocationParamsPrivate;
class QTGSTREAMER_EXPORT AllocationParams
{
public:
    AllocationParams();
    AllocationParams(AllocationParams &other);
    ~AllocationParams();

    MemoryFlags flags() const;
    void setFlags(MemoryFlags flags);
    size_t align() const;
    void setAlign(size_t align);
    size_t prefix() const;
    void setPrefix(size_t align);
    size_t padding() const;
    void setPadding(size_t padding);

protected:
    AllocationParamsPrivate * const d_ptr;

private:
    Q_DECLARE_PRIVATE(AllocationParams)
    friend Allocator;
};


class QTGSTREAMER_EXPORT Allocator : public Object
{
    QGST_WRAPPER(Allocator)
public:
    explicit Allocator(GstAllocator *g_alloc);

    operator GstAllocator *();
    operator GstAllocator *() const;

    // search for an already registered allocator.
    static AllocatorPtr find(const char *name);
    // this corresponds to gst_allocator_register.
    void registerAllocator(const char *name);
    // sets this allocator to be the default allocator
    void setDefault();

    // using this allocator create a chunk of memory
    MemoryPtr alloc(size_t size, AllocationParams &params);
    // release memory allocated with alloc
    void free(MemoryPtr memory);
//    MemoryPtr newWrapped(MemoryFlags flags, void *data, size_t maxsize, size_t offset, size_t size, void *user_data, GDestroyNotify);

    friend Memory;
};

} /* QGst */

QGST_REGISTER_TYPE(QGst::Allocator)
#endif /* QGST_ALLOCATOR_H */
