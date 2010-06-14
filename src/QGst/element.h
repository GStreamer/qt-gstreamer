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
#ifndef QGST_ELEMENT_H
#define QGST_ELEMENT_H

#include "object.h"
#include "caps.h"

namespace QGst {

class Element : public virtual Object
{
    QGST_WRAPPER(Element)
public:
    StateChangeReturn getState(State *state, State *pending, quint64 timeout);
    StateChangeReturn setState(State state);
    bool syncStateWithParent();

    bool stateIsLocked() const;
    bool setStateLocked(bool locked); //### is the return value ever needed?

    bool addPad(const PadPtr & pad);
    PadPtr getStaticPad(const QGlib::String & name);
    PadPtr getRequestPad(const QGlib::String & name);
    void releaseRequestPad(const PadPtr & pad);

    bool link(const QGlib::String & srcPadName, const ElementPtr & dest,
              const QGlib::String & sinkPadName, const CapsPtr & filter = CapsPtr());
    bool link(const QGlib::String & srcPadName, const ElementPtr & dest,
              const CapsPtr & filter = CapsPtr());
    bool link(const ElementPtr & dest, const QGlib::String & sinkPadName,
              const CapsPtr & filter = CapsPtr());
    bool link(const ElementPtr & dest, const CapsPtr & filter = CapsPtr());

    void unlink(const QGlib::String & srcPadName, const ElementPtr & dest,
                const QGlib::String & sinkPadName = QGlib::String());
    void unlink(const ElementPtr & dest, const QGlib::String & sinkPadName = QGlib::String());
};

}

QGLIB_REGISTER_TYPE(QGst::Element)

#endif // QGST_ELEMENT_H
