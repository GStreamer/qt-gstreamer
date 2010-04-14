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
#ifndef QGST_BIN_H
#define QGST_BIN_H

#include "element.h"

namespace QGst {

class Bin : public Element
{
    QGST_WRAPPER(Bin)
public:
    static BinPtr newBin(const QString & name = QString());

    bool add(const ElementPtr & element);
    bool remove(const ElementPtr & element);

    /** This enum is used with getElementByName() to specify where to look for
     * elements if an element is not found as a direct child of this Bin.
     */
    enum RecursionType { //codegen: skip=true
        /** Recurse down into child bins (if any) */
        RecurseDown,
        /** Recurse down to child bins and if the element is not found there as well,
         * recurse up the hierarchy to the parent bins (if any) and their children.
         */
        RecurseUp
    };
    ElementPtr getElementByName(const QString & name, RecursionType = RecurseDown) const;
    ElementPtr getElementByInterface(QGlib::Type interfaceType) const;
    template <typename T> QGlib::RefPointer<T> getElementByInterface() const;

    PadPtr findUnlinkedPad(PadDirection direction) const;
    bool recalculateLatency();
};

template <typename T>
QGlib::RefPointer<T> Bin::getElementByInterface() const
{
    ElementPtr p = getElementByInterface(QGlib::GetType<T>());
    return p.staticCast<T>();
}

}

QGLIB_REGISTER_TYPE(QGst::Bin)

#endif
