/*
    Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies). <qt-info@nokia.com>
    Copyright (C) 2011 Collabora Ltd. <info@collabora.com>

    This library is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License version 2.1
    as published by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
 * SECTION:element-qtvideosink
 *
 * qtvideosink is a video sink element that can draw directly on a Qt
 * surface, such as QWidget or QGraphicsItem, using either the QPainter API
 * or the OpenGL/OpenGLES API.
 */

#include "gstqtvideosink.h"
#include "gstqtvideosinksurface.h"
#include "gstqtvideosinkmarshal.h"

#include <QtCore/QCoreApplication>

//------------------------------

inline bool qRealIsDouble() { return sizeof(qreal) == sizeof(double); }
#define G_TYPE_QREAL qRealIsDouble() ? G_TYPE_DOUBLE : G_TYPE_FLOAT

//------------------------------

guint GstQtVideoSink::s_signals[];
GstVideoSinkClass *GstQtVideoSink::s_parent_class = NULL;

GType GstQtVideoSink::get_type()
{
  /* The typedef for GType may be gulong or gsize, depending on the
   * system and whether the compiler is c++ or not. The g_once_init_*
   * functions always take a gsize * though ... */
    static volatile gsize gonce_data = 0;
    if (g_once_init_enter(&gonce_data)) {
        GType type;
        type = gst_type_register_static_full(
            GST_TYPE_VIDEO_SINK,
            g_intern_static_string("GstQtVideoSink"),
            sizeof(GstQtVideoSinkClass),
            &GstQtVideoSink::base_init,
            NULL,   /* base_finalize */
            &GstQtVideoSink::class_init,
            NULL,   /* class_finalize */
            NULL,   /* class_data */
            sizeof(GstQtVideoSink),
            0,      /* n_preallocs */
            &GstQtVideoSink::init,
            NULL,
            (GTypeFlags) 0);
        g_once_init_leave(&gonce_data, (gsize) type);
    }
    return (GType) gonce_data;
}

void GstQtVideoSink::base_init(gpointer g_class)
{
    GstElementClass *element_class = GST_ELEMENT_CLASS(g_class);

    static GstStaticPadTemplate sink_pad_template =
        GST_STATIC_PAD_TEMPLATE("sink", GST_PAD_SINK, GST_PAD_ALWAYS,
            GST_STATIC_CAPS(
                "video/x-raw-rgb, "
                "framerate = (fraction) [ 0, MAX ], "
                "width = (int) [ 1, MAX ], "
                "height = (int) [ 1, MAX ]"
                "; "
                "video/x-raw-yuv, "
                "framerate = (fraction) [ 0, MAX ], "
                "width = (int) [ 1, MAX ], "
                "height = (int) [ 1, MAX ]"
                "; "
            )
        );

    gst_element_class_set_details_simple(element_class, "Qt video sink", "Sink/Video",
        "A video sink that can draw on any Qt surface (QPaintDevice/QWidget/QGraphicsItem/QML)",
        "George Kiagiadakis <george.kiagiadakis@collabora.com>");

    gst_element_class_add_pad_template(
            element_class, gst_static_pad_template_get(&sink_pad_template));
}

