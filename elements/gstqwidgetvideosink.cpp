/*
    Copyright (C) 2010 George Kiagiadakis <kiagiadakis.george@gmail.com>

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

/**
 * SECTION:element-qwidgetvideosink
 *
 * qwidgetvideosink is a video sink element that can draw directly on a QWidget
 * using the QPainter API. To use it, you only have to set a QWidget pointer as
 * the "widget" property and the video will then be rendered on that QWidget.
 *
 * This is useful for cases where you cannot or you do not want to use one of the
 * sinks that implement the GstXOverlay interface, for example for rendering video
 * inside a QGraphicsView or for rendering video on the QWS (Qt/Embedded) platform.
 * This sink is guaranteed to work on all platforms supported by Qt, however it
 * is not recommended to use it if you have another choice. For example, on X11 it
 * is recommended to use "xvimagesink" instead, which uses hardware acceleration.
 *
 * There are certain rules for using qwidgetvideosink with threads. It must be
 * created in the main thread, it must be destructed in the main thread and the
 * "widget" property may only be read or written from the main thread as well.
 * This is a (reasonable) limitation implied by Qt.
 */

#include <gst/gst.h>
#include <gst/video/gstvideosink.h>
#include <gst/video/video.h>
#include <QtCore/QCoreApplication>
#include <QtCore/QWeakPointer>
#include <QtCore/QEvent>
#include <QtCore/QMutex>
#include <QtGui/QWidget>
#include <QtGui/QResizeEvent>
#include <QtGui/QPainter>

//BEGIN ******** declarations ********

enum {
    BufferEventType = QEvent::User,
    DeactivateEventType
};

class BufferEvent : public QEvent
{
public:
    inline BufferEvent(GstBuffer *buf)
        : QEvent(static_cast<QEvent::Type>(BufferEventType)),
          buffer(gst_buffer_ref(buf))
    {
    }

    GstBuffer *buffer;
};

class DeactivateEvent : public QEvent
{
public:
    inline DeactivateEvent()
        : QEvent(static_cast<QEvent::Type>(DeactivateEventType))
    {
    }
};

class WidgetProxy : public QObject
{
    Q_OBJECT
public:
    WidgetProxy(GObject *sink);
    virtual ~WidgetProxy();

    // "widget" property
    QWidget *widget() const;
    void setWidget(QWidget *widget);

    // "force-aspect-ratio" property
    bool forceAspectRatio() const;
    void setForceAspectRatio(bool force);

    bool isActive() const;
    void setActive(bool active);

    QSize widgetSize() const;
    void setWidgetSize(const QSize & size);

private Q_SLOTS:
    void widgetDestroyed();

protected:
    virtual bool event(QEvent *event);
    virtual bool eventFilter(QObject *filteredObject, QEvent *event);

private:
#ifndef GST_DISABLE_GST_DEBUG
    //used for calling the Gst debug macros
    GObject *m_sink;
#endif

    // "widget" property
    QWeakPointer<QWidget> m_widget;

    // original value of the Qt::WA_OpaquePaintEvent attribute
    bool m_opaquePaintEventAttribute : 1;

    // "force-aspect-ratio" property
    bool m_forceAspectRatio : 1;

    // whether the sink is active (PAUSED or PLAYING)
    bool m_isActive : 1;
    mutable QMutex m_isActiveMutex;

    // the current widget size, used for caps negotiation
    mutable QMutex m_widgetSizeMutex;
    QSize m_widgetSize;

    // the buffer to be drawn next
    GstBuffer *m_buffer;
};


GST_DEBUG_CATEGORY_STATIC(gst_qwidget_video_sink_debug);
#define GST_CAT_DEFAULT gst_qwidget_video_sink_debug

static const char *qwidgetvideosink_description = "A video sink that draws on a QWidget using QPainter";

#define GST_TYPE_QWIDGETVIDEOSINK \
  (gst_qwidget_video_sink_get_type())
#define GST_QWIDGETVIDEOSINK(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_QWIDGETVIDEOSINK,GstQWidgetVideoSink))
#define GST_QWIDGETVIDEOSINK_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_QWIDGETVIDEOSINK,GstQWidgetVideoSinkClass))

typedef struct _GstQWidgetVideoSink      GstQWidgetVideoSink;
typedef struct _GstQWidgetVideoSinkClass GstQWidgetVideoSinkClass;

