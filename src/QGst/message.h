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
#ifndef QGST_MESSAGE_H
#define QGST_MESSAGE_H

#include "miniobject.h"
#include "structure.h"

namespace QGst {

/*! \headerfile message.h <QGst/Message>
 * \brief Wrapper class for GstMessage
 *
 * Messages are lightweight objects to signal the application of pipeline events. They
 * are posted by objects in the pipeline and are passed to the application using the Bus.
 *
 * Messages are implemented as a subclass of MiniObject with a generic GstStructure as the
 * content. This allows for writing custom messages without requiring an API change while
 * allowing a wide range of different types of messages.
 *
 * In these bindings, for convenience, each message type has its own Message subclass. This
 * does not reflect 1-1 the native C API, where there is only one Message class with tens of
 * 'new_foo' and 'parse_foo' methods. You can use RefPointer::dynamicCast() to cast a MessagePtr
 * to a RefPointer of one of the Message subclasses and it will behave as expected (i.e. it will
 * only succeed if the message type matches the message type that the subclass handles). Note
 * however that the Message subclasses \em cannot be used with ValueBase::get(), since a GValue
 * will actually contain a GstMessage (the subclasses do not exist in C) and ValueBase::get()
 * is not able to do dynamic casts. As a result of that, Message subclasses also \em cannot be
 * used as arguments in slots connected to GObject signals, even though you may know that your
 * slot will only be called with that type of message.
 */
class Message : public MiniObject
{
    QGST_WRAPPER(Message)
public:
    static MessagePtr create(MessageType type, const ObjectPtr & source,
                             const StructureBase & structure = SharedStructure(NULL));

    ObjectPtr source() const;
    quint64 timestamp() const;
    MessageType type() const;

    SharedStructure internalStructure();
    const SharedStructure internalStructure() const;

    quint32 sequenceNumber() const;
    void setSequenceNumber(quint32 num);
};

/*! \headerfile message.h <QGst/Message>
 * \brief Wrapper class for messages of type QGst::MessageEos
 */
class EosMessage : public Message
{
    QGST_WRAPPER_DIFFERENT_C_CLASS(EosMessage, Message)
public:
    static EosMessagePtr create(const ObjectPtr & source);
};

/*! \headerfile message.h <QGst/Message>
 * \brief Wrapper class for messages of type QGst::MessageError
 */
class ErrorMessage : public Message
{
    QGST_WRAPPER_DIFFERENT_C_CLASS(ErrorMessage, Message)
public:
    static ErrorMessagePtr create(const ObjectPtr & source,
                                  const QGlib::Error & error, const char *debug);

    QGlib::Error error() const;
    QString debugMessage() const;
};

/*! \headerfile message.h <QGst/Message>
 * \brief Wrapper class for messages of type QGst::MessageWarning
 */
class WarningMessage : public Message
{
    QGST_WRAPPER_DIFFERENT_C_CLASS(WarningMessage, Message)
public:
    static WarningMessagePtr create(const ObjectPtr & source,
                                    const QGlib::Error & error, const char *debug);

    QGlib::Error error() const;
    QString debugMessage() const;
};

/*! \headerfile message.h <QGst/Message>
 * \brief Wrapper class for messages of type QGst::MessageInfo
 */
class InfoMessage : public Message
{
    QGST_WRAPPER_DIFFERENT_C_CLASS(InfoMessage, Message)
public:
    static InfoMessagePtr create(const ObjectPtr & source,
                                 const QGlib::Error & error, const char *debug);

    QGlib::Error error() const;
    QString debugMessage() const;
};

//TODO TagMessage

/*! \headerfile message.h <QGst/Message>
 * \brief Wrapper class for messages of type QGst::MessageBuffering
 */
class BufferingMessage : public Message
{
    QGST_WRAPPER_DIFFERENT_C_CLASS(BufferingMessage, Message)
public:
    static BufferingMessagePtr create(const ObjectPtr & source, int percent);

