/*
    Copyright (C) 2013  Diane Trout
      @author Diane Trout <diane@ghic.org>

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
#include "buffer.h"
#include "caps.h"
#include "sample.h"
#include "structure.h"
#include <QtCore/QDebug>
#include <gst/gst.h>

namespace QGst {

// FIXME: need a real type for segment
SamplePtr Sample::create(const BufferPtr & buffer, const CapsPtr & caps,
                         void *segment, const Structure & info)
{
    GstStructure *cinfo = NULL;
    if (info.isValid())
        cinfo = gst_structure_copy(info);

    return SamplePtr::wrap(gst_sample_new(buffer, caps, static_cast<GstSegment *>(segment), cinfo), false);
}

BufferPtr Sample::buffer()
{
    return BufferPtr::wrap(gst_sample_get_buffer(object<GstSample>()));
}

CapsPtr Sample::caps()
{
    return CapsPtr::wrap(gst_sample_get_caps(object<GstSample>()));
}

const Structure Sample::info()
{
    return Structure(gst_sample_get_info(object<GstSample>()));
}

// FIXME: implement wrapper for gst_sample_get_segment, will need a wrapper for GstSegment
} //namespace QGst