struct _GstQWidgetVideoSink
{
    GstVideoSink element;
    WidgetProxy *proxy;
};

struct _GstQWidgetVideoSinkClass
{
    GstVideoSinkClass parent_class;
};

enum {
    PROP_0,
    PROP_WIDGET,
    PROP_FORCE_ASPECT_RATIO
};

static void
gst_qwidget_video_sink_finalize(GObject *object);

static void
gst_qwidget_video_sink_set_property(GObject *object, guint prop_id,
                                    const GValue *value, GParamSpec *pspec);

static void
gst_qwidget_video_sink_get_property(GObject *object, guint prop_id,
                                    GValue *value, GParamSpec *pspec);

static GstStateChangeReturn
gst_qwidget_video_sink_change_state(GstElement *element, GstStateChange transition);

static GstFlowReturn
gst_qwidget_video_sink_buffer_alloc(GstBaseSink *sink, guint64 offset,
                                    guint size, GstCaps *caps, GstBuffer **buf);

static GstFlowReturn
gst_qwidget_video_sink_show_frame(GstVideoSink *video_sink, GstBuffer *buf);

//END ******** declarations ********
//BEGIN ******** WidgetProxy ********

WidgetProxy::WidgetProxy(GObject *sink)
    : QObject(),
#ifndef GST_DISABLE_GST_DEBUG
    m_sink(sink),
#endif
    m_forceAspectRatio(false),
    m_isActive(false),
    m_buffer(NULL)
{
}

WidgetProxy::~WidgetProxy()
{
    if (m_buffer) {
        gst_buffer_unref(m_buffer);
    }
    setWidget(NULL);
}

QWidget *WidgetProxy::widget() const
{
    return m_widget.data();
}

void WidgetProxy::setWidget(QWidget *widget)
{
    GST_DEBUG_OBJECT(m_sink, "Setting \"widget\" property to %"GST_PTR_FORMAT, widget);

    if (m_widget) {
        m_widget.data()->removeEventFilter(this);
        m_widget.data()->setAttribute(Qt::WA_OpaquePaintEvent, m_opaquePaintEventAttribute);
        m_widget.data()->update();
        disconnect(m_widget.data(), SIGNAL(destroyed(QObject*)), this, SLOT(widgetDestroyed()));

        m_widget = QWeakPointer<QWidget>();
    }

    if (widget) {
        widget->installEventFilter(this);
        m_opaquePaintEventAttribute = widget->testAttribute(Qt::WA_OpaquePaintEvent);
        widget->setAttribute(Qt::WA_OpaquePaintEvent, true);
        widget->update();
        connect(widget, SIGNAL(destroyed(QObject*)), this, SLOT(widgetDestroyed()));

        m_widget = widget;
        setWidgetSize(widget->size());
    }
}

void WidgetProxy::widgetDestroyed()
{
    m_widget = QWeakPointer<QWidget>();
}

bool WidgetProxy::forceAspectRatio() const
{
    return m_forceAspectRatio;
}

void WidgetProxy::setForceAspectRatio(bool force)
{
    GST_DEBUG_OBJECT(m_sink, "Setting \"force-aspect-ratio\" property to %s", force ? "true" : "false");

    m_forceAspectRatio = force;
}

bool WidgetProxy::isActive() const
{
    QMutexLocker lock(&m_isActiveMutex);
    return m_isActive;
}

void WidgetProxy::setActive(bool active)
{
    GST_INFO_OBJECT(m_sink, active ? "Activating" : "Deactivating");

    QMutexLocker lock(&m_isActiveMutex);
    m_isActive = active;
    if (!active) {
        QCoreApplication::postEvent(this, new DeactivateEvent());
    }
}

QSize WidgetProxy::widgetSize() const
{
    QMutexLocker lock(&m_widgetSizeMutex);
    return m_widgetSize;
}

void WidgetProxy::setWidgetSize(const QSize & size)
{
    GST_DEBUG_OBJECT(m_sink, "Widget size changed to (%d, %d)", size.width(), size.height());

    QMutexLocker lock(&m_widgetSizeMutex);
    m_widgetSize = size;
}