    int percent() const;
    BufferingMode mode() const;
    int averageInputRate() const;
    int averageOutputRate() const;
    qint64 bufferingTimeLeft() const;

    void setStats(BufferingMode mode, int avgIn, int avgOut, qint64 bufferingLeft);
};

/*! \headerfile message.h <QGst/Message>
 * \brief Wrapper class for messages of type QGst::MessageStateChanged
 */
class StateChangedMessage : public Message
{
    QGST_WRAPPER_DIFFERENT_C_CLASS(StateChangedMessage, Message)
public:
    static StateChangedMessagePtr create(const ObjectPtr & source,
                                         State oldState, State newState, State pending);

    State oldState() const;
    State newState() const;
    State pendingState() const;
};

//won't do: STATE_DIRTY (deprecated)

/*! \headerfile message.h <QGst/Message>
 * \brief Wrapper class for messages of type QGst::MessageStepDone
 */
class StepDoneMessage : public Message
{
    QGST_WRAPPER_DIFFERENT_C_CLASS(StepDoneMessage, Message)
public:
    static StepDoneMessagePtr create(const ObjectPtr & source, Format format, quint64 amount,
                                     double rate, bool flush, bool intermediate, quint64 duration, bool eos);

    Format format() const;
    quint64 amount() const;
    double rate() const;
    bool isFlushingStep() const;
    bool isIntermediateStep() const;
    quint64 duration() const;
    bool causedEos() const;
};

//TODO CLOCK_PROVIDE, CLOCK_LOST, NEW_CLOCK
//maybe do: STRUCTURE_CHANGE (internal)

/*! \headerfile message.h <QGst/Message>
 * \brief Wrapper class for messages of type QGst::MessageStreamStatus
 */
class StreamStatusMessage : public Message
{
    QGST_WRAPPER_DIFFERENT_C_CLASS(StreamStatusMessage, Message)
public:
    static StreamStatusMessagePtr create(const ObjectPtr & source,
                                         StreamStatusType type, const ElementPtr & owner);

    StreamStatusType type() const;
    ElementPtr owner() const;
    QGlib::Value streamStatusObject() const;
    void setStreamStatusObject(const QGlib::ValueBase & object);
};

/*! \headerfile message.h <QGst/Message>
 * \brief Wrapper class for messages of type QGst::MessageApplication
 */
class ApplicationMessage : public Message
{
    QGST_WRAPPER_DIFFERENT_C_CLASS(ApplicationMessage, Message)
public:
    static ApplicationMessagePtr create(const ObjectPtr & source,
                                        const StructureBase & structure = SharedStructure(NULL));
};

/*! \headerfile message.h <QGst/Message>
 * \brief Wrapper class for messages of type QGst::MessageElement
 */
class ElementMessage : public Message
{
    QGST_WRAPPER_DIFFERENT_C_CLASS(ElementMessage, Message)
public:
    static ElementMessagePtr create(const ObjectPtr & source,
                                    const StructureBase & structure = SharedStructure(NULL));
};

//maybe do: SEGMENT_START (internal)

/*! \headerfile message.h <QGst/Message>
 * \brief Wrapper class for messages of type QGst::MessageSegmentDone
 */
class SegmentDoneMessage : public Message
{
    QGST_WRAPPER_DIFFERENT_C_CLASS(SegmentDoneMessage, Message)
public:
    static SegmentDoneMessagePtr create(const ObjectPtr & source, Format format, qint64 position);

    Format format() const;
    qint64 position() const;
};

/*! \headerfile message.h <QGst/Message>
 * \brief Wrapper class for messages of type QGst::MessageDuration
 */
class DurationMessage : public Message
{
    QGST_WRAPPER_DIFFERENT_C_CLASS(DurationMessage, Message)
public:
    static DurationMessagePtr create(const ObjectPtr & source, Format format, qint64 duration);

