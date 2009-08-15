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
#ifndef QTGSTREAMER_QGSTPAD_H
#define QTGSTREAMER_QGSTPAD_H

#include "qgstobject.h"

namespace QtGstreamer {

class QGstPad : public QGstObject
{
    Q_OBJECT
    Q_DISABLE_COPY(QGstPad)
    QGST_WRAPPER(GstPad)
public:
    enum LinkReturn { Ok = 0, WrongHierarchy = -1, WasLinked = -2, WrongDirection = -3,
                      NoFormat = -4, NoSched = -5, Refused = -6 };
    enum Direction { Unknown, Src, Sink };

    static QGstPadPtr fromGstPad(GstPad *gstPad);
    virtual ~QGstPad();

    Direction direction() const;

    bool isLinked() const;
    LinkReturn link(const QGstPadPtr &other);
    bool unlink(const QGstPadPtr & other);
    bool canLink(const QGstPadPtr & other);

    QGstCapsPtr getCaps();
    bool setBlocked(bool blocked);

protected:
    QGstPad(GstPad *gstPad);
};

}

Q_DECLARE_METATYPE(QtGstreamer::QGstPadPtr)

#endif
