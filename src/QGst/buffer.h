/*
    Copyright (C) 2010  Collabora Multimedia.
      @author Mauricio Piacentini <mauricio.piacentini@collabora.co.uk>

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
#ifndef QGST_BUFFER_H
#define QGST_BUFFER_H

#include "miniobject.h"

namespace QGst {

    /*! \headerfile buffer.h <QGst/Buffer>
     * \brief Wrapper class for GstBuffer
     *
     * Buffers are the basic unit of data transfer in GStreamer. The data() pointer holds the
     * contents of the buffer, and the length is obtained from size(). Buffers also contain a
     * CapsPtr in caps() that indicates the format of the buffer data.
     *
     */
class QTGSTREAMER_EXPORT Buffer : public MiniObject
{
    QGST_WRAPPER(Buffer)
public:
    static BufferPtr create(uint size = 0);

    quint8 * data() const;
    quint32 size() const;

    ClockTime timeStamp() const;
    ClockTime duration() const;

    CapsPtr caps() const;
    void setCaps(const CapsPtr & caps);

    quint64 offset() const;
    quint64 offsetEnd() const;

    BufferFlags flags() const;
    void setFlags(const BufferFlags flags);

    BufferPtr copy() const;

};

} //namespace QGst

QGST_REGISTER_TYPE(QGst::Buffer)

#endif
