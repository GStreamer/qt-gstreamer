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
#ifndef _QTGSTREAMER_QGSTOBJECT_H
#define _QTGSTREAMER_QGSTOBJECT_H

#include <QtCore/QObject>
#include <QtCore/QVariant>
#include <QtCore/QSharedPointer>
typedef struct _GstObject GstObject;

namespace QtGstreamer {

class QGstObject;
typedef QSharedPointer<QGstObject> QGstObjectPtr;

class QGstObject : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(QGstObject)
public:
    static QGstObjectPtr fromGstObject(GstObject *gstObject);
    virtual ~QGstObject();

    QVariant property(const char *name) const;
    template <class T> T property(const char *name) const;
    void setProperty(const char *name, const QVariant & value);
    template <class T> void setProperty(const char *name, const T & value);

protected:
    QGstObject(GstObject *gstObject);
    GstObject *m_object;
};

template <class T>
T QGstObject::property(const char *name) const
{
    return property(name).value<T>();
}

template <class T>
void QGstObject::setProperty(const char *name, const T & value)
{
    setProperty(name, QVariant::fromValue<T>(value));
}

}

Q_DECLARE_METATYPE(QtGstreamer::QGstObjectPtr)

#endif
