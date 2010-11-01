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

/*! \headerfile element.h <QGst/Element>
 * \brief Wrapper class for GstElement
 */
class Element : public virtual Object
{
    QGST_WRAPPER(Element)
public:
    StateChangeReturn getState(State *state, State *pending, ClockTime timeout);
    StateChangeReturn setState(State state);
    bool syncStateWithParent();

    bool stateIsLocked() const;
    bool setStateLocked(bool locked); //### is the return value ever needed?

    bool addPad(const PadPtr & pad);
    PadPtr getStaticPad(const char *name);
    PadPtr getRequestPad(const char *name);
    void releaseRequestPad(const PadPtr & pad);

    ClockPtr clock() const;
    bool setClock(const ClockPtr & clock);

    bool link(const char *srcPadName, const ElementPtr & dest,
              const char *sinkPadName, const CapsPtr & filter = CapsPtr());
    bool link(const char *srcPadName, const ElementPtr & dest,
              const CapsPtr & filter = CapsPtr());
    bool link(const ElementPtr & dest, const char *sinkPadName,
              const CapsPtr & filter = CapsPtr());
    bool link(const ElementPtr & dest, const CapsPtr & filter = CapsPtr());

    void unlink(const char *srcPadName, const ElementPtr & dest,
                const char *sinkPadName = NULL);
    void unlink(const ElementPtr & dest, const char *sinkPadName = NULL);

    void query(const QueryPtr & query);
    bool sendEvent(const EventPtr & event);
    bool seek(const Format format, SeekFlags flags, quint64 position);
};

}

QGLIB_REGISTER_TYPE(QGst::Element)

#endif // QGST_ELEMENT_H
