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
#ifndef _QTGSTREAMER_QGSTELEMENT_H
#define _QTGSTREAMER_QGSTELEMENT_H

#include "qgstobject.h"

namespace QtGstreamer {

class QGstElementPrivate;

class QGstElement : public QGstObject
{
    Q_OBJECT
    Q_DISABLE_COPY(QGstElement)
    friend class QGstBin;
    friend class QGstXOverlay;
    friend class QGstElementPrivate;
public:
    enum State { VoidPending, Null, Ready, Paused, Playing };
    enum StateChangeReturn { StateChangeFailure, StateChangeSuccess,
                             StateChangeAsync, StateChangeNoPreroll };

    static QGstElementPtr fromGstElement(GstElement *gstElement);
    virtual ~QGstElement();

    State currentState();
    StateChangeReturn getState(State *state, State *pending, quint64 timeout);
    StateChangeReturn setState(State state);
    bool syncStateWithParent();

    bool addPad(const QGstPadPtr & pad);
    QGstPadPtr getStaticPad(const char *name);
    QGstPadPtr getRequestPad(const char *name);
    void releaseRequestPad(const QGstPadPtr & pad);

    QGstBusPtr getBus();

    static bool link(const QGstElementPtr & element1, const QGstElementPtr & element2,
                     const QGstElementPtr & element3 = QGstElementPtr(),
                     const QGstElementPtr & element4 = QGstElementPtr(),
                     const QGstElementPtr & element5 = QGstElementPtr(),
                     const QGstElementPtr & element6 = QGstElementPtr(),
                     const QGstElementPtr & element7 = QGstElementPtr(),
                     const QGstElementPtr & element8 = QGstElementPtr(),
                     const QGstElementPtr & element9 = QGstElementPtr(),
                     const QGstElementPtr & element10 = QGstElementPtr());

    static void unlink(const QGstElementPtr & element1, const QGstElementPtr & element2,
                       const QGstElementPtr & element3 = QGstElementPtr(),
                       const QGstElementPtr & element4 = QGstElementPtr(),
                       const QGstElementPtr & element5 = QGstElementPtr(),
                       const QGstElementPtr & element6 = QGstElementPtr(),
                       const QGstElementPtr & element7 = QGstElementPtr(),
                       const QGstElementPtr & element8 = QGstElementPtr(),
                       const QGstElementPtr & element9 = QGstElementPtr(),
                       const QGstElementPtr & element10 = QGstElementPtr());

Q_SIGNALS:
    void noMorePads();
    void padAdded(QtGstreamer::QGstPadPtr pad);
    void padRemoved(QtGstreamer::QGstPadPtr pad);

protected:
    QGstElement(GstElement *gstElement);
};

}

Q_DECLARE_METATYPE(QtGstreamer::QGstElementPtr)

#endif
