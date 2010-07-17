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
#include "xoverlay.h"
#include <gst/interfaces/xoverlay.h>
#include <QtCore/QRect>

namespace QGst {

void XOverlay::expose()
{
    gst_x_overlay_expose(object<GstXOverlay>());
}

void XOverlay::setWindowId(ulong id)
{
    gst_x_overlay_set_xwindow_id(object<GstXOverlay>(), id);
}

void XOverlay::setHandleEvents(bool enabled)
{
    gst_x_overlay_handle_events(object<GstXOverlay>(), enabled);
}

bool XOverlay::setRenderRectangle(int x, int y, int width, int height)
{
    return gst_x_overlay_set_render_rectangle(object<GstXOverlay>(), x, y, width, height);
}

bool XOverlay::setRenderRectangle(const QRect& rect)
{
    return setRenderRectangle(rect.x(), rect.y(), rect.width(), rect.height());
}

} //namespace QGst
