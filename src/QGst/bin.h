/*
    Copyright (C) 2009-2010  George Kiagiadakis <kiagiadakis.george@gmail.com>
    Copyright (C) 2011 Collabora Ltd.
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
#ifndef QGST_BIN_H
#define QGST_BIN_H

#include "element.h"
#include "childproxy.h"

namespace QGst {

/*! \headerfile bin.h <QGst/Bin>
 * \brief Wrapper class for GstBin
 *
 * Bin is an element that can contain other Elements, allowing them to be managed as a group.
 * Pads from the child elements can be ghosted to the bin, see GhostPad. This makes the bin
 * look like any other elements and enables creation of higher-level abstraction elements.
 *
 * For more information refer to GStreamer's C API documentation.
 */
class QTGSTREAMER_EXPORT Bin : public Element, public ChildProxy
{
    QGST_WRAPPER(Bin)
public:
    /*! Creates a new Bin with the specified \a name */
    static BinPtr create(const char *name = NULL);

    /*! \see fromDescription() */
    enum BinFromDescriptionOption { //codegen: skip=true
        NoGhost = 0, ///< Do not create ghost pads
        Ghost = 1 ///< Create ghost pads
    };

    /*! Creates a new Bin from a bin description. The description's syntax is the same as the one
     * used in the gst-launch tool. If \a ghostUnlinkedPads is set to Ghost, Ghost pads on the bin
     * for unlinked source or sink pads within the bin can automatically be created (but only a
     * maximum of one ghost pad for each direction will be created; if you expect multiple unlinked
     * source pads or multiple unlinked sink pads and want them all ghosted, you will have to
     * create the ghost pads yourself)
     * \throws QGlib::Error when there was a problem creating the pipeline
     */
    static BinPtr fromDescription(const char *description,
                                  BinFromDescriptionOption ghostUnlinkedPads = Ghost);
    /*! \overload */
    static inline BinPtr fromDescription(const QString & description,
                                         BinFromDescriptionOption ghostUnlinkedPads = Ghost);

    /*! Adds the given element to the bin. Sets the element's parent, and thus takes
     * ownership of the element. An element can only be added to one bin.
     *
     * If the element's pads are linked to other pads, the pads will be unlinked before
     * the element is added to the bin.
     */
    bool add(const ElementPtr & element);

    /*! Removes the element from the bin, unparenting it as well.
     *
     * If the element's pads are linked to other pads, the pads will be
     * unlinked before the element is removed from the bin.
     */
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

    /*! Gets the element with the given name from a bin.
     * Returns a null ElementPtr if the element is not found.
     *
     * By default, this function also recurses into child bins. If \a recursionType
     * is set to RecurseUp, it will also search parent bins (if any) and their children.
     */
    ElementPtr getElementByName(const char *name, RecursionType recursionType = RecurseDown) const;

    /*! Returns an element inside the bin that implements the given interface type. */
    ElementPtr getElementByInterface(QGlib::Type interfaceType) const;

    /*! Looks for an element inside the bin that implements the given interface
     * and returns it casted to the interface type. Example:
     * \code
     * QGst::XOverlayPtr xoverlay = bin->getElementByInterface<QGst::XOverlay>();
     * \endcode
     */
    template <typename T> QGlib::RefPointer<T> getElementByInterface() const;

    /*! Recursively looks for elements with an unlinked pad of the given direction within the
     * specified bin and returns an unlinked pad if one is found, or a null PadPtr otherwise.
     */
    PadPtr findUnlinkedPad(PadDirection direction) const;

    bool recalculateLatency();
};

inline BinPtr Bin::fromDescription(const QString & description,
                                   BinFromDescriptionOption ghostUnlinkedPads)
{
    return fromDescription(description.toUtf8().constData(), ghostUnlinkedPads);
}

template <typename T>
QGlib::RefPointer<T> Bin::getElementByInterface() const
{
    ElementPtr p = getElementByInterface(QGlib::GetType<T>());
    return p.dynamicCast<T>();
}

} //namespace QGst

QGST_REGISTER_TYPE(QGst::Bin)

#endif
