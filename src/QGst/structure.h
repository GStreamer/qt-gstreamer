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
#ifndef QGST_STRUCTURE_H
#define QGST_STRUCTURE_H

#include "global.h"
#include "../QGlib/type.h"
#include "../QGlib/string.h"
#include "../QGlib/value.h"

namespace QGst {

class StructureBase
{
public:
    bool isValid() const;

    QGlib::String name() const;
    void setName(const QGlib::String & name);

    QGlib::Value value(const QGlib::String & fieldName) const;
    void setValue(const QGlib::String & fieldName, const QGlib::Value & value);

    unsigned int numberOfFields() const;
    QGlib::String fieldName(unsigned int fieldNumber);
    QGlib::Type fieldType(const QGlib::String & fieldName) const;
    bool hasField(const QGlib::String & fieldName) const;
    bool hasFieldTyped(const QGlib::String & fieldName, QGlib::Type type) const; //FIXME better name?

    void removeField(const QGlib::String & fieldName);
    void removeAllFields();

    QGlib::String toString() const; //FIXME maybe call it serialize()?

    inline operator GstStructure*() { return m_structure; }
    inline operator const GstStructure*() const { return m_structure; }

    //TODO iterators, gst_structure_fixate_*, quark methods

protected:
    StructureBase(GstStructure *structure);
    virtual ~StructureBase();
    Q_DISABLE_COPY(StructureBase);

    GstStructure *m_structure;
};

class SharedStructure;

class Structure : public StructureBase
{
public:
    Structure(const QGlib::String & name);
    Structure(const SharedStructure & other);
    Structure(const Structure & other);
    virtual ~Structure();

    Structure & operator=(const SharedStructure & other);
    Structure & operator=(const Structure & other);

    static Structure fromString(const QGlib::String & str);
};

class SharedStructure : public StructureBase
{
public:
    SharedStructure(GstStructure *structure);
    SharedStructure(const SharedStructure & other);
    virtual ~SharedStructure();
    SharedStructure & operator=(const SharedStructure & other);
};

} //namespace QGst

QDebug operator<<(QDebug debug, const QGst::StructureBase & structure);

QGLIB_REGISTER_TYPE(QGst::StructureBase) //codegen: GType=GST_TYPE_STRUCTURE
QGLIB_REGISTER_TYPE(QGst::Structure)
QGLIB_REGISTER_TYPE(QGst::SharedStructure) //codegen: GType=GST_TYPE_STRUCTURE

#endif
