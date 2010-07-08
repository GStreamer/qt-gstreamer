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
#ifndef QGST_ENUMS_H
#define QGST_ENUMS_H

#include "../QGlib/type.h"
#include <QtCore/QFlags>

namespace QGst {
    enum MiniObjectFlag {
        MiniObjectFlagReadonly = (1<<0),
        /*padding*/
        MiniObjectFlagLast = (1<<4)
    };
    Q_DECLARE_FLAGS(MiniObjectFlags, MiniObjectFlag);
}
Q_DECLARE_OPERATORS_FOR_FLAGS(QGst::MiniObjectFlags);
QGLIB_REGISTER_TYPE(QGst::MiniObjectFlags);


namespace QGst {
    enum ObjectFlag {
        ObjectDisposing = (1<<0),
        ObjectFloating = (1<<1),
        /*padding*/
        ObjectFlagLast = (1<<4)
    };
    Q_DECLARE_FLAGS(ObjectFlags, ObjectFlag);
}
Q_DECLARE_OPERATORS_FOR_FLAGS(QGst::ObjectFlags);
QGLIB_REGISTER_TYPE(QGst::ObjectFlags);


namespace QGst {
    enum State {
        StateVoidPending = 0,
        StateNull = 1,
        StateReady = 2,
        StatePaused = 3,
        StatePlaying = 4
    };
}
QGLIB_REGISTER_TYPE(QGst::State)

namespace QGst {
    enum StateChange {
        StateChangeNullToReady = (StateNull<<3) | StateReady,
        StateChangeReadyToPaused = (StateReady<<3) | StatePaused,
        StateChangePausedToPlaying = (StatePaused<<3) | StatePlaying,
        StateChangePlayingToPaused = (StatePlaying<<3) | StatePaused,
        StateChangePausedToReady = (StatePaused<<3) | StateReady,
        StateChangeReadyToNull = (StateReady<<3) | StateNull
    };
}
QGLIB_REGISTER_TYPE(QGst::StateChange)

namespace QGst {
    enum StateChangeReturn {
        StateChangeFailure = 0,
        StateChangeSuccess = 1,
        StateChangeAsync = 2,
        StateChangeNoPreroll = 3
    };
}
QGLIB_REGISTER_TYPE(QGst::StateChangeReturn)


namespace QGst {
    enum PadDirection {
        PadUnknown,
        PadSrc,
        PadSink
    };
}
QGLIB_REGISTER_TYPE(QGst::PadDirection)

namespace QGst {
    enum PadFlag {
        //codegen: PadInGetCaps=PAD_IN_GETCAPS, PadInSetCaps=PAD_IN_SETCAPS
        PadBlocked = (ObjectFlagLast << 0),
        PadFlushing = (ObjectFlagLast << 1),
        PadInGetCaps = (ObjectFlagLast << 2),
        PadInSetCaps = (ObjectFlagLast << 3),
        PadBlocking = (ObjectFlagLast << 4),
        /*padding*/
        PadFlagLast = (ObjectFlagLast << 8)
    };
    Q_DECLARE_FLAGS(PadFlags, PadFlag);
}
Q_DECLARE_OPERATORS_FOR_FLAGS(QGst::PadFlags);
QGLIB_REGISTER_TYPE(QGst::PadFlags);

namespace QGst {
    enum PadLinkReturn {
        //codegen: PadLinkNoFormat=PAD_LINK_NOFORMAT, PadLinkNoSched=PAD_LINK_NOSCHED
        PadLinkOk = 0,
        PadLinkWrongHierarchy = -1,
        PadLinkWasLinked = -2,
        PadLinkWrongDirection = -3,
        PadLinkNoFormat = -4,
        PadLinkNoSched = -5,
        PadLinkRefused = -6
    };
}
QGLIB_REGISTER_TYPE(QGst::PadLinkReturn)

namespace QGst {
    enum FlowReturn {
        FlowCustomSuccess = 100,
        FlowResend = 1,
        FlowOk = 0,
        FlowNotLinked = -1,
        FlowWrongState = -2,
        FlowUnexpected = -3,
        FlowNotNegotiated = -4,
        FlowError = -5,
        FlowNotSupported = -6,
        FlowCustomError = -100
    };
}
QGLIB_REGISTER_TYPE(QGst::FlowReturn)

namespace QGst {
    enum ActivateMode {
        ActivateNone,
        ActivatePush,
        ActivatePull
    };
}
QGLIB_REGISTER_TYPE(QGst::ActivateMode)


namespace QGst {
    enum Rank {
        RankNone = 0,
        RankMarginal = 64,
        RankSecondary = 128,
        RankPrimary = 256
    };
}
QGLIB_REGISTER_TYPE(QGst::Rank)


namespace QGst {
    enum MessageType {
        MessageUnknown         = 0,
        MessageEos             = (1 << 0),
        MessageError           = (1 << 1),
        MessageWarning         = (1 << 2),
        MessageInfo            = (1 << 3),
        MessageTag             = (1 << 4),
        MessageBuffering       = (1 << 5),
        MessageStateChanged    = (1 << 6),
        MessageStateDirty      = (1 << 7),
        MessageStepDone        = (1 << 8),
        MessageClockProvide    = (1 << 9),
        MessageClockLost       = (1 << 10),
        MessageNewClock        = (1 << 11),
        MessageStructureChange = (1 << 12),
        MessageStreamStatus    = (1 << 13),
        MessageApplication     = (1 << 14),
        MessageElement         = (1 << 15),
        MessageSegmentStart    = (1 << 16),
        MessageSegmentDone     = (1 << 17),
        MessageDuration        = (1 << 18),
        MessageLatency         = (1 << 19),
        MessageAsyncStart      = (1 << 20),
        MessageAsyncDone       = (1 << 21),
        MessageRequestState    = (1 << 22),
        MessageStepStart       = (1 << 23),
        MessageAny             = ~0
    };
}
QGLIB_REGISTER_TYPE(QGst::MessageType)


namespace QGst {
    enum ParseError {
        ParseErrorSyntax,
        ParseErrorNoSuchElement,
        ParseErrorNoSuchProperty,
        ParseErrorLink,
        ParseErrorCouldNotSetProperty,
        ParseErrorEmptyBin,
        ParseErrorEmpty
    };
}
QGLIB_REGISTER_TYPE(QGst::ParseError)


namespace QGst {
    enum UriType {
        UriUnknown,
        UriSink,
        UriSrc
    };
}
QGLIB_REGISTER_TYPE(QGst::UriType)


#endif
