/*
    Copyright (C) 2009-2010  George Kiagiadakis <kiagiadakis.george@gmail.com>
    Copyright (C) 2010  Collabora Multimedia.
      @author Mauricio Piacentini <mauricio.piacentini@collabora.co.uk>

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
#include "../QGlib/string_p.h"
#include <gst/gststructure.h>
#include <QtCore/QDebug>

namespace QGst {

Structure::Structure(GstStructure *structure)
    : m_structure(structure)
{
}

bool Structure::isValid() const
{
    return m_structure != NULL;
}

QString Structure::name() const
{
    Q_ASSERT(isValid());
    return QString::fromUtf8(gst_structure_get_name(m_structure));
}

void Structure::setName(const char *name)
{
    if (!isValid()) {
        //lazy construction
        m_structure = gst_structure_empty_new(name);
    } else {
        gst_structure_set_name(m_structure, name);
    }
}

QGlib::Value Structure::value(const char *fieldName) const
{
    Q_ASSERT(isValid());
    return QGlib::Value(gst_structure_get_value(m_structure, fieldName));
}

void Structure::setValue(const char *fieldName, const QGlib::Value & value)
{
    Q_ASSERT(isValid());
    gst_structure_set_value(m_structure, fieldName, value);
}

unsigned int Structure::numberOfFields() const
{
    Q_ASSERT(isValid());
    return gst_structure_n_fields(m_structure);
}

QString Structure::fieldName(unsigned int fieldNumber) const
{
    Q_ASSERT(isValid());
    return QString::fromUtf8(gst_structure_nth_field_name(m_structure, fieldNumber));
}

QGlib::Type Structure::fieldType(const char *fieldName) const
{
    Q_ASSERT(isValid());
    return gst_structure_get_field_type(m_structure, fieldName);
}

bool Structure::hasField(const char *fieldName) const
{
    Q_ASSERT(isValid());
    return gst_structure_has_field(m_structure, fieldName);
}

bool Structure::hasFieldTyped(const char *fieldName, QGlib::Type type) const
{
    Q_ASSERT(isValid());
    return gst_structure_has_field_typed(m_structure, fieldName, type);
}

void Structure::removeField(const char *fieldName)
{
    Q_ASSERT(isValid());
    return gst_structure_remove_field(m_structure, fieldName);
}

void Structure::removeAllFields()
{
    Q_ASSERT(isValid());
    return gst_structure_remove_all_fields(m_structure);
}

QString Structure::toString() const
{
    Q_ASSERT(isValid());
    return QGlib::Private::stringFromGCharPtr(gst_structure_to_string(m_structure));
}

Structure::Structure(const char *name)
    : m_structure(gst_structure_empty_new(name))
{
}

Structure::Structure(const GstStructure* structure)
    : m_structure(gst_structure_copy(structure))
{
}

Structure::Structure(const Structure & other)
    : m_structure(gst_structure_copy(other))
{
}

Structure::~Structure()
{
    if (m_structure) {
        gst_structure_free(m_structure);
    }
}

Structure & Structure::operator=(const Structure & other)
{
    if (m_structure) {
        gst_structure_free(m_structure);
    }
    m_structure = gst_structure_copy(other);
    return *this;
}

Structure Structure::fromString(const char *str)
{
    //we don't use the Structure(const GstStructure*) constructor to avoid copying
    Structure s;
    s.m_structure = gst_structure_from_string(str, NULL);
    return s;
}

Structure::Structure()
    :m_structure(NULL)
{
}

//END Structure

//BEGIN SharedStructure

SharedStructure::SharedStructure(GstStructure *structure)
{
    m_structure = structure;
}

SharedStructure::~SharedStructure()
{
    m_structure = NULL;
}

//END SharedStructure

QDebug operator<<(QDebug debug, const Structure & structure)
{
    debug.nospace() << "QGst::StructureBase";
    if (structure.isValid()) {
        debug.nospace() << "(" << structure.toString() << ")";
    } else {
        debug.nospace() << "(<invalid>)";
    }
    return debug.space();
}

} //namespace QGst
