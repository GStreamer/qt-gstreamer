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
#ifndef QTGSTREAMER_QGSTMESSAGE_H
#define QTGSTREAMER_QGSTMESSAGE_H

#include "qgstminiobject.h"

namespace QtGstreamer {

class QGstMessage : public QGstMiniObject
{
    Q_DISABLE_COPY(QGstMessage)
    QGST_WRAPPER(GstMessage)
public:
    enum Type {
        Unknown           = 0,
        EOS               = (1 << 0),
        Error             = (1 << 1),
        Warning           = (1 << 2),
        Info              = (1 << 3),
        Tag               = (1 << 4),
        Buffering         = (1 << 5),
        StateChanged      = (1 << 6),
        StateDirty        = (1 << 7),
        StepDone          = (1 << 8),
        ClockProvide      = (1 << 9),
        ClockLost         = (1 << 10),
        NewClock          = (1 << 11),
        StructureChange   = (1 << 12),
        StreamStatus      = (1 << 13),
        Application       = (1 << 14),
        Element           = (1 << 15),
        SegmentStart      = (1 << 16),
        SegmentDone       = (1 << 17),
        Duration          = (1 << 18),
        Latency           = (1 << 19),
        AsyncStart        = (1 << 20),
        AsyncDone         = (1 << 21),
        RequestState      = (1 << 22),
        StepStart         = (1 << 23),
        Any               = ~0
    };

    static QGstMessagePtr fromGstMessage(GstMessage *message);
    virtual ~QGstMessage();

    QGstObjectPtr source() const;
    quint64 timestamp() const;

    Type type() const;
    QByteArray typeName() const;
    static QByteArray typeToName(Type type);

    quint32 sequenceNumber() const;
    void setSequenceNumber(quint32 num);

    struct ErrorStruct {
        int code; ///< The GError code field
        QByteArray message; ///< The GError message
        QByteArray debugMessage; ///< The optional debug string of GstMessage
    };
    ErrorStruct parseError() const;

    typedef ErrorStruct WarningStruct;
    WarningStruct parseWarning() const;

    typedef ErrorStruct InfoStruct;
    InfoStruct parseInfo() const;

protected:
    QGstMessage(GstMessage *message);
};

}

QDebug operator<<(QDebug debug, const QtGstreamer::QGstMessagePtr & message);
Q_DECLARE_METATYPE(QtGstreamer::QGstMessagePtr);

#endif
