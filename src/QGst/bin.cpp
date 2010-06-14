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
#include "bin.h"
#include "pad.h"
#include <gst/gstbin.h>
#include <gst/gstutils.h>

namespace QGst {

//static
BinPtr Bin::newBin(const QGlib::String & name)
{
    GstElement *bin = gst_bin_new(name);
    gst_object_ref_sink(GST_OBJECT(bin));
    return BinPtr::wrap(GST_BIN(bin), false);
}

bool Bin::add(const ElementPtr & element)
{
    return gst_bin_add(GST_BIN(m_object), element);
}

bool Bin::remove(const ElementPtr & element)
{
    return gst_bin_remove(GST_BIN(m_object), element);
}

ElementPtr Bin::getElementByName(const QGlib::String & name, RecursionType r) const
{
    GstElement *e = NULL;
    switch(r) {
    case RecurseDown:
        e = gst_bin_get_by_name(GST_BIN(m_object), name);
        break;
    case RecurseUp:
        e = gst_bin_get_by_name_recurse_up(GST_BIN(m_object), name);
        break;
    default:
        Q_ASSERT_X(false, "QGst::Bin::getByName", "Invalid RecursionType");
    }
    return ElementPtr::wrap(e, false);
}

ElementPtr Bin::getElementByInterface(QGlib::Type interfaceType) const
{
    return ElementPtr::wrap(gst_bin_get_by_interface(GST_BIN(m_object), interfaceType), false);
}

PadPtr Bin::findUnlinkedPad(PadDirection direction) const
{
    return PadPtr::wrap(gst_bin_find_unlinked_pad(GST_BIN(m_object),
                                                  static_cast<GstPadDirection>(direction)), false);
}

bool Bin::recalculateLatency()
{
    return gst_bin_recalculate_latency(GST_BIN(m_object));
}

}
