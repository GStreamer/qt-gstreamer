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
typedef struct _GstElement GstElement;

namespace QtGstreamer {

class QGstPad;

class QGstElement : public QGstObject
{
    Q_OBJECT
    Q_DISABLE_COPY(QGstElement)
public:
    enum State { VoidPending, Null, Ready, Paused, Playing };
    enum StateChangeReturn { StateChangeFailure, StateChangeSuccess,
                             StateChangeAsync, StateChangeNoPreroll };

    explicit QGstElement(GstElement *gstElement, QObject *parent = 0);
    virtual ~QGstElement();

    State currentState() const;
    StateChangeReturn setState(State state);

    bool addPad(QGstPad *pad);
    QGstPad *getStaticPad(const char *name);
    QGstPad *getRequestPad(const char *name);

    static bool link(QGstElement *element1, QGstElement *element2,
                     QGstElement *element3 = NULL, QGstElement *element4 = NULL,
                     QGstElement *element5 = NULL, QGstElement *element6 = NULL,
                     QGstElement *element7 = NULL, QGstElement *element8 = NULL,
                     QGstElement *element9 = NULL, QGstElement *element10 = NULL);

    static void unlink(QGstElement *element1, QGstElement *element2,
                       QGstElement *element3 = NULL, QGstElement *element4 = NULL,
                       QGstElement *element5 = NULL, QGstElement *element6 = NULL,
                       QGstElement *element7 = NULL, QGstElement *element8 = NULL,
                       QGstElement *element9 = NULL, QGstElement *element10 = NULL);
};

}

#endif
