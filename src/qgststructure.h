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
#ifndef QTGSTREAMER_QGSTSTRUCTURE_H
#define QTGSTREAMER_QGSTSTRUCTURE_H

#include "qgstdeclarations.h"

namespace QtGstreamer {

class QGstStructure
{
    friend class QGstCaps;
public:
    QGstStructure(const char *name);
    QGstStructure(const QGstStructure & other);
    virtual ~QGstStructure();

    QGstStructure & operator=(const QGstStructure & other);

    QGstValue getValue(const char *fieldName) const;
    void setValue(const char *fieldName, const QGstValue & value);

    QByteArray getName() const;
    bool hasName(const char *name) const;
    void setName(const char *name);

private:
    GstStructure *m_structure;
};

}

#endif
