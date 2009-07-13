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
#include "qgstghostpad.h"
#include <gst/gstghostpad.h>

namespace QtGstreamer {

QGstGhostPad::QGstGhostPad(const char *name, const QGstPadPtr & target)
    : QGstPad(gst_ghost_pad_new(name, GST_PAD(target->m_object)))
{
}

//static
QGstGhostPadPtr QGstGhostPad::newGhostPad(const char *name, const QGstPadPtr & target)
{
    return QGstGhostPadPtr(new QGstGhostPad(name, target));
}

QGstGhostPad::~QGstGhostPad()
{
}

}

#include "qgstghostpad.moc"
