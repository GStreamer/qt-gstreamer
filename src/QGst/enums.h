/*
    Copyright (C) 2010  George Kiagiadakis <kiagiadakis.george@gmail.com>
    Copyright (C) 2010  Collabora Multimedia.
      @author Mauricio Piacentini <mauricio.piacentini@collabora.co.uk>

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

#include "global.h"
#include <QtCore/QFlags>

namespace QGst {
    enum MiniObjectFlag {
        MiniObjectFlagReadonly = (1<<0),
        /*padding*/
        MiniObjectFlagLast = (1<<4)
    };
    Q_DECLARE_FLAGS(MiniObjectFlags, MiniObjectFlag);
    Q_DECLARE_OPERATORS_FOR_FLAGS(MiniObjectFlags);
}
QGST_REGISTER_TYPE(QGst::MiniObjectFlags);


namespace QGst {
    enum ObjectFlag {
        ObjectDisposing = (1<<0),
        ObjectFloating = (1<<1),
        /*padding*/
        ObjectFlagLast = (1<<4)
    };
    Q_DECLARE_FLAGS(ObjectFlags, ObjectFlag);
    Q_DECLARE_OPERATORS_FOR_FLAGS(ObjectFlags);
}
QGST_REGISTER_TYPE(QGst::ObjectFlags);


namespace QGst {
    enum State {
        StateVoidPending = 0,
        StateNull = 1,
        StateReady = 2,
        StatePaused = 3,
        StatePlaying = 4
    };
}
QGST_REGISTER_TYPE(QGst::State)

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
QGST_REGISTER_TYPE(QGst::StateChange)

namespace QGst {
    enum StateChangeReturn {
        StateChangeFailure = 0,
        StateChangeSuccess = 1,
        StateChangeAsync = 2,
        StateChangeNoPreroll = 3
    };
}
QGST_REGISTER_TYPE(QGst::StateChangeReturn)


namespace QGst {
    enum PadDirection {
        PadUnknown,
        PadSrc,
        PadSink
    };
}
QGST_REGISTER_TYPE(QGst::PadDirection)

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
    Q_DECLARE_OPERATORS_FOR_FLAGS(PadFlags);
}
QGST_REGISTER_TYPE(QGst::PadFlags);

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
QGST_REGISTER_TYPE(QGst::PadLinkReturn)

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
QGST_REGISTER_TYPE(QGst::FlowReturn)

namespace QGst {
    enum ActivateMode {
        ActivateNone,
        ActivatePush,
        ActivatePull
    };
}
QGST_REGISTER_TYPE(QGst::ActivateMode)


namespace QGst {
    enum Rank {
        RankNone = 0,
        RankMarginal = 64,
        RankSecondary = 128,
        RankPrimary = 256
    };
}
QGST_REGISTER_TYPE(QGst::Rank)


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
        MessageQos             = (1 << 24),
        MessageAny             = ~0
    };
}
QGST_REGISTER_TYPE(QGst::MessageType)


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
QGST_REGISTER_TYPE(QGst::ParseError)


namespace QGst {
    enum UriType {
        UriUnknown,
        UriSink,
        UriSrc
    };
}
QGST_REGISTER_TYPE(QGst::UriType)

namespace QGst {
    enum BufferingMode {
        BufferingStream,
        BufferingDownload,
        BufferingTimeshift,
        BufferingLive
    };
}
QGST_REGISTER_TYPE(QGst::BufferingMode)

namespace QGst {
    enum Format {
        FormatUndefined,
        FormatDefault,
        FormatBytes,
        FormatTime,
        FormatBuffers,
        FormatPercent
    };
}
QGST_REGISTER_TYPE(QGst::Format)

namespace QGst {
    enum StreamStatusType {
        StreamStatusTypeCreate = 0,
        StreamStatusTypeEnter = 1,
        StreamStatusTypeLeave = 2,
        StreamStatusTypeDestroy = 3,
        StreamStatusTypeStart = 8,
        StreamStatusTypePause = 9,
        StreamStatusTypeStop = 10
    };
}
QGST_REGISTER_TYPE(QGst::StreamStatusType)

namespace QGst {
    enum StreamVolumeFormat {
        StreamVolumeFormatLinear,
        StreamVolumeFormatCubic,
        StreamVolumeFormatDb
    };
}
QGST_REGISTER_TYPE(QGst::StreamVolumeFormat)

namespace QGst {
    enum ColorBalanceType {
        ColorBalanceHardware,
        ColorBalanceSoftware
    };
}
QGST_REGISTER_TYPE(QGst::ColorBalanceType)

namespace QGst {
    enum QueryType {
        QueryNone = 0,
        QueryPosition,
        QueryDuration,
        QueryLatency,
        QueryJitter,
        QueryRate,
        QuerySeeking,
        QuerySegment,
        QueryConvert,
        QueryFormats,
        QueryBuffering,
        QueryCustom,
        QueryUri
    };
}
QGST_REGISTER_TYPE(QGst::QueryType)

