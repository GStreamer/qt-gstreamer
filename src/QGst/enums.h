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

#include "../QGlib/global.h"
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

#endif
