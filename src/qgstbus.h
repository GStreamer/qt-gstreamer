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
#ifndef _QTGSTREAMER_QGSTBUS_H
#define _QTGSTREAMER_QGSTBUS_H

#include "qgstobject.h"

namespace QtGstreamer {

class QGstBusPrivate;

class QGstBus : public QGstObject
{
    Q_OBJECT
    Q_DISABLE_COPY(QGstBus)
    QGST_WRAPPER(GstBus)
    friend class QGstBusPrivate;
public:
    static QGstBusPtr fromGstBus(GstBus *gstBus);
    virtual ~QGstBus();

    void addSignalWatch();
    void removeSignalWatch();

Q_SIGNALS:
    void message(const QtGstreamer::QGstMessagePtr & message);

protected:
    QGstBus(GstBus *gstBus);
};

}

Q_DECLARE_METATYPE(QtGstreamer::QGstBusPtr)

#endif
