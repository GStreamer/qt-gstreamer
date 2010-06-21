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
#include "message.h"
#include "object.h"
#include <QtCore/QDebug>
#include <gst/gst.h>

namespace QGst {

ObjectPtr Message::source() const
{
    return ObjectPtr::wrap(GST_MESSAGE_SRC(GST_MESSAGE(object())));
}

quint64 Message::timestamp() const
{
    return GST_MESSAGE(object())->timestamp;
}

MessageType Message::type() const
{
    return static_cast<MessageType>(GST_MESSAGE_TYPE(object()));
}

quint32 Message::sequenceNumber() const
{
    return gst_message_get_seqnum(GST_MESSAGE(object()));
}

void Message::setSequenceNumber(quint32 num)
{
    gst_message_set_seqnum(GST_MESSAGE(object()), num);
}

} //namespace QGst

QDebug operator<<(QDebug debug, QGst::MessageType type)
{
    debug.nospace() << gst_message_type_get_name(static_cast<GstMessageType>(type));
    return debug.space();
}

QDebug operator<<(QDebug debug, const QGst::MessagePtr & message)
{
    debug.nospace() << "QGst::Message(Type: " << message->type() << ")";
    return debug.space();
}
