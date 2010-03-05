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
#if !(defined(QGLIB_VALUEIMPL_H) && defined(QGLIB_VALUE_H))
# error "This header must not be included directly"
#endif

#include "assert.h"
#include "../type.h"
#include <boost/static_assert.hpp>
#include <boost/type_traits.hpp>

namespace QGlib {

/** This class provides the implementation for the Value::get() / Value::set() methods.
 * If you want to provide support for a custom type, you should provide a template
 * specialization of this class that sets and gets data to the underlying GValue
 * for your custom type.
 * The default implementation expects T to be an enum.
 */
template <typename T>
class ValueImpl
{
    BOOST_STATIC_ASSERT(boost::is_enum<T>::value);
public:
    static T get(const ValueBase & value);
    static void set(ValueBase & value, const T & data);
};

//default implementation (handles enums)

template <typename T>
T ValueImpl<T>::get(const ValueBase & value)
{
    VALUE_ASSERT(value.type().fundamental() == Type::Enum);
    return static_cast<T>(value.getEnum());
}

template <typename T>
void ValueImpl<T>::set(ValueBase & value, const T & data)
{
    VALUE_ASSERT(value.type().fundamental() == Type::Enum);
    value.setEnum(static_cast<int>(data));
}

}