bool WidgetProxy::event(QEvent *event)
{
    switch(event->type()) {
    case BufferEventType:
    {
        BufferEvent *bufEvent = dynamic_cast<BufferEvent*>(event);
        Q_ASSERT(bufEvent);

        GST_LOG_OBJECT(m_sink, "Received buffer %"GST_PTR_FORMAT, bufEvent->buffer);

        if (m_buffer) {
            //free the previous buffer
            gst_buffer_unref(m_buffer);
            m_buffer = NULL;
        }

        if (m_widget && isActive()) {
            //schedule this frame for rendering
            m_buffer = bufEvent->buffer;
            m_widget.data()->update();
        } else {
            //no widget, drop the frame
            gst_buffer_unref(bufEvent->buffer);
        }

        return true;
    }
    case DeactivateEventType:
    {
        GST_LOG_OBJECT(m_sink, "Received deactivate event");

        if (m_buffer) {
            gst_buffer_unref(m_buffer);
            m_buffer = NULL;
        }

        if (m_widget) {
            m_widget.data()->update();
        }

        return true;
    }
    default:
        return QObject::event(event);
    }
}

bool WidgetProxy::eventFilter(QObject *filteredObject, QEvent *event)
{
    if (filteredObject == m_widget.data()) {
        switch(event->type()) {
        case QEvent::Paint:
        {
            QPainter painter(m_widget.data());
            if (m_buffer && isActive()) {
                GstCaps *caps = GST_BUFFER_CAPS(m_buffer);
                GstStructure *structure = gst_caps_get_structure(caps, 0);

                GstVideoRectangle srcRect;
                srcRect.x = srcRect.y = 0;
                gst_structure_get_int(structure, "width", &srcRect.w);
                gst_structure_get_int(structure, "height", &srcRect.h);

                QRect drawRect;
                if (m_forceAspectRatio) {
                    GstVideoRectangle destRect;
                    destRect.x = destRect.y = 0;
                    destRect.w = m_widget.data()->width();
                    destRect.h = m_widget.data()->height();

                    GstVideoRectangle resultRect;
                    gst_video_sink_center_rect(srcRect, destRect, &resultRect, TRUE);
                    drawRect = QRect(resultRect.x, resultRect.y, resultRect.w, resultRect.h);

                    //we are probably not going to paint all the widget,
                    //so fill the remaining space with black
                    painter.fillRect(m_widget.data()->rect(), Qt::black);
                } else {
                    drawRect = m_widget.data()->rect();
                }

                GST_LOG_OBJECT(m_sink, "Rendering buffer %"GST_PTR_FORMAT". "
                                       "Frame size is (%d, %d), "
                                       "widget size is (%d, %d), "
                                       "calculated draw area is (%d, %d)",
                                       m_buffer,
                                       srcRect.w, srcRect.h,
                                       m_widget.data()->width(), m_widget.data()->height(),
                                       drawRect.width(), drawRect.height());

                QImage image(m_buffer->data, srcRect.w, srcRect.h, QImage::Format_RGB32);
                painter.drawImage(drawRect, image);
            } else {
                GST_LOG_OBJECT(m_sink, "Filling widget with black");
                painter.fillRect(m_widget.data()->rect(), Qt::black);
            }
            return true;
        }
        case QEvent::Resize:
        {
            QResizeEvent *resizeEvent = dynamic_cast<QResizeEvent*>(event);
            setWidgetSize(resizeEvent->size());
            return false;
        }
        default:
            return false;
        }
    } else {
        return QObject::eventFilter(filteredObject, event);
    }
}

//END ******** WidgetProxy ********
//BEGIN ******** GstQWidgetVideoSink pad template & boilerplate ********

static GstStaticPadTemplate pad_template =
    GST_STATIC_PAD_TEMPLATE("sink", GST_PAD_SINK, GST_PAD_ALWAYS,
                            GST_STATIC_CAPS (GST_VIDEO_CAPS_xRGB_HOST_ENDIAN));

GST_BOILERPLATE(GstQWidgetVideoSink, gst_qwidget_video_sink, GstVideoSink, GST_TYPE_VIDEO_SINK);

//END ******** GstQWidgetVideoSink pad template & boilerplate ********
//BEGIN ******** GstQWidgetVideoSinkClass initialization methods ********

static void gst_qwidget_video_sink_base_init(gpointer gclass)
{
    GstElementClass *element_class = GST_ELEMENT_CLASS(gclass);

    gst_element_class_set_details_simple(element_class, "QWidgetVideoSink", "Sink/Video",
                                         qwidgetvideosink_description,
                                         "George Kiagiadakis <kiagiadakis.george@gmail.com>");

    gst_element_class_add_pad_template(element_class, gst_static_pad_template_get(&pad_template));
}

