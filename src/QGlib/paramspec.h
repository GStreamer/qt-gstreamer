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
#ifndef QGLIB_PARAMSPEC_H
#define QGLIB_PARAMSPEC_H

#include "global.h"

namespace QGlib {

class ParamSpec : public RefCountedObject
{
    QGLIB_WRAPPER(ParamSpec)
public:
    enum ParamFlag { //codegen: prefix=G_PARAM_, ReadWrite=READWRITE
        Readable = 1<<0,
        Writable = 1<<1,
        ReadWrite = Readable | Writable,
        Construct = 1<<2,
        ConstructOnly = 1<<3,
        LaxValidation = 1<<4
    };
    Q_DECLARE_FLAGS(ParamFlags, ParamFlag);

    String name() const;
    String nick() const;
    String description() const;

    ParamFlags flags() const;
    Type valueType() const;
    Type ownerType() const;

    void *quarkData(const Quark & quark) const;
    void *stealQuarkData(const Quark & quark) const;
    void setQuarkData(const Quark & quark, void *data, void (*destroyCallback)(void*) = NULL);

    ParamSpecPtr redirectTarget() const;

protected:
    virtual void ref();
    virtual void unref();
};

}

QGLIB_REGISTER_TYPE(QGlib::ParamSpec) //codegen: GType=G_TYPE_PARAM
QGLIB_REGISTER_TYPE(QGlib::ParamSpec::ParamFlags)
Q_DECLARE_OPERATORS_FOR_FLAGS(QGlib::ParamSpec::ParamFlags)

//inject ValueImpl for ParamSpecPtr
#include "valueimpl/paramspecptr.h"

#endif
