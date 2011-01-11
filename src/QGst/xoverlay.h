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
#ifndef QGST_XOVERLAY_H
#define QGST_XOVERLAY_H

#include "global.h"
#include "../QGlib/object.h"
#include <QtGui/qwindowdefs.h>

class QRect;

namespace QGst {

/*! \interface XOverlay xoverlay.h <QGst/XOverlay>
 * \brief Wrapper class for GstXOverlay
 */
class QTGSTREAMER_EXPORT XOverlay : public QGlib::Interface
{
    QGST_WRAPPER(XOverlay)
public:
    void expose();
    void setWindowHandle(WId id);
    void enableEventHandling(bool enabled);
    bool setRenderRectangle(int x, int y, int width, int height);
    bool setRenderRectangle(const QRect & rect);
};

} //namespace QGst

QGST_REGISTER_TYPE(QGst::XOverlay)
QGLIB_REGISTER_INTERFACE(QGst::XOverlay)

#endif // QGST_XOVERLAY_H
