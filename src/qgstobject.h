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
#ifndef QTGSTREAMER_QGSTOBJECT_H
#define QTGSTREAMER_QGSTOBJECT_H

#include "qgstvalue.h"
#include <QtCore/QObject>

namespace QtGstreamer {

class QGstObject : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(QGstObject)
    QGST_WRAPPER(GstObject)
public:
    static QGstObjectPtr fromGstObject(GstObject *gstObject);
    virtual ~QGstObject();

    QGstValue property(const char *name) const;
    template <class T> T property(const char *name) const;
    void setProperty(const char *name, const QGstValue & value);

    /** Returns the internal GstObject pointer, without any extra reference count.
     * This is useful if you want to use a native function on the object.
     * @sa getNativeObject
     */
    inline GstObject *peekNativeObject();

    /** Returns the internal GstObject with an extra reference count on it.
     * This is useful if you want to extract the GstObject and destroy the QGstObject.
     * Equivalent to gst_object_ref(qgstobjectptr->peekNativeObject());
     * @sa peekNativeObject
     */
    GstObject *getNativeObject();

protected:
    QGstObject(GstObject *gstObject);
    GstObject *m_object;
};

template <class T>
T QGstObject::property(const char *name) const
{
    return property(name).value<T>();
}

inline GstObject *QGstObject::peekNativeObject()
{
    return m_object;
}

}

Q_DECLARE_METATYPE(QtGstreamer::QGstObjectPtr)

#endif
