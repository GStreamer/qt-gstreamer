/*
    Copyright (C) 2010 George Kiagiadakis <kiagiadakis.george@gmail.com>
    Copyright (C) 2012 Collabora Ltd. <info@collabora.com>

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

#include "gstqwidgetvideosink.h"
#include "gstqtvideosinksurface.h"
#include <QtCore/QWeakPointer>
#include <QtCore/QEvent>
#include <QtGui/QWidget>
#include <QtGui/QPainter>

//BEGIN ******** WidgetProxy ********

class WidgetProxy : public QObject
{
    Q_OBJECT
public:
    WidgetProxy(GstQWidgetVideoSink *sink);
    virtual ~WidgetProxy();

    // "widget" property
    QWidget *widget() const;
    void setWidget(QWidget *widget);

private Q_SLOTS:
    void widgetDestroyed();

protected:
    virtual bool eventFilter(QObject *filteredObject, QEvent *event);

private:
    GstQWidgetVideoSink *m_sink;

    // "widget" property
    QWeakPointer<QWidget> m_widget;

    // original value of the Qt::WA_OpaquePaintEvent attribute
    bool m_opaquePaintEventAttribute;
};

WidgetProxy::WidgetProxy(GstQWidgetVideoSink *sink)
    : QObject(),
      m_sink(sink)
{
}

WidgetProxy::~WidgetProxy()
{
    setWidget(NULL);
}

QWidget *WidgetProxy::widget() const
{
    return m_widget.data();
}

void WidgetProxy::setWidget(QWidget *widget)
{
    GST_LOG_OBJECT(m_sink, "Setting \"widget\" property to %"GST_PTR_FORMAT, widget);

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
    }
}

void WidgetProxy::widgetDestroyed()
{
    m_widget = QWeakPointer<QWidget>();
}

bool WidgetProxy::eventFilter(QObject *filteredObject, QEvent *event)
{
    if (filteredObject == m_widget.data()) {
        switch(event->type()) {
        case QEvent::Paint:
          {
            QPainter painter(m_widget.data());
            QRect rect = m_widget.data()->rect();
            GST_QT_VIDEO_SINK_BASE(m_sink)->surface->paint(&painter,
                    rect.x(), rect.y(), rect.width(), rect.height());
            return true;
          }
        default:
            return false;
        }
    } else {
        return QObject::eventFilter(filteredObject, event);
    }
}

//END ******** WidgetProxy ********
//BEGIN ******** GstQWidgetVideoSink ********

GstQtVideoSinkBaseClass *GstQWidgetVideoSink::s_parent_class = NULL;

GType GstQWidgetVideoSink::get_type()
{
  /* The typedef for GType may be gulong or gsize, depending on the
   * system and whether the compiler is c++ or not. The g_once_init_*
   * functions always take a gsize * though ... */
    static volatile gsize gonce_data = 0;
    if (g_once_init_enter(&gonce_data)) {
        GType type;
        type = gst_type_register_static_full(
            GST_TYPE_QT_VIDEO_SINK_BASE,
            g_intern_static_string("GstQWidgetVideoSink"),
            sizeof(GstQWidgetVideoSinkClass),
            &GstQWidgetVideoSink::base_init,
            NULL,   /* base_finalize */
            &GstQWidgetVideoSink::class_init,
            NULL,   /* class_finalize */
            NULL,   /* class_data */
            sizeof(GstQWidgetVideoSink),
            0,      /* n_preallocs */
            &GstQWidgetVideoSink::init,
            NULL,
            (GTypeFlags) 0);
        g_once_init_leave(&gonce_data, (gsize) type);
    }
    return (GType) gonce_data;
}

void GstQWidgetVideoSink::base_init(gpointer gclass)
{
    GstElementClass *element_class = GST_ELEMENT_CLASS(gclass);

    gst_element_class_set_details_simple(element_class, "QWidget video sink", "Sink/Video",
        "A video sink that draws on a QWidget using QPainter",
        "George Kiagiadakis <george.kiagiadakis@collabora.com>");
}

void GstQWidgetVideoSink::class_init(gpointer g_class, gpointer class_data)
{
    Q_UNUSED(class_data);

    s_parent_class = GST_QT_VIDEO_SINK_BASE_CLASS(g_type_class_peek_parent(g_class));

    GObjectClass *gobject_class = G_OBJECT_CLASS(g_class);
    gobject_class->finalize = GstQWidgetVideoSink::finalize;
    gobject_class->set_property = GstQWidgetVideoSink::set_property;
    gobject_class->get_property = GstQWidgetVideoSink::get_property;

    GstQtVideoSinkBaseClass *qt_video_sink_base_class = GST_QT_VIDEO_SINK_BASE_CLASS(g_class);
    qt_video_sink_base_class->update = GstQWidgetVideoSink::update;

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
}

void GstQWidgetVideoSink::init(GTypeInstance *instance, gpointer g_class)
{
    Q_UNUSED(g_class);

    GstQWidgetVideoSink *sink = GST_QWIDGET_VIDEO_SINK(instance);
    sink->proxy = new WidgetProxy(sink);
}

void GstQWidgetVideoSink::finalize(GObject *object)
{
    GstQWidgetVideoSink *sink = GST_QWIDGET_VIDEO_SINK(object);

    delete sink->proxy;
    sink->proxy = NULL;

    G_OBJECT_CLASS(s_parent_class)->finalize(object);
}

void GstQWidgetVideoSink::set_property(GObject *object, guint prop_id,
                                       const GValue *value, GParamSpec *pspec)
{
    GstQWidgetVideoSink *sink = GST_QWIDGET_VIDEO_SINK(object);

    switch (prop_id) {
    case PROP_WIDGET:
        sink->proxy->setWidget(static_cast<QWidget*>(g_value_get_pointer(value)));
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

void GstQWidgetVideoSink::get_property(GObject *object, guint prop_id,
                                       GValue *value, GParamSpec *pspec)
{
    GstQWidgetVideoSink *sink = GST_QWIDGET_VIDEO_SINK(object);

    switch (prop_id) {
    case PROP_WIDGET:
        g_value_set_pointer(value, sink->proxy->widget());
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

void GstQWidgetVideoSink::update(GstQtVideoSinkBase *sink)
{
    QWidget *w = GST_QWIDGET_VIDEO_SINK(sink)->proxy->widget();
    if (w) {
        w->update();
    }
}

//END ******** GstQWidgetVideoSink ********

#include "gstqwidgetvideosink.moc"
