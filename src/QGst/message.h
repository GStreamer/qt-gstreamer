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

namespace QGst {

class Message : public MiniObject
{
    QGST_WRAPPER(Message)
public:
    ObjectPtr source() const;
    quint64 timestamp() const;

    MessageType type() const;

    quint32 sequenceNumber() const;
    void setSequenceNumber(quint32 num);
};

}

QGLIB_REGISTER_TYPE(QGst::Message)

QDebug operator<<(QDebug debug, QGst::MessageType type);
QDebug operator<<(QDebug debug, const QGst::MessagePtr & message);

#endif