void GstQtVideoSink::class_init(gpointer g_class, gpointer class_data)
{
    Q_UNUSED(class_data);

    s_parent_class = reinterpret_cast<GstVideoSinkClass*>(g_type_class_peek_parent(g_class));

    GObjectClass *object_class = G_OBJECT_CLASS(g_class);
    object_class->finalize = GstQtVideoSink::finalize;
    object_class->set_property = GstQtVideoSink::set_property;
    object_class->get_property = GstQtVideoSink::get_property;

    GstElementClass *element_class = GST_ELEMENT_CLASS(g_class);
    element_class->change_state = GstQtVideoSink::change_state;

    GstBaseSinkClass *base_sink_class = GST_BASE_SINK_CLASS(g_class);
    base_sink_class->get_caps = GstQtVideoSink::get_caps;
    base_sink_class->set_caps = GstQtVideoSink::set_caps;
    base_sink_class->buffer_alloc = GstQtVideoSink::buffer_alloc;

    GstVideoSinkClass *video_sink_class = GST_VIDEO_SINK_CLASS(g_class);
    video_sink_class->show_frame = GstQtVideoSink::show_frame;

    GstQtVideoSinkClass *qt_video_sink_class = GST_QT_VIDEO_SINK_CLASS(g_class);
    qt_video_sink_class->paint = GstQtVideoSink::paint;


    /**
     * GstQtVideoSink::force-aspect-ratio
     *
     * If set to TRUE, the sink will scale the video respecting its original aspect ratio
     * and any remaining space will be filled with black.
     * If set to FALSE, the sink will scale the video to fit the whole drawing area.
     **/
    g_object_class_install_property(object_class, PROP_FORCE_ASPECT_RATIO,
        g_param_spec_boolean("force-aspect-ratio", "Force aspect ratio",
                             "When enabled, scaling will respect original aspect ratio",
                             FALSE, static_cast<GParamFlags>(G_PARAM_READWRITE)));

#ifndef GST_QT_VIDEO_SINK_NO_OPENGL
    /**
     * GstQtVideoSink::glcontext
     *
     * This property holds a pointer to the QGLContext that will be used to render
     * the video using OpenGL acceleration. You must set this to a valid QGLContext
     * pointer before linking this element, or else OpenGL acceleration will be disabled.
     **/
    g_object_class_install_property(object_class, PROP_GLCONTEXT,
        g_param_spec_pointer("glcontext", "GL context",
                             "The QGLContext that will be used to do OpenGL-accelerated rendering",
                             static_cast<GParamFlags>(G_PARAM_READWRITE)));
#endif

    /**
     * GstQtVideoSink::paint
     * @painter: A valid QPainter pointer that will be used to paint the video
     * @x: The x coordinate of the target area rectangle
     * @y: The y coordinate of the target area rectangle
     * @width: The width of the target area rectangle
     * @height: The height of the target area rectangle
     *
     * This is an action signal that you can call from your Qt surface class inside
     * its paint function to render the video. It takes a QPainter* and the target
     * area rectangle as arguments. You should schedule to call this function to
     * repaint the surface whenever the ::update signal is emited.
     */
    s_signals[PAINT_SIGNAL] =
        g_signal_new("paint", G_TYPE_FROM_CLASS(g_class),
                     static_cast<GSignalFlags>(G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION),
                     G_STRUCT_OFFSET(GstQtVideoSinkClass, paint),
                     NULL, NULL,
                     qRealIsDouble() ?
                        g_cclosure_user_marshal_VOID__POINTER_DOUBLE_DOUBLE_DOUBLE_DOUBLE :
                        g_cclosure_user_marshal_VOID__POINTER_FLOAT_FLOAT_FLOAT_FLOAT,
                     G_TYPE_NONE, 5,
                     G_TYPE_POINTER, G_TYPE_QREAL, G_TYPE_QREAL, G_TYPE_QREAL, G_TYPE_QREAL);

    /**
     * GstQtVideoSink::update
     *
     * This signal is emited when the surface should be repainted. It should
     * be connected to QWidget::update() or QGraphicsItem::update() or any
     * other similar function in your surface.
     */
    s_signals[UPDATE_SIGNAL] =
        g_signal_new("update", G_TYPE_FROM_CLASS(g_class),
                     G_SIGNAL_RUN_LAST,
                     0, NULL, NULL,
                     g_cclosure_marshal_VOID__VOID,
                     G_TYPE_NONE, 0);
}

void GstQtVideoSink::init(GTypeInstance *instance, gpointer g_class)
{
    GstQtVideoSink *sink = GST_QT_VIDEO_SINK(instance);
    Q_UNUSED(g_class);

    sink->surface = new GstQtVideoSinkSurface(sink);
    sink->formatDirty = true;
}

