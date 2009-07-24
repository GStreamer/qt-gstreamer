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
#include "qgstxoverlay.h"
#include <gst/interfaces/xoverlay.h>

namespace QtGstreamer {

QGstXOverlay::QGstXOverlay(const QGstElementPtr & element)
    : m_element(element)
{
}

QGstXOverlay::~QGstXOverlay()
{
}

//static
bool QGstXOverlay::isXOverlay(const QGstElementPtr & element)
{
    return GST_IS_X_OVERLAY(element->m_object);
}

void QGstXOverlay::setXWindowId(unsigned long id)
{
    gst_x_overlay_set_xwindow_id(GST_X_OVERLAY(m_element->m_object), id);
}

void QGstXOverlay::expose()
{
    gst_x_overlay_expose(GST_X_OVERLAY(m_element->m_object));
}

void QGstXOverlay::handleEvents(bool handle_events)
{
    gst_x_overlay_handle_events(GST_X_OVERLAY(m_element->m_object), handle_events);
}

}
