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
#ifndef QGST_UTILS_APPLICATIONSINK_H
#define QGST_UTILS_APPLICATIONSINK_H

#include "global.h"
#include "../element.h"
#include "../bufferlist.h"

namespace QGst {
namespace Utils {

/*! \headerfile applicationsink.h <QGst/Utils/ApplicationSink>
 * \brief Helper class for using a GstAppSink
 *
 * Appsink is a sink plugin that supports many different methods for making the application
 * get a handle on the GStreamer data in a pipeline. Unlike most GStreamer elements, appsink
 * provides external API functions. This class exports those API functions in the bindings
 * and makes it easy to implement a custom sink.
 *
 * There are two ways to use this class. The first method is to construct an instance and use
 * the pull* methods to pull buffers into your code. Those functions will block until a buffer
 * is available. The second method is to subclass and re-implement one of the virtual protected
 * functions, to get notifications of events that happen in the pipeline. Note that these
 * functions will be called from the streaming thread.
 *
 * The actuall appsink element can be retrieved with element() and set with setElement(). It is
 * not necessary to set an appsink, as this class will create one as soon as it is needed.
 *
 * \note It is not necessary to use this class in order to use GstAppSink. GstAppSink
 * also provides signals and properties that fully substitute the functionality of this class.
 */
class QTGSTREAMERUTILS_EXPORT ApplicationSink
{
public:
    ApplicationSink();
    virtual ~ApplicationSink();

    /*! \returns the appsink element */
    ElementPtr element() const;

    /*! Sets the appsink element that will be used by this class */
    void setElement(const ElementPtr & appsink);


    /*! \returns the caps that the appsink is configured to accept */
    CapsPtr caps() const;

    /*! Sets the caps that appsink will accept */
    void setCaps(const CapsPtr & caps);


    /*! \returns true if the appsink has received an End Of Stream event
     * or if it is not in PAUSED or PLAYING state. */
    bool isEos() const;


    /*! \returns the maximum number of buffers that the sink is
     * configured to hold in its internal queue. 0 means unlimited. */
    uint maxBuffers() const;

    /*! Sets the maximum number of buffers that the sink will hold in its queue.
     * After this amount of buffers are queued in appsink, any more buffers will
     * block upstream elements until a buffer is pulled from appsink. */
    void setMaxBuffers(uint maxbuffers);


    /*! \returns whether the sink is configured to drop old buffers
     * when the maximum amount of queued buffers is reached. */
    bool dropEnabled() const;

    /*! Enables dropping old buffers when the maximum amount of queued buffers is reached. */
    void enableDrop(bool enable);

    /*! Get the last preroll buffer in appsink. This was the buffer that caused the appsink
     * to preroll in the PAUSED state. This buffer can be pulled many times and remains
     * available to the application even after EOS.
     *
     * This function is typically used when dealing with a pipeline in the PAUSED state.
     * Calling this function after doing a seek will give the buffer right after the seek position.
     *
     * Note that the preroll buffer will also be returned as the first buffer when calling
     * pullBuffer().
     *
     * If an EOS event was received before any buffers, this function returns a null BufferPtr.
     * Use isEos() to check for the EOS condition.
     *
     * This function blocks until a preroll buffer or EOS is received or the appsink element
     * is set to the READY/NULL state.
     */
    BufferPtr pullPreroll();

    /*! This function blocks until a buffer or EOS becomes available or the appsink
     * element is set to the READY/NULL state.
     *
     * This function will only return buffers when the appsink is in the PLAYING state.
     * All rendered buffers will be put in a queue so that the application can pull buffers
     * at its own rate. Note that when the application does not pull buffers fast enough, the
     * queued buffers could consume a lot of memory, especially when dealing with raw video frames.
     *
     * If an EOS event was received before any buffers, this function returns a null BufferPtr.
     * Use isEos() to check for the EOS condition.
     */
    BufferPtr pullBuffer();

    /*! This function blocks until a buffer list or EOS becomes available or the appsink
     * element is set to the READY/NULL state.
     *
     * This function will only return buffer lists when the appsink is in the PLAYING state.
     * All rendered buffer lists will be put in a queue so that the application can pull buffer
     * lists at its own rate. Note that when the application does not pull buffer lists fast
     * enough, the queued buffer lists could consume a lot of memory, especially when dealing
     * with raw video frames.
     *
     * If an EOS event was received before any buffer lists, this function returns a null
     * BufferListPtr. Use isEos() to check for the EOS condition.
     */
    BufferListPtr pullBufferList();

protected:
    /*! Called when the end-of-stream has been reached.
     * \note This function is called from the steaming thread. */
    virtual void eos();

    /*! Called when a new preroll buffer is available. The new preroll buffer can be
     * retrieved with pullPreroll() either from this function or from any other thread.
     * \note This function is called from the steaming thread. */
    virtual FlowReturn newPreroll();

    /*! Called when a new buffer is available. The new buffer can be retrieved
     * with pullBuffer() either from this function or from any other thread.
     * \note This function is called from the steaming thread. */
    virtual FlowReturn newBuffer();

    /*! Called when a new buffer list is available. The new buffer list can be retrieved
     * with pullBufferList() either from this function or from any other thread.
     * \note This function is called from the steaming thread. */
    virtual FlowReturn newBufferList();

private:
    /* vtable padding */
    virtual void reservedVirtual1() {}
    virtual void reservedVirtual2() {}
    virtual void reservedVirtual3() {}

private:
    struct Priv;
    friend struct Priv;
    Priv *const d;
    Q_DISABLE_COPY(ApplicationSink)
};

} //namespace Utils
} //namespace QGst

#endif // QGST_APPLICATIONSINK_H
