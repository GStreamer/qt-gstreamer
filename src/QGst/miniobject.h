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
#ifndef QGST_MINIOBJECT_H
#define QGST_MINIOBJECT_H

#include "global.h"
#include "../QGlib/refpointer.h"
#include "../QGlib/type.h"
#include "../QGlib/value.h"

namespace QGst {

/*! \headerfile miniobject.h <QGst/MiniObject>
 * \brief Wrapper class for GstMiniObject
 */
class MiniObject : public QGlib::RefCountedObject
{
    QGST_WRAPPER(MiniObject)
public:
    MiniObjectPtr copy() const;
    bool isWritable() const;
    void makeWritable();

    MiniObjectFlags flags() const;
    bool flagIsSet(MiniObjectFlag flag) const;
    void setFlag(MiniObjectFlag flag);
    void unsetFlag(MiniObjectFlag flag);

protected:
    virtual void ref(bool increaseRef);
    virtual void unref();
};

} //namespace QGst

QGLIB_REGISTER_TYPE(QGst::MiniObject)
QGLIB_REGISTER_VALUEIMPL(QGst::MiniObjectPtr)

#endif
