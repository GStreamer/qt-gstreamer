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
#ifndef QTGSTREAMER_QGSTCAPS_H
#define QTGSTREAMER_QGSTCAPS_H

#include "qgstdeclarations.h"
#include <QtCore/QMetaType>

namespace QtGstreamer {

class QGstCaps
{
    Q_DISABLE_COPY(QGstCaps)
    friend class QGstValue;
public:
    static QGstCapsPtr newEmpty();
    static QGstCapsPtr newAny();
    static QGstCapsPtr fromGstCaps(GstCaps *gstCaps);
    virtual ~QGstCaps();

    void makeWritable();
    void appendStructure(const QGstStructure & structure);
    uint getSize() const;

    static QGstCapsPtr fromString(const char *string);
    QByteArray toString() const;

protected:
    QGstCaps(GstCaps *gstCaps);

private:
    GstCaps *m_caps;
};

}

Q_DECLARE_METATYPE(QtGstreamer::QGstCapsPtr)

#endif
