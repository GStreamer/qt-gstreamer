/*
    Copyright (C) 2010  George Kiagiadakis <kiagiadakis.george@gmail.com>

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
#include "bus.h"
#include "message.h"
#include "../QGlib/signal.h"
#include <gst/gst.h>

namespace QGst {

//static
BusPtr Bus::create()
{
    GstBus *bus = gst_bus_new();
    gst_object_ref_sink(bus);
    return BusPtr::wrap(bus, false);
}

bool Bus::hasPendingMessages() const
{
    return gst_bus_have_pending(object<GstBus>());
}

MessagePtr Bus::peek() const
{
    return MessagePtr::wrap(gst_bus_peek(object<GstBus>()), false);
}

MessagePtr Bus::pop(ClockTime timeout)
{
    return MessagePtr::wrap(gst_bus_timed_pop(object<GstBus>(), timeout), false);
}

MessagePtr Bus::pop(MessageType type, ClockTime timeout)
{
    return MessagePtr::wrap(gst_bus_timed_pop_filtered(object<GstBus>(), timeout,
                                                       static_cast<GstMessageType>(type)), false);
}

bool Bus::post(const QGst::MessagePtr & message)
{
    return gst_bus_post(object<GstBus>(), message->copy().staticCast<Message>());
}

void Bus::setFlushing(bool flush)
{
    gst_bus_set_flushing(object<GstBus>(), flush);
}

void Bus::enableSyncMessageEmission()
{
    gst_bus_enable_sync_message_emission(object<GstBus>());
}

void Bus::disableSyncMessageEmission()
{
    gst_bus_disable_sync_message_emission(object<GstBus>());
}

} //namespace QGst
