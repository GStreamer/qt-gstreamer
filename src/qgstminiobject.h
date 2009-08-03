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
#ifndef _QTGSTREAMER_QGSTMINIOBJECT_H
#define _QTGSTREAMER_QGSTMINIOBJECT_H

#include "qgstdeclarations.h"
#include <QtCore/QMetaType>

namespace QtGstreamer {

class QGstMiniObject
{
    Q_DISABLE_COPY(QGstMiniObject)
    QGST_WRAPPER(GstMiniObject)
public:
    enum Flag {
        ReadOnly = (1<<0),
        /*padding*/
        Last = (1<<4)
    };
    Q_DECLARE_FLAGS(Flags, Flag);

    static QGstMiniObjectPtr fromGstMiniObject(GstMiniObject *gstMiniObject);
    virtual ~QGstMiniObject();

    QGstMiniObjectPtr copy() const;
    bool isWritable() const;
    void makeWritable();

    Flags flags() const;
    bool flagIsSet(Flag flag) const;
    void flagSet(Flag flag);
    void flagUnset(Flag flag);

    /** Returns the internal GstMiniObject pointer, without any extra reference count.
     * This is useful if you want to use a native function on the object.
     * @sa getNativeObject
     */
    inline GstMiniObject *peekNativeObject();

    /** Returns the internal GstMiniObject with an extra reference count on it.
     * This is useful if you want to extract the GstMiniObject and destroy the QGstMiniObject.
     * Equivalent to gst_mini_object_ref(qgstminiobjectptr->peekNativeObject());
     * @sa peekNativeObject
     */
    GstMiniObject *getNativeObject();

protected:
    QGstMiniObject(GstMiniObject *gstMiniObject);
    GstMiniObject *m_object;
};

inline GstMiniObject *QGstMiniObject::peekNativeObject()
{
    return m_object;
}

}

Q_DECLARE_OPERATORS_FOR_FLAGS(QtGstreamer::QGstMiniObject::Flags)
Q_DECLARE_METATYPE(QtGstreamer::QGstMiniObjectPtr)

#endif
