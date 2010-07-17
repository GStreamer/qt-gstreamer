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

#include "global.h"
#include "refpointer.h"
#include "paramspec.h"
#include "value.h"
#include "type.h"
#include <QtCore/QList>

namespace QGlib {

/*! \headerfile QGlib/object.h <QGlib/Object>
 * \brief Wrapper class for GObject
 */
class Object : public RefCountedObject
{
    QGLIB_WRAPPER(Object)
public:
    ParamSpecPtr findProperty(const char *name) const;
    QList<ParamSpecPtr> listProperties() const;

    Value property(const char *name) const;
    template <class T> void setProperty(const char *name, const T & value);
    void setPropertyValue(const char *name, const ValueBase & value);

protected:
    virtual void ref();
    virtual void unref();
};

template <class T>
void Object::setProperty(const char *name, const T & value)
{
    ParamSpecPtr param = findProperty(name);
    if (param) {
        Value v;
        v.init(param->valueType());
        v.set<T>(value);
        setPropertyValue(name, v);
    }
}

// -- ValueImpl specialization for GObject-derived classes --

struct ValueImpl_Object
{
    static ObjectPtr get(const ValueBase & value);
    static void set(ValueBase & value, const ObjectPtr & data);
};

template <typename T>
struct ValueImpl< RefPointer<T> >
{
    QGLIB_STATIC_ASSERT((boost::is_base_of<Object, T>::value),
                        "No QGlib::ValueImpl<T> specialization has been registered for this type");

    static inline RefPointer<T> get(const ValueBase & value)
    {
        return ValueImpl_Object::get(value).dynamicCast<T>();
    }

    static inline void set(ValueBase & value, const RefPointer<T> & data)
    {
        ValueImpl_Object::set(value, data);
    }
};

} //namespace QGlib

QGLIB_REGISTER_TYPE(QGlib::Object)

#endif