void GstQtVideoSink::finalize(GObject *object)
{
    GstQtVideoSink *sink = GST_QT_VIDEO_SINK(object);

    delete sink->surface;
    sink->surface = 0;
}

void GstQtVideoSink::set_property(GObject *object, guint prop_id,
                                  const GValue *value, GParamSpec *pspec)
{
    GstQtVideoSink *sink = GST_QT_VIDEO_SINK(object);

    switch (prop_id) {
    case PROP_FORCE_ASPECT_RATIO:
        sink->surface->setForceAspectRatio(g_value_get_boolean(value));
        break;
#ifndef GST_QT_VIDEO_SINK_NO_OPENGL
    case PROP_GLCONTEXT:
        sink->surface->setGLContext(static_cast<QGLContext*>(g_value_get_pointer(value)));
        break;
#endif
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

void GstQtVideoSink::get_property(GObject *object, guint prop_id,
                                  GValue *value, GParamSpec *pspec)
{
    GstQtVideoSink *sink = GST_QT_VIDEO_SINK(object);

    switch (prop_id) {
    case PROP_FORCE_ASPECT_RATIO:
        g_value_set_boolean(value, sink->surface->forceAspectRatio());
        break;
#ifndef GST_QT_VIDEO_SINK_NO_OPENGL
    case PROP_GLCONTEXT:
        g_value_set_pointer(value, sink->surface->glContext());
        break;
#endif
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

GstStateChangeReturn GstQtVideoSink::change_state(GstElement *element, GstStateChange transition)
{
    GstQtVideoSink *sink = GST_QT_VIDEO_SINK(element);

    switch (transition) {
    case GST_STATE_CHANGE_READY_TO_PAUSED:
        sink->surface->setActive(true);
        break;
    case GST_STATE_CHANGE_PAUSED_TO_READY:
        sink->surface->setActive(false);
        break;
    default:
        break;
    }

    return GST_ELEMENT_CLASS(s_parent_class)->change_state(element, transition);
}

GstCaps *GstQtVideoSink::get_caps(GstBaseSink *base)
{
    GstQtVideoSink *sink = GST_QT_VIDEO_SINK(base);
    GstCaps *caps = gst_caps_new_empty();

    Q_FOREACH(BufferFormat::PixelFormat format, sink->surface->supportedPixelFormats()) {
        gst_caps_append(caps, BufferFormat::pixelFormatToCaps(format));
    }

    return caps;
}

gboolean GstQtVideoSink::set_caps(GstBaseSink *base, GstCaps *caps)
{
    GstQtVideoSink *sink = GST_QT_VIDEO_SINK(base);

    GST_LOG_OBJECT(sink, "new caps %" GST_PTR_FORMAT, caps);
    sink->formatDirty = true;
    return TRUE;
}

GstFlowReturn GstQtVideoSink::buffer_alloc(GstBaseSink *base, guint64 offset, guint size,
                                           GstCaps *caps, GstBuffer **buffer)
{
    Q_UNUSED(base);
    Q_UNUSED(offset);
    Q_UNUSED(size);
    Q_UNUSED(caps);

    *buffer = 0;

    return GST_FLOW_OK;
}

GstFlowReturn GstQtVideoSink::show_frame(GstVideoSink *video_sink, GstBuffer *buffer)
{
    GstQtVideoSink *sink = GST_QT_VIDEO_SINK(video_sink);

    GST_TRACE_OBJECT(sink, "Posting new buffer (%"GST_PTR_FORMAT") for rendering. "
                           "Format dirty: %d", buffer, (int)sink->formatDirty);

    QCoreApplication::postEvent(sink->surface,
            new GstQtVideoSinkSurface::BufferEvent(buffer, sink->formatDirty));

    sink->formatDirty = false;
    return GST_FLOW_OK;
}

void GstQtVideoSink::paint(GstQtVideoSink *sink, gpointer painter,
                           qreal x, qreal y, qreal width, qreal height)
{
    sink->surface->paint(static_cast<QPainter*>(painter), x, y, width, height);
}
