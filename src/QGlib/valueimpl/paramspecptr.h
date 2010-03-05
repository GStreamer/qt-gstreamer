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
#ifndef QGLIB_VALUEIMPL_PARAMSPECPTR_H
#define QGLIB_VALUEIMPL_PARAMSPECPTR_H

#include "assert.h"
#include "../paramspec.h"
#include "../value.h"

namespace QGlib {

//specialization for ParamSpecPtr

template <>
class ValueImpl<ParamSpecPtr>
{
public:
    static ParamSpecPtr get(const ValueBase & value) {
        VALUE_ASSERT(value.type().fundamental() == Type::Param);
        return ParamSpecPtr::wrap(value.getParamSpec());
    }

    static void set(ValueBase & value, const ParamSpecPtr & data) {
        VALUE_ASSERT(value.type().fundamental() == Type::Param);
        value.setParamSpec(data);
    }
};

}

#endif
