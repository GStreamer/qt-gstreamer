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
#ifndef _QTGSTREAMER_QGSTBIN_H
#define _QTGSTREAMER_QGSTBIN_H

#include "qgstelement.h"
typedef struct _GstBin GstBin;

namespace QtGstreamer {

class QGstBin : public QGstElement
{
    Q_OBJECT
    Q_DISABLE_COPY(QGstBin)
public:
    explicit QGstBin(const char *name = 0, QObject *parent = 0);
    explicit QGstBin(GstBin *gstBin, QObject *parent = 0);
    virtual ~QGstBin();

    bool add(QGstElement *element);
    QGstBin & operator<<(QGstElement *element);

    bool remove(QGstElement *element);
};

}

#endif
