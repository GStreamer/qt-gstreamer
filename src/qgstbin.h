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
#ifndef QTGSTREAMER_QGSTBIN_H
#define QTGSTREAMER_QGSTBIN_H

#include "qgstelement.h"

namespace QtGstreamer {

class QGstBin : public QGstElement
{
    Q_OBJECT
    Q_DISABLE_COPY(QGstBin)
    QGST_WRAPPER(GstBin)
public:
    static QGstBinPtr newBin(const char *name = 0);
    static QGstBinPtr fromGstBin(GstBin *gstBin);
    virtual ~QGstBin();

    bool add(const QGstElementPtr & element);
    QGstBin & operator<<(const QGstElementPtr & element);
    bool remove(const QGstElementPtr & element);

    QGstElementPtr getByName(const char *name);

protected:
    QGstBin(const char *name = 0);
    QGstBin(GstBin *gstBin);
};

}

Q_DECLARE_METATYPE(QtGstreamer::QGstBinPtr)

#endif
