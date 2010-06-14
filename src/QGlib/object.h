/*
    Copyright (C) 2010  George Kiagiadakis <kiagiadakis.george@gmail.com>

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
#ifndef QGLIB_OBJECT_H
#define QGLIB_OBJECT_H

#include "paramspec.h"
#include "value.h"

namespace QGlib {

class Object : public RefCountedObject
{
    QGLIB_WRAPPER(Object)
public:
    ParamSpecPtr findProperty(const String & name) const;
    QList<ParamSpecPtr> listProperties() const;

    Value property(const String & name) const;
    template <class T> void setProperty(const String & name, const T & value);
    void setPropertyValue(const String & name, const ValueBase & value);

protected:
    virtual void ref();
    virtual void unref();
};

template <class T>
void Object::setProperty(const String & name, const T & value)
{
    ParamSpecPtr param = findProperty(name);
    if (param) {
        Value v;
        v.init(param->valueType());
        v.set<T>(value);
        setPropertyValue(name, v);
    }
}

}

QGLIB_REGISTER_TYPE(QGlib::Object)

//inject ValueImpl for Object refpointers
#include "valueimpl/objectptr.h"

#endif
