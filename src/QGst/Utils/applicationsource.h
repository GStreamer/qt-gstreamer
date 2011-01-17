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
#ifndef QGST_UTILS_APPLICATIONSOURCE_H
#define QGST_UTILS_APPLICATIONSOURCE_H

#include "global.h"
#include "../element.h"
#include "../buffer.h"

namespace QGst {
namespace Utils {

/*! \headerfile applicationsource.h <QGst/Utils/ApplicationSource>
 * \brief Helper class for using a GstAppSrc
 *
 */
class QTGSTREAMERUTILS_EXPORT ApplicationSource
{
public:
    ApplicationSource();
    virtual ~ApplicationSource();

    /*! \returns the appsrc element */
    ElementPtr element() const;

    /*! Sets the appsrc element that will be used by this class */
    void setElement(const ElementPtr & appsrc);


    /*! \returns the caps that the appsrc is configured to produce */
    CapsPtr caps() const;

    /*! Sets the capabilities of this source. After calling this method,
     * this appsrc will only be able to produce buffers with these capabilities.
     * \a caps must be fixed and the caps on the buffers must match the \a caps or left NULL.
     */
    void setCaps(const CapsPtr & caps);


    /*! \returns the minimum latency of this element */
    quint64 minLatency() const;

    /*! \returns the maximum latency of this element */
    quint64 maxLatency() const;

    /*! Configure the minimum and maximum latency. If \a min is set to -1,
     * the default latency calculations for pseudo-live sources will be used. */
    void setLatency(quint64 min, quint64 max);


    /*! \returns the size of the stream in bytes.
     * A value of -1 means that the size is not known. */
    qint64 size() const;

    /*! Set the size of the stream in bytes.
     * A value of -1 means that the size is not known. */
    void setSize(qint64 size);


    /*! \returns the stream type of this appsrc */
    AppStreamType streamType() const;

    /*! Set the stream type on appsrc. For seekable streams,
     * the seekData() method must be reimplemented. */
    void setStreamType(AppStreamType type);


    /*! \returns the maximum amount of bytes that can be queued in appsrc */
    quint64 maxBytes() const;

    /*! Set the maximum amount of bytes that can be queued in appsrc. After
     * the maximum amount of bytes are queued, the enoughData() method of
     * this class will be called. */
    void setMaxBytes(quint64 max);


    /*! \returns true if appsrc is configured to block any calls
     * to pushBuffer() when the maximum amount of bytes are queued. */
    bool blockEnabled() const;

    /*! Enables or disables blocking calls to pushBuffer()
     * when the maximum amount of bytes are queued. */
    void enableBlock(bool enable);


    /*! \returns true if the appsrc is configured to act as a live source */
    bool isLive() const;

    /*! Sets whether appsrc will act as a live source */
    void setLive(bool islive);


    /*! \returns the minimum percent of maxBytes() that
     * when reached, needData() will be called */
    uint minPercent() const;

    /*! Sets the minimum percent of maxBytes() that
     * when reached, needData() will be called */
    void setMinPercent(uint min);


    /*! Adds a buffer to the queue of buffers that the appsrc element will
     * push to its source pad. When the block property is TRUE, this function
     * can block until free space becomes available in the queue.
     *
     * \returns FlowOk when the buffer was successfuly queued, FlowWrongState when
     * appsrc is not PAUSED or PLAYING or FlowUnexpected when EOS occured.
     */
    FlowReturn pushBuffer(const BufferPtr & buffer);

    /*! Indicates to the appsrc element that the last buffer queued
     * in the element is the last buffer of the stream.
     *
     * \returns FlowOk when the EOS was successfuly queued or
     * FlowWrongState when appsrc is not PAUSED or PLAYING.
     */
    FlowReturn endOfStream();

protected:
    /*! Called when the appsrc needs more data. A buffer or EOS should be pushed
     * to appsrc from this thread or another thread. length is just a hint and when
     * it is set to -1, any number of bytes can be pushed into appsrc. */
    virtual void needData(uint length);

    /*! Called when appsrc has enough data. It is recommended that the application
     * stops calling pushBuffer() until the needData() method is called again to
     * avoid excessive buffer queueing. */
    virtual void enoughData();

    /*! Called when a seek should be performed to the offset. The next pushBuffer()
     * should produce buffers from the new offset. This method is only called for
     * seekable stream types. */
    virtual bool seekData(quint64 offset);

private:
    /* vtable padding */
    virtual void reservedVirtual1() {}
    virtual void reservedVirtual2() {}
    virtual void reservedVirtual3() {}
    virtual void reservedVirtual4() {}

private:
    struct Priv;
    friend struct Priv;
    Priv *const d;
    Q_DISABLE_COPY(ApplicationSource)
};

} //namespace Utils
} //namespace QGst

#endif // QGST_UTILS_APPLICATIONSOURCE_H
