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
#ifndef QGLIB_CLOSURE_H
#define QGLIB_CLOSURE_H

#include "global.h"
#include "type.h"
#include "refpointer.h"

namespace QGlib {

/* Wrapper class for GClosure. It contains no public methods, it is
 * only used internally to pass around GClosure pointers in a C++ fashion.
 */
class Closure : public RefCountedObject
{
    QGLIB_WRAPPER(Closure)

    virtual void ref();
    virtual void unref();
};

} //namespace QGlib

QGLIB_REGISTER_TYPE(QGlib::Closure)

#define IN_QGLIB_CLOSURE_H
# include "closureimpl_p.h"
#undef IN_QGLIB_CLOSURE_H

#endif