/* initialize the class vtable */
static void gst_qwidget_video_sink_class_init(GstQWidgetVideoSinkClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS(klass);
    gobject_class->finalize = gst_qwidget_video_sink_finalize;
    gobject_class->set_property = gst_qwidget_video_sink_set_property;
    gobject_class->get_property = gst_qwidget_video_sink_get_property;

    GstElementClass *element_class = GST_ELEMENT_CLASS(klass);
    element_class->change_state = gst_qwidget_video_sink_change_state;

    GstBaseSinkClass *basesink_class = GST_BASE_SINK_CLASS(klass);
    basesink_class->buffer_alloc = gst_qwidget_video_sink_buffer_alloc;

    GstVideoSinkClass *videosink_class = GST_VIDEO_SINK_CLASS(klass);
    videosink_class->show_frame = gst_qwidget_video_sink_show_frame;

    /**
     * GstQWidgetVideoSink::widget
     *
     * This property holds a pointer to the QWidget on which the sink will paint the video.
     * You can set this property at any time, even if the element is in PLAYING
     * state. You can also set this property to NULL at any time to release
     * the widget. In this case, qwidgetvideosink will behave like a fakesink,
     * i.e. it will silently drop all the frames that it receives. It is also safe
     * to delete the widget that has been set as this property; the sink will be
     * signaled and this property will automatically be set to NULL.
     **/
    g_object_class_install_property(gobject_class, PROP_WIDGET,
        g_param_spec_pointer("widget", "Widget",
                             "The widget on which this element will paint the video",
                             static_cast<GParamFlags>(G_PARAM_READWRITE)));

    /**
     * GstQWidgetVideoSink::force-aspect-ratio
     *
     * If set to TRUE, the sink will scale the video respecting its original aspect ratio
     * and any remaining space will be filled with black.
     * If set to FALSE, the sink will scale the video to fit the whole widget.
     **/
    g_object_class_install_property(gobject_class, PROP_FORCE_ASPECT_RATIO,
        g_param_spec_boolean("force-aspect-ratio", "Force aspect ratio",
                             "When enabled, scaling will respect original aspect ratio",
                             FALSE, static_cast<GParamFlags>(G_PARAM_READWRITE)));
}

//END ******** GstQWidgetVideoSinkClass initialization methods ********
//BEGIN ******** GstQWidgetVideoSink instance virtual methods ********

/* element constructor */
static void gst_qwidget_video_sink_init(GstQWidgetVideoSink *sink, GstQWidgetVideoSinkClass*)
{
    GST_INFO_OBJECT(sink, "Initializing qwidgetvideosink");
    sink->proxy = new WidgetProxy(G_OBJECT(sink));
}

/* element destructor */
static void gst_qwidget_video_sink_finalize(GObject *object)
{
    GstQWidgetVideoSink *sink = GST_QWIDGETVIDEOSINK(object);

    delete sink->proxy;
    sink->proxy = NULL;
    GST_INFO_OBJECT(sink, "Finalized qwidgetvideosink");

    G_OBJECT_CLASS(parent_class)->finalize(object);
}

