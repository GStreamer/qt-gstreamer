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
#include "childproxy.h"

namespace QGst {

/*! \headerfile bin.h <QGst/Bin>
 * \brief Wrapper class for GstBin
 */
class Bin : public Element, public ChildProxy
{
    QGST_WRAPPER(Bin)
public:
    /*! Creates a new Bin with the specified \a name */
    static BinPtr create(const char *name = NULL);

    /*! Creates a new Bin from a bin description. The syntax is the same as the one used
     * in the gst-launch tool. Ghost pads on the bin for unlinked source or sink pads within the
     * bin can automatically be created, if \a ghostUnlinkedPads is set to true (but only a maximum
     * of one ghost pad for each direction will be created; if you expect multiple unlinked source
     * pads or multiple unlinked sink pads and want them all ghosted, you will have to create the
     * ghost pads yourself)
     * \throws QGlib::Error when there was a problem creating the pipeline
     */
    static BinPtr fromDescription(const char *description, bool ghostUnlinkedPads = true);

    bool add(const ElementPtr & element);
    bool remove(const ElementPtr & element);

    /*! This enum is used with getElementByName() to specify where to look for
     * elements if an element is not found as a direct child of this Bin.
     */
    enum RecursionType { //codegen: skip=true
        /*! Recurse down into child bins (if any) */
        RecurseDown,
        /*! Recurse down to child bins and if the element is not found there as well,
         * recurse up the hierarchy to the parent bins (if any) and their children.
         */
        RecurseUp
    };
    ElementPtr getElementByName(const char *name, RecursionType = RecurseDown) const;
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
