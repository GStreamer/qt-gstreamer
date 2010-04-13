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
#include "element.h"
#include "pad.h"
#include <gst/gstelement.h>
#include <gst/gstutils.h>

namespace QGst {

StateChangeReturn Element::getState(State *state, State *pending, quint64 timeout)
{
    GstState curState, pendingState;
    GstStateChangeReturn result = gst_element_get_state(GST_ELEMENT(m_object),
                                                        &curState, &pendingState, timeout);
    if (state) {
        *state = static_cast<State>(curState);
    }
    if (pending) {
        *pending = static_cast<State>(pendingState);
    }
    return static_cast<StateChangeReturn>(result);
}

StateChangeReturn Element::setState(State state)
{
    return static_cast<StateChangeReturn>(gst_element_set_state(GST_ELEMENT(m_object),
                                                                static_cast<GstState>(state)));
}

bool Element::syncStateWithParent()
{
   return gst_element_sync_state_with_parent(GST_ELEMENT(m_object));
}

bool Element::stateIsLocked() const
{
    return gst_element_is_locked_state(GST_ELEMENT(m_object));
}

bool Element::setStateLocked(bool locked)
{
    return gst_element_set_locked_state(GST_ELEMENT(m_object), locked);
}

bool Element::addPad(const PadPtr & pad)
{
    return gst_element_add_pad(GST_ELEMENT(m_object), pad);
}

PadPtr Element::getStaticPad(const QString & name)
{
    GstPad *pad = gst_element_get_static_pad(GST_ELEMENT(m_object), name.toUtf8());
    return PadPtr::wrap(pad, false);
}

PadPtr Element::getRequestPad(const QString & name)
{
    GstPad *pad = gst_element_get_request_pad(GST_ELEMENT(m_object), name.toUtf8());
    return PadPtr::wrap(pad, false);
}

void Element::releaseRequestPad(const PadPtr & pad)
{
    gst_element_release_request_pad(GST_ELEMENT(m_object), pad);
}

bool Element::link(const QString & srcPadName, const ElementPtr & dest,
                   const QString& sinkPadName, const CapsPtr & filter)
{
    return gst_element_link_pads_filtered(GST_ELEMENT(m_object),
                                          srcPadName.isEmpty() ? NULL : srcPadName.toUtf8().constData(),
                                          dest,
                                          sinkPadName.isEmpty() ? NULL : sinkPadName.toUtf8().constData(),
                                          filter);
}

bool Element::link(const QString & srcPadName, const ElementPtr & dest, const CapsPtr & filter)
{
    return link(srcPadName, dest, QString(), filter);
}

bool Element::link(const ElementPtr & dest, const QString & sinkPadName, const CapsPtr & filter)
{
    return link(QString(), dest, sinkPadName, filter);
}

bool Element::link(const ElementPtr & dest, const CapsPtr & filter)
{
    return link(QString(), dest, QString(), filter);
}

void Element::unlink(const QString & srcPadName, const ElementPtr & dest, const QString & sinkPadName)
{
    gst_element_unlink_pads(GST_ELEMENT(m_object),
                            srcPadName.isEmpty() ? NULL : srcPadName.toUtf8().constData(),
                            dest,
                            sinkPadName.isEmpty() ? NULL : sinkPadName.toUtf8().constData());
}

void Element::unlink(const ElementPtr & dest, const QString & sinkPadName)
{
    unlink(QString(), dest, sinkPadName);
}

}
