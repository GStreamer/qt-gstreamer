/*
    Copyright (C) 2009  George Kiagiadakis <kiagiadakis.george@gmail.com>

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
#include "qgstmessage.h"
#include "qgstobject.h"
#include <QtCore/QDebug>
#include <gst/gst.h>

namespace QtGstreamer {

QGST_WRAPPER_GTYPE(QGstMessage, GST_TYPE_MESSAGE)

QGstMessage::QGstMessage(GstMessage *message)
    : QGstMiniObject(GST_MINI_OBJECT(message))
{
}

//static
QGstMessagePtr QGstMessage::fromGstMessage(GstMessage *message)
{
    return QGstMessagePtr(new QGstMessage(gst_message_ref(message)));
}

QGstMessage::~QGstMessage()
{
}

QGstObjectPtr QGstMessage::source() const
{
    return QGstObject::fromGstObject(GST_MESSAGE_SRC(GST_MESSAGE(m_object)));
}

quint64 QGstMessage::timestamp() const
{
    return GST_MESSAGE(m_object)->timestamp;
}

QGstMessage::Type QGstMessage::type() const
{
    return static_cast<Type>(GST_MESSAGE_TYPE(m_object));
}

QByteArray QGstMessage::typeName() const
{
    return QByteArray(GST_MESSAGE_TYPE_NAME(m_object));
}

//static
QByteArray QGstMessage::typeToName(Type type)
{
    return QByteArray(gst_message_type_get_name(static_cast<GstMessageType>(type)));
}

quint32 QGstMessage::sequenceNumber() const
{
    return gst_message_get_seqnum(GST_MESSAGE(m_object));
}

void QGstMessage::setSequenceNumber(quint32 num)
{
    gst_message_set_seqnum(GST_MESSAGE(m_object), num);
}

static QGstMessage::ErrorStruct parseErrorInternal(GstMessage *msg,
                                                   void (*func)(GstMessage*, GError**, gchar**))
{
    QGstMessage::ErrorStruct result;
    GError *err;
    gchar *debug;

    (*func)(msg, &err, &debug);

    result.code = err->code;
    result.message = err->message;
    result.debugMessage = debug;

    g_error_free(err);
    g_free(debug);
    return result;
}

QGstMessage::ErrorStruct QGstMessage::parseError() const
{
    Q_ASSERT(type() == Error);
    return parseErrorInternal(GST_MESSAGE(m_object), &gst_message_parse_error);
}

QGstMessage::WarningStruct QGstMessage::parseWarning() const
{
    Q_ASSERT(type() == Warning);
    return parseErrorInternal(GST_MESSAGE(m_object), &gst_message_parse_warning);
}

QGstMessage::InfoStruct QGstMessage::parseInfo() const
{
    Q_ASSERT(type() == Info);
    return parseErrorInternal(GST_MESSAGE(m_object), &gst_message_parse_info);
}

QDebug operator<<(QDebug debug, const QGstMessage::ErrorStruct & error)
{
    debug.nospace() << "[Code: " << error.code << ", Message: " << error.message
                    << ", DebugMessage: " << error.debugMessage << "]";
    return debug;
}

} //namespace QtGstreamer

QDebug operator<<(QDebug debug, const QtGstreamer::QGstMessagePtr & message)
{
    using namespace QtGstreamer;
    debug.nospace() << "QGstMessage(Type: " << message->typeName();
    switch (message->type()) {
    case QGstMessage::Error:
        debug.space() << message->parseError();
        break;
    case QGstMessage::Warning:
        debug.space() << message->parseWarning();
        break;
    case QGstMessage::Info:
        debug.space() << message->parseInfo();
        break;
    default:
        break;
    }

    debug.nospace() << ")";
    return debug.space();
}
