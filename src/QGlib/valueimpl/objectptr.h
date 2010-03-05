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
#ifndef QGLIB_VALUEIMPL_OBJECTPTR_H
#define QGLIB_VALUEIMPL_OBJECTPTR_H

#include "assert.h"
#include "../object.h"

namespace QGlib {

//specialization for object types

template <typename T>
class ValueImpl< RefPointer<T> >
{
    BOOST_STATIC_ASSERT((boost::is_base_of<QGlib::Object, T>::value));
public:
    static RefPointer<T> get(const ValueBase & value);
    static void set(ValueBase & value, const RefPointer<T> & data);
};

template <typename T>
RefPointer<T> ValueImpl< RefPointer<T> >::get(const ValueBase & value)
{
    VALUE_ASSERT(value.type().fundamental() == Type::Object);
    ObjectPtr object = ObjectPtr::wrap(static_cast<GObject*>(value.getObject()));
    return object.dynamicCast<T>();
}

template <typename T>
void ValueImpl< RefPointer<T> >::set(ValueBase & value, const RefPointer<T> & data)
{
    VALUE_ASSERT(value.type().fundamental() == Type::Object);
    value.setObject(static_cast<typename T::CType*>(data));
}

}

#endif
