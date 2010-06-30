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
#include "pad.h"
#include "caps.h"
#include "element.h"
#include <QtCore/QDebug>
#include <gst/gstpad.h>
#include <gst/gstutils.h>

namespace QGst {

//static
PadPtr Pad::create(PadDirection direction, const QGlib::String & name)
{
    GstPad *pad = gst_pad_new(name, static_cast<GstPadDirection>(direction));
    return PadPtr::wrap(pad, false);
}

PadDirection Pad::direction() const
{
    return static_cast<PadDirection>(gst_pad_get_direction(GST_PAD(object())));
}

ElementPtr Pad::parentElement() const
{
    return ElementPtr::wrap(gst_pad_get_parent_element(GST_PAD(object())), false);
}

PadPtr Pad::peer() const
{
    return PadPtr::wrap(gst_pad_get_peer(GST_PAD(object())), false);
}

bool Pad::isLinked() const
{
    return gst_pad_is_linked(GST_PAD(object()));
}

bool Pad::canLink(const PadPtr & sink) const
{
    return gst_pad_can_link(GST_PAD(object()), sink);
}

PadLinkReturn Pad::link(const PadPtr & sink)
{
    return static_cast<PadLinkReturn>(gst_pad_link(GST_PAD(object()), sink));
}

bool Pad::unlink(const PadPtr & sink)
{
    return gst_pad_unlink(GST_PAD(object()), sink);
}

CapsPtr Pad::caps() const
{
    return CapsPtr::wrap(gst_pad_get_caps_reffed(GST_PAD(object())), false);
}

CapsPtr Pad::allowedCaps() const
{
    return CapsPtr::wrap(gst_pad_get_allowed_caps(GST_PAD(object())), false);
}

CapsPtr Pad::negotiatedCaps() const
{
    return CapsPtr::wrap(gst_pad_get_negotiated_caps(GST_PAD(object())), false);
}

bool Pad::setCaps(const CapsPtr & caps)
{
    return gst_pad_set_caps(GST_PAD(object()), caps);
}

bool Pad::isActive() const
{
    return gst_pad_is_active(GST_PAD(object()));
}

bool Pad::isBlocked() const
{
    return gst_pad_is_blocked(GST_PAD(object()));
}

bool Pad::isBlocking() const
{
    return gst_pad_is_blocking(GST_PAD(object()));
}

bool Pad::setBlocked(bool blocked)
{
    return gst_pad_set_blocked(GST_PAD(object()), blocked);
}

}
