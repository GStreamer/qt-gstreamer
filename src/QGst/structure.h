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
#include "../QGlib/value.h"
#include <QtCore/QString>

namespace QGst {

class StructureBase
{
public:
    bool isValid() const;

    QString name() const;
    void setName(const char *name);

    QGlib::Value value(const char *fieldName) const;
    void setValue(const char *fieldName, const QGlib::Value & value);

    unsigned int numberOfFields() const;
    QString fieldName(unsigned int fieldNumber) const;
    QGlib::Type fieldType(const char *fieldName) const;
    bool hasField(const char *fieldName) const;
    bool hasFieldTyped(const char *fieldName, QGlib::Type type) const; //FIXME better name?

    void removeField(const char *fieldName);
    void removeAllFields();

    QString toString() const; //FIXME maybe call it serialize()?

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
    explicit Structure(const char *name);
    explicit Structure(const GstStructure *structure);
    Structure(const SharedStructure & other);
    Structure(const Structure & other);
    virtual ~Structure();

    Structure & operator=(const SharedStructure & other);
    Structure & operator=(const Structure & other);

    static Structure fromString(const char *str);

private:
    Structure(); //used in fromString()
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
