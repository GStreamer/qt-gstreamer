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
#ifndef QGST_BUS_H
#define QGST_BUS_H

#include "object.h"

namespace QGst {

class Bus : public Object
{
    QGST_WRAPPER(Bus)
public:
    static BusPtr create();

    bool post(const MessagePtr & message);

    bool hasPendingMessages() const;
    MessagePtr peek() const;
    MessagePtr pop(ClockTime timeout = 0);
    MessagePtr pop(MessageType type, ClockTime timeout = 0);

    void setFlushing(bool flush);

    void enableSyncMessageEmission();
    void disableSyncMessageEmission();
};

} //namespace QGst

QGLIB_REGISTER_TYPE(QGst::Bus)

#endif //QGST_BUS_H
