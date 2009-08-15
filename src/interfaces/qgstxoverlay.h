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
#ifndef QTGSTREAMER_QGSTXOVERLAY_H
#define QTGSTREAMER_QGSTXOVERLAY_H

#include "../qgstelement.h"

namespace QtGstreamer {

class QGstXOverlay
{
public:
    QGstXOverlay(const QGstElementPtr & element);
    ~QGstXOverlay();

    static bool isXOverlay(const QGstElementPtr & element);

    void setXWindowId(unsigned long id);
    void expose();
    void handleEvents(bool handle_events);

private:
    QGstElementPtr m_element;
};

}

#endif