    Format format() const;
    qint64 duration() const;
};

/*! \headerfile message.h <QGst/Message>
 * \brief Wrapper class for messages of type QGst::MessageLatency
 */
class LatencyMessage : public Message
{
    QGST_WRAPPER_DIFFERENT_C_CLASS(LatencyMessage, Message)
public:
    static LatencyMessagePtr create(const ObjectPtr & source);
};

//maybe do: ASYNC_START (internal)

/*! \headerfile message.h <QGst/Message>
 * \brief Wrapper class for messages of type QGst::MessageAsyncDone
 */
class AsyncDoneMessage : public Message
{
    QGST_WRAPPER_DIFFERENT_C_CLASS(AsyncDoneMessage, Message)
public:
    static AsyncDoneMessagePtr create(const ObjectPtr & source);
};

/*! \headerfile message.h <QGst/Message>
 * \brief Wrapper class for messages of type QGst::MessageRequestState
 */
class RequestStateMessage : public Message
{
    QGST_WRAPPER_DIFFERENT_C_CLASS(RequestStateMessage, Message)
public:
    static RequestStateMessagePtr create(const ObjectPtr & source, State state);

    State state() const;
};

/*! \headerfile message.h <QGst/Message>
 * \brief Wrapper class for messages of type QGst::MessageStepStart
 */
class StepStartMessage : public Message
{
    QGST_WRAPPER_DIFFERENT_C_CLASS(StepStartMessage, Message)
public:
    static StepStartMessagePtr create(const ObjectPtr & source, bool active, Format format,
                                      quint64 amount, double rate, bool flush, bool intermediate);
    bool isActive() const;
    Format format() const;
    quint64 amount() const;
    double rate() const;
    bool isFlushingStep() const;
    bool isIntermediateStep() const;
};

/*! \headerfile message.h <QGst/Message>
 * \brief Wrapper class for messages of type QGst::MessageQos
 */
class QosMessage : public Message
{
    QGST_WRAPPER_DIFFERENT_C_CLASS(QosMessage, Message)
public:
    static QosMessagePtr create(const ObjectPtr & source, bool live, quint64 runningTime,
                                quint64 streamTime, quint64 timestamp, quint64 duration);

    bool live() const;
    quint64 runningTime() const;
    quint64 streamTime() const;
    quint64 timestamp() const;
    quint64 duration() const;

    qint64 jitter() const;
    double proportion() const;
    int quality() const;
    void setValues(qint64 jitter, double proportion, int quality);

    Format format() const;
    quint64 processed() const;
    quint64 dropped() const;
    void setStats(Format format, quint64 processed, quint64 dropped);
};

} //namespace QGst

QGLIB_REGISTER_TYPE(QGst::Message)
QGST_REGISTER_SUBCLASS(Message, Eos)
QGST_REGISTER_SUBCLASS(Message, Error)
QGST_REGISTER_SUBCLASS(Message, Warning)
QGST_REGISTER_SUBCLASS(Message, Info)
QGST_REGISTER_SUBCLASS(Message, Buffering)
QGST_REGISTER_SUBCLASS(Message, StateChanged)
QGST_REGISTER_SUBCLASS(Message, StepDone)
QGST_REGISTER_SUBCLASS(Message, StreamStatus)
QGST_REGISTER_SUBCLASS(Message, Application)
QGST_REGISTER_SUBCLASS(Message, Element)
QGST_REGISTER_SUBCLASS(Message, SegmentDone)
QGST_REGISTER_SUBCLASS(Message, Duration)
QGST_REGISTER_SUBCLASS(Message, Latency)
QGST_REGISTER_SUBCLASS(Message, AsyncDone)
QGST_REGISTER_SUBCLASS(Message, RequestState)
QGST_REGISTER_SUBCLASS(Message, StepStart)
QGST_REGISTER_SUBCLASS(Message, Qos)

/*! \relates QGst::Message */
QDebug operator<<(QDebug debug, QGst::MessageType type);

/*! \relates QGst::Message */
QDebug operator<<(QDebug debug, const QGst::MessagePtr & message);

#endif