namespace QGst {
    enum BufferFlag {
        //codegen: BufferFlagReadOnly=BUFFER_FLAG_READONLY
        BufferFlagReadOnly = MiniObjectFlagReadonly,
        BufferFlagPreroll  = (MiniObjectFlagLast << 0),
        BufferFlagDiscont = (MiniObjectFlagLast << 1),
        BufferFlagInCaps = (MiniObjectFlagLast << 2),
        BufferFlagGap = (MiniObjectFlagLast << 3),
        BufferFlagDeltaUnit = (MiniObjectFlagLast << 4),
        BufferFlagMedia1 = (MiniObjectFlagLast << 5),
        BufferFlagMedia2 = (MiniObjectFlagLast << 6),
        BufferFlagMedia3 = (MiniObjectFlagLast << 7),
        BufferFlagLast = (MiniObjectFlagLast << 8)
    };
    Q_DECLARE_FLAGS(BufferFlags, BufferFlag);
    Q_DECLARE_OPERATORS_FOR_FLAGS(BufferFlags)
}
QGST_REGISTER_TYPE(QGst::BufferFlags) //codegen: GType=GST_TYPE_BUFFER_FLAG


namespace QGst {
    enum EventTypeFlag {
        EventTypeUpstream = 1 << 0,
        EventTypeDownstream = 1 << 1,
        EventTypeSerialized = 1 << 2,
        EventTypeBoth = (EventTypeUpstream | EventTypeDownstream)
    };
    Q_DECLARE_FLAGS(EventTypeFlags, EventTypeFlag);
    Q_DECLARE_OPERATORS_FOR_FLAGS(EventTypeFlags)
}
QGST_REGISTER_TYPE(QGst::EventTypeFlags)

namespace QGst {
    enum EventType {
        //codegen: EventNewSegment=EVENT_NEWSEGMENT, EventBufferSize=EVENT_BUFFERSIZE
        EventUnknown = (0 << 4),
        EventFlushStart = (1 << 4) | EventTypeBoth,
        EventFlushStop = (2 << 4) | EventTypeBoth | EventTypeSerialized,
        EventEos = (5 << 4) | EventTypeDownstream | EventTypeSerialized,
        EventNewSegment = (6 << 4) | EventTypeDownstream | EventTypeSerialized,
        EventTag = (7 << 4) | EventTypeDownstream | EventTypeSerialized,
        EventBufferSize = (8 << 4) | EventTypeDownstream | EventTypeSerialized,
        EventSinkMessage = (9 << 4) | EventTypeDownstream | EventTypeSerialized,
        EventQos = (15 << 4) | EventTypeUpstream,
        EventSeek = (16 << 4) | EventTypeUpstream,
        EventNavigation = (17 << 4) | EventTypeUpstream,
        EventLatency = (18 << 4) | EventTypeUpstream,
        EventStep = (19 << 4) | EventTypeUpstream,
        EventCustomUpstream = (32 << 4) | EventTypeUpstream,
        EventCustomDownstream = (32 << 4) | EventTypeDownstream | EventTypeSerialized,
        EventCustomDownstreamOob = (32 << 4) | EventTypeDownstream,
        EventCustomBoth = (32 << 4) | EventTypeBoth | EventTypeSerialized,
        EventCustomBothOob = (32 << 4) | EventTypeBoth
    };
}
QGST_REGISTER_TYPE(QGst::EventType)

namespace QGst {
    enum SeekFlag {
        SeekFlagNone = 0,
        SeekFlagFlush = (1 << 0),
        SeekFlagAccurate = (1 << 1),
        SeekFlagKeyUnit = (1 << 2),
        SeekFlagSegment = (1 << 3),
        SeekFlagSkip = (1 << 4)
    };
    Q_DECLARE_FLAGS(SeekFlags, SeekFlag);
    Q_DECLARE_OPERATORS_FOR_FLAGS(SeekFlags)
}
QGST_REGISTER_TYPE(QGst::SeekFlags)

namespace QGst {
    enum SeekType {
        SeekTypeNone = 0,
        SeekTypeCur = 1,
        SeekTypeSet = 2,
        SeekTypeEnd = 3
    };
}
QGST_REGISTER_TYPE(QGst::SeekType)

namespace QGst {
    enum TagMergeMode {
        TagMergeUndefined,
        TagMergeReplaceAll,
        TagMergeReplace,
        TagMergeAppend,
        TagMergePrepend,
        TagMergeKeep,
        TagMergeKeepAll,
        // add more
        TagMergeCount
    };
}
QGST_REGISTER_TYPE(QGst::TagMergeMode)

namespace QGst {
    enum TagFlag {
        TagFlagUndefined,
        TagFlagMeta,
        TagFlagEncoded,
        TagFlagDecoded,
        TagFlagCount
    };
    Q_DECLARE_FLAGS(TagFlags, TagFlag);
}
Q_DECLARE_OPERATORS_FOR_FLAGS(QGst::TagFlags)
QGST_REGISTER_TYPE(QGst::TagFlags) //codegen: GType=GST_TYPE_TAG_FLAG

namespace QGst {
    enum AppStreamType {
        /*! No seeking is supported in the stream, such as a live stream. */
        AppStreamTypeStream,
        /*! The stream is seekable but seeking might not be very
         * fast, such as data from a webserver. */
        AppStreamTypeSeekable,
        /*! The stream is seekable and seeking is fast, such as in a local file. */
        AppStreamTypeRandomAccess
    };
}
//FIXME: GST_TYPE_APP_STREAM_TYPE is not exported from libgstapp,
//so we have to reside in this method...
namespace QGlib {
    template <>
    struct GetTypeImpl<QGst::AppStreamType>
    {
        inline operator Type() { return Type::fromName("GstAppStreamType"); }
    };
}

#endif
