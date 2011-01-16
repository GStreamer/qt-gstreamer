/*
    Copyright (C) 2011 Collabora Ltd. <info@collabora.co.uk>
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
#ifndef QGST_BUFFERLIST_H
#define QGST_BUFFERLIST_H

#include "buffer.h"

namespace QGst {

/*! \headerfile bufferlist.h <QGst/BufferList>
 * \brief Wrapper class for GstBufferList
 *
 * Buffer lists are units of grouped scatter/gather data transfer in GStreamer.
 *
 * \sa BufferListIterator
 */
class QTGSTREAMER_EXPORT BufferList : public MiniObject
{
    QGST_WRAPPER(BufferList)
public:
    static BufferListPtr create();

    uint groupsCount() const;
    BufferPtr bufferAt(uint group, uint index) const;

    inline BufferListPtr copy() const;
    inline BufferListPtr makeWritable() const;
};

inline BufferListPtr BufferList::copy() const
{
    return MiniObject::copy().staticCast<BufferList>();
}

inline BufferListPtr BufferList::makeWritable() const
{
    return MiniObject::makeWritable().staticCast<BufferList>();
}


/*! \headerfile bufferlist.h <QGst/BufferList>
 * \brief Wrapper class for GstBufferListIterator
 *
 * This class allows you to iterate through and modify a BufferList.
 *
 * The iterator has no current buffer; its cursor position lies between buffers,
 * immediately before the buffer that would be returned by next(). After iterating
 * to the end of a group the iterator must be advanced to the next group by a call
 * to nextGroup() before any further calls to next() can return buffers again.
 *
 * The cursor position of a newly created iterator lies before the first group;
 * a call to nextGroup() is necessary before calls to next() can return buffers.
 *
 * \sa BufferList
 */
class QTGSTREAMER_EXPORT BufferListIterator
{
public:
    BufferListIterator(const BufferListPtr & list);
    ~BufferListIterator();

    /*! Returns the number of buffers in the current group */
    uint buffersInCurrentGroup() const;

    /*! Advances to the next buffer in the list. If a new group
     * is reached, it will return a null BufferPtr. */
    BufferPtr next();

    /*! Inserts a new buffer in the current group, immediately
     * before the buffer that would be returned by next(). */
    void insert(const BufferPtr & buffer);

    /*! Removes the last buffer returned by next() from the list.
     * \note You must call next() before calling this function
     * and make sure it doesn't return a null BufferPtr. */
    void remove();

    /*! Removes and returns the last buffer returned by next().
     * \note You must call next() before calling this function
     * and make sure it doesn't return a null BufferPtr. */
    BufferPtr take();

    /*! Replaces the last buffer returned by next() with \a other.
     * The buffer that was previously in its place will be unrefed.
     * \note You must call next() before calling this function
     * and make sure it doesn't return a null BufferPtr. */
    void replace(const BufferPtr & other);

    /*! Adds a new empty group in the list, imeediately before the group
     * that would be returned by nextGroup(). The iterator is advanced
     * to point at the beginning of the new group. */
    void addGroup();

    /*! Advances the iterator to the beginning of the next group and
     * returns true if it succeeded or false if there are no more groups. */
    bool nextGroup();

    /*! Merges all the buffers of the current group in a signle buffer and retruns it. */
    BufferPtr mergeGroup() const;

private:
    GstBufferListIterator *m_it;
    Q_DISABLE_COPY(BufferListIterator)
};

} //namespace QGst

QGST_REGISTER_TYPE(QGst::BufferList)

#endif // QGST_BUFFERLIST_H