static void gst_qwidget_video_sink_set_property(GObject *object, guint prop_id,
                                                const GValue *value, GParamSpec *pspec)
{
    GstQWidgetVideoSink *sink = GST_QWIDGETVIDEOSINK(object);

    switch (prop_id) {
    case PROP_WIDGET:
        sink->proxy->setWidget(static_cast<QWidget*>(g_value_get_pointer(value)));
        break;
    case PROP_FORCE_ASPECT_RATIO:
        sink->proxy->setForceAspectRatio(g_value_get_boolean(value));
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static void gst_qwidget_video_sink_get_property(GObject *object, guint prop_id,
                                                GValue *value, GParamSpec *pspec)
{
    GstQWidgetVideoSink *sink = GST_QWIDGETVIDEOSINK(object);

    switch (prop_id) {
    case PROP_WIDGET:
        g_value_set_pointer(value, sink->proxy->widget());
        break;
    case PROP_FORCE_ASPECT_RATIO:
        g_value_set_boolean(value, sink->proxy->forceAspectRatio());
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static GstStateChangeReturn gst_qwidget_video_sink_change_state(GstElement *element,
                                                                GstStateChange transition)
{
    GstQWidgetVideoSink *sink = GST_QWIDGETVIDEOSINK(element);

    switch(transition) {
    case GST_STATE_CHANGE_READY_TO_PAUSED:
        sink->proxy->setActive(true);
        break;
    case GST_STATE_CHANGE_PAUSED_TO_READY:
        sink->proxy->setActive(false);
        break;
    default:
        break;
    }

    return GST_ELEMENT_CLASS(parent_class)->change_state(element, transition);
}

static GstFlowReturn gst_qwidget_video_sink_buffer_alloc(GstBaseSink *base_sink, guint64 offset,
                                                         guint size, GstCaps *caps, GstBuffer **buf)
{
    GstQWidgetVideoSink *sink = GST_QWIDGETVIDEOSINK(base_sink);

    bool unrefCaps = false;
    GstStructure *structure = gst_caps_get_structure(caps, 0);

    GstVideoRectangle srcRect;
    srcRect.x = srcRect.y = 0;
    if (!gst_structure_get_int(structure, "width", &srcRect.w) ||
        !gst_structure_get_int(structure, "height", &srcRect.h))
    {
        return GST_FLOW_NOT_NEGOTIATED;
    }

    GstVideoRectangle destRect;
    destRect.x = destRect.y = 0;
    QSize widgetSize = sink->proxy->widgetSize();
    destRect.w = widgetSize.width();
    destRect.h = widgetSize.height();

    GstVideoRectangle resultRect;
    if (sink->proxy->forceAspectRatio()) {
        gst_video_sink_center_rect(srcRect, destRect, &resultRect, TRUE);
    } else {
        resultRect = destRect;
    }

    if (resultRect.w != srcRect.w || resultRect.h != srcRect.h) {
        GstCaps *desiredCaps = gst_caps_copy(caps);
        GstStructure *structure = gst_caps_get_structure(desiredCaps, 0);
        gst_structure_set(structure, "width", G_TYPE_INT, resultRect.w,
                                     "height", G_TYPE_INT, resultRect.h, NULL);

        if (gst_pad_peer_accept_caps(GST_VIDEO_SINK_PAD(sink), desiredCaps)) {
            caps = desiredCaps;
            unrefCaps = true;
            size = resultRect.w * resultRect.h * 4;
        } else {
            gst_caps_unref(desiredCaps);
        }
    }

    *buf = gst_buffer_new_and_alloc(size);
    gst_buffer_set_caps(*buf, caps);
    GST_BUFFER_OFFSET(*buf) = offset;

    if (unrefCaps) {
        gst_caps_unref(caps);
    }

    GST_LOG_OBJECT(sink, "Requested buffer was (%d, %d), allocated was (%d, %d)",
                          srcRect.w, srcRect.h, resultRect.w, resultRect.h);

    return GST_FLOW_OK;
}

static GstFlowReturn gst_qwidget_video_sink_show_frame(GstVideoSink *video_sink, GstBuffer *buf)
{
    GstQWidgetVideoSink *sink = GST_QWIDGETVIDEOSINK(video_sink);
    GST_LOG_OBJECT(sink, "Posting new buffer (%"GST_PTR_FORMAT") for rendering", buf);
    QCoreApplication::postEvent(sink->proxy, new BufferEvent(buf));
    return GST_FLOW_OK;
}

//END ******** GstQWidgetVideoSink instance virtual methods ********
//BEGIN ******** plugin interface ********

/* entry point to initialize the plug-in */
static gboolean plugin_init(GstPlugin *plugin)
{
    GST_DEBUG_CATEGORY_INIT(gst_qwidget_video_sink_debug, "qwidgetvideosink",
                            0, qwidgetvideosink_description);

    return gst_element_register(plugin, "qwidgetvideosink",
                                GST_RANK_NONE, GST_TYPE_QWIDGETVIDEOSINK);
}

GST_PLUGIN_DEFINE (
    GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    "qwidgetvideosink",
    qwidgetvideosink_description,
    plugin_init,
    PACKAGE_VERSION,
    "LGPL",
    PACKAGE_NAME,
    PACKAGE_ORIGIN
)

//END ******** plugin interface ********

#include "gstqwidgetvideosink.moc"
