/*
    Copyright (C) 2009-2010  George Kiagiadakis <kiagiadakis.george@gmail.com>

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
#include "structure.h"
#include <gst/gststructure.h>
#include <QtCore/QDebug>

namespace QGst {

//BEGIN StructureBase

StructureBase::StructureBase(GstStructure *structure)
    : m_structure(structure)
{
}

StructureBase::~StructureBase()
{
}

bool StructureBase::isValid() const
{
    return m_structure != NULL;
}

QGlib::String StructureBase::name() const
{
    Q_ASSERT(isValid());
    return QGlib::String(gst_structure_get_name(m_structure));
}

void StructureBase::setName(const QGlib::String & name)
{
    Q_ASSERT(isValid());
    gst_structure_set_name(m_structure, name);
}

QGlib::Value StructureBase::value(const QGlib::String & fieldName) const
{
    Q_ASSERT(isValid());
    return QGlib::Value(*gst_structure_get_value(m_structure, fieldName));
}

void StructureBase::setValue(const QGlib::String & fieldName, const QGlib::Value & value)
{
    Q_ASSERT(isValid());
    gst_structure_set_value(m_structure, fieldName, value.peekGValue());
}

unsigned int StructureBase::numberOfFields() const
{
    Q_ASSERT(isValid());
    return gst_structure_n_fields(m_structure);
}

QGlib::String StructureBase::fieldName(unsigned int fieldNumber)
{
    Q_ASSERT(isValid());
    return QGlib::String(gst_structure_nth_field_name(m_structure, fieldNumber));
}

QGlib::Type StructureBase::fieldType(const QGlib::String& fieldName) const
{
    Q_ASSERT(isValid());
    return gst_structure_get_field_type(m_structure, fieldName);
}

bool StructureBase::hasField(const QGlib::String& fieldName) const
{
    Q_ASSERT(isValid());
    return gst_structure_has_field(m_structure, fieldName);
}

bool StructureBase::hasFieldTyped(const QGlib::String& fieldName, QGlib::Type type) const
{
    Q_ASSERT(isValid());
    return gst_structure_has_field_typed(m_structure, fieldName, type);
}

void StructureBase::removeField(const QGlib::String& fieldName)
{
    Q_ASSERT(isValid());
    return gst_structure_remove_field(m_structure, fieldName);
}

void StructureBase::removeAllFields()
{
    Q_ASSERT(isValid());
    return gst_structure_remove_all_fields(m_structure);
}

QGlib::String StructureBase::toString() const
{
    Q_ASSERT(isValid());
    return QGlib::String::fromGCharPtr(gst_structure_to_string(m_structure));
}

//END StructureBase

//BEGIN Structure

Structure::Structure(const QGlib::String & name)
    : StructureBase(gst_structure_empty_new(name))
{
}

Structure::Structure(const SharedStructure & other)
    : StructureBase(other.peekGstStructure() ? gst_structure_copy(other.peekGstStructure()) : NULL)
{
}

Structure::Structure(const Structure & other)
    : StructureBase(other.peekGstStructure() ? gst_structure_copy(other.peekGstStructure()) : NULL)
{
}

Structure::~Structure()
{
    if (m_structure) {
        gst_structure_free(m_structure);
    }
}

Structure & Structure::operator=(const SharedStructure & other)
{
    if (m_structure) {
        gst_structure_free(m_structure);
    }
    m_structure = other.peekGstStructure() ? gst_structure_copy(other.peekGstStructure()) : NULL;
    return *this;
}

Structure & Structure::operator=(const Structure & other)
{
    if (m_structure) {
        gst_structure_free(m_structure);
    }
    m_structure = other.peekGstStructure() ? gst_structure_copy(other.peekGstStructure()) : NULL;
    return *this;
}

Structure Structure::fromString(const QGlib::String& str)
{
    Structure s(SharedStructure(NULL));
    s.m_structure = gst_structure_from_string(str, NULL);
    return s;
}

//END Structure

//BEGIN SharedStructure

SharedStructure::SharedStructure(GstStructure *structure)
    : StructureBase(structure)
{
}

SharedStructure::SharedStructure(const SharedStructure & other)
    : StructureBase(other.m_structure)
{
}

SharedStructure::~SharedStructure()
{
}

SharedStructure& SharedStructure::operator=(const SharedStructure & other)
{
    m_structure = other.m_structure;
    return *this;
}

//END SharedStructure

} //namespace QGst

QDebug operator<<(QDebug debug, const QGst::StructureBase & structure)
{
    debug.nospace() << "QGst::StructureBase";
    if (structure.isValid()) {
        debug.nospace() << "(" << structure.toString() << ")";
    } else {
        debug.nospace() << "(<invalid>)";
    }
    return debug.space();
}
