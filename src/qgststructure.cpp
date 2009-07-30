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
#include "qgststructure.h"
#include "qgstvalue.h"
#include <gst/gststructure.h>

namespace QtGstreamer {

QGstStructure::QGstStructure(const char *name)
{
    m_structure = gst_structure_empty_new(name);
}

QGstStructure::QGstStructure(const QGstStructure & other)
{
    m_structure = gst_structure_copy(other.m_structure);
}

QGstStructure::~QGstStructure()
{
    gst_structure_free(m_structure);
}

QGstStructure & QGstStructure::operator=(const QGstStructure & other)
{
    gst_structure_free(m_structure);
    m_structure = gst_structure_copy(other.m_structure);
    return *this;
}

QGstValue QGstStructure::getValue(const char *fieldName) const
{
    return QGstValue::fromGValue(gst_structure_get_value(m_structure, fieldName));
}

void QGstStructure::setValue(const char *fieldName, const QGstValue & value)
{
    gst_structure_set_value(m_structure, fieldName, value.toGValue());
}

QByteArray QGstStructure::getName() const
{
    return QByteArray(gst_structure_get_name(m_structure));
}

bool QGstStructure::hasName(const char *name) const
{
    return gst_structure_has_name(m_structure, name);
}

void QGstStructure::setName(const char *name)
{
    gst_structure_set_name(m_structure, name);
}

}
