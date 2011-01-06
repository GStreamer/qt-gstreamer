/*
    Copyright (C) 2010 George Kiagiadakis <kiagiadakis.george@gmail.com>
    Copyright (C) 2010 Collabora Ltd.
      @author George Kiagiadakis <george.kiagiadakis@collabora.co.uk>

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
 * \brief Common virtual base class for Object and Interface
 *
 * This class is an implementation detail that serves only in code reuse between
 * the Object and Interface classes. You should not use this class directly at all.
 * Use Object or Interface instead.
 */
class ObjectBase : public RefCountedObject
{
public:
    ParamSpecPtr findProperty(const char *name) const;
    QList<ParamSpecPtr> listProperties() const;

    Value property(const char *name) const;
    template <class T> void setProperty(const char *name, const T & value);
    void setPropertyValue(const char *name, const Value & value);

    void *data(const char *key) const;
    void *stealData(const char *key) const;
    void setData(const char *key, void *data, void (*destroyCallback)(void*) = NULL);

    void *quarkData(const Quark & quark) const;
    void *stealQuarkData(const Quark & quark) const;
    void setQuarkData(const Quark & quark, void *data, void (*destroyCallback)(void*) = NULL);

protected:
    ObjectBase() {}
    virtual ~ObjectBase() {}
    Q_DISABLE_COPY(ObjectBase);

    virtual void ref(bool increaseRef);
    virtual void unref();
};

/*! \headerfile QGlib/object.h <QGlib/Object>
 * \brief Wrapper class for GObject
 *
 * The methods of this class can be found in ObjectBase.
 */
class Object : virtual public ObjectBase
{
    QGLIB_WRAPPER(Object)
};

/*! \headerfile QGlib/object.h <QGlib/Object>
 * \brief Base class for interface wrappers
 *
 * The methods of this class can be found in ObjectBase.
 */
class Interface : virtual public ObjectBase
{
    QGLIB_WRAPPER_DIFFERENT_C_CLASS(Interface, Object)
};


template <class T>
void ObjectBase::setProperty(const char *name, const T & value)
{
    ParamSpecPtr param = findProperty(name);
    if (param) {
        Value v;
        v.init(param->valueType());
        v.set<T>(value);
        setPropertyValue(name, v);
    }
}

} //namespace QGlib

QGLIB_REGISTER_TYPE(QGlib::Object)
QGLIB_REGISTER_TYPE(QGlib::Interface)
QGLIB_REGISTER_WRAPIMPL_FOR_SUBCLASSES_OF(QGlib::Object, QGlib::Private::wrapObject)

#endif
