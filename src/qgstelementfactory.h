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
#ifndef _QTGSTREAMER_QGSTELEMENTFACTORY_H
#define _QTGSTREAMER_QGSTELEMENTFACTORY_H

#include "qgstobject.h"

namespace QtGstreamer {

class QGstElementFactory : public QGstObject //FIXME public QGstPluginFeature
{
    Q_OBJECT
    Q_DISABLE_COPY(QGstElementFactory)
    QGST_WRAPPER(GstElementFactory)
public:
    static QGstElementFactoryPtr fromGstElementFactory(GstElementFactory *factory);
    virtual ~QGstElementFactory();

    static QGstElementFactoryPtr find(const char *factoryName);
    static QGstElementPtr make(const char *factoryName, const char *elementName = 0);

protected:
    QGstElementFactory(GstElementFactory *factory);
};

}

Q_DECLARE_METATYPE(QtGstreamer::QGstElementFactoryPtr)

#endif
