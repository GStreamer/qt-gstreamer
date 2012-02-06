/*
    Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies). <qt-info@nokia.com>
    Copyright (C) 2011-2012 Collabora Ltd. <info@collabora.com>

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

#include "gstqtglvideosinkbase.h"
#include "openglsurfacepainter.h"
#include "qtvideosinkdelegate.h"

DEFINE_TYPE_FULL(GstQtGLVideoSinkBase, GST_TYPE_QT_VIDEO_SINK_BASE, init_interfaces)

//------------------------------

void GstQtGLVideoSinkBase::base_init(gpointer g_class)
{
    GstElementClass *element_class = GST_ELEMENT_CLASS(g_class);
    element_class->padtemplates = NULL; //get rid of the pad template of the base class

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

    gst_element_class_add_pad_template(
            element_class, gst_static_pad_template_get(&sink_pad_template));
}

void GstQtGLVideoSinkBase::class_init(gpointer g_class, gpointer class_data)
{
    Q_UNUSED(class_data);

    GObjectClass *object_class = G_OBJECT_CLASS(g_class);
    object_class->set_property = GstQtGLVideoSinkBase::set_property;
    object_class->get_property = GstQtGLVideoSinkBase::get_property;

    GstBaseSinkClass *base_sink_class = GST_BASE_SINK_CLASS(g_class);
    base_sink_class->start = GstQtGLVideoSinkBase::start;
    base_sink_class->get_caps = GstQtGLVideoSinkBase::get_caps;

    //TODO colorbalance properties
}

void GstQtGLVideoSinkBase::init(GTypeInstance *instance, gpointer g_class)
{
    Q_UNUSED(instance);
    Q_UNUSED(g_class);
}

void GstQtGLVideoSinkBase::init_interfaces(GType type)
{
    //TODO colorbalance interface
}

//------------------------------

void GstQtGLVideoSinkBase::set_property(GObject *object, guint prop_id,
                                        const GValue *value, GParamSpec *pspec)
{
    GstQtVideoSinkBase *sink = GST_QT_VIDEO_SINK_BASE(object);

    switch (prop_id) {
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

void GstQtGLVideoSinkBase::get_property(GObject *object, guint prop_id,
                                        GValue *value, GParamSpec *pspec)
{
    GstQtVideoSinkBase *sink = GST_QT_VIDEO_SINK_BASE(object);

    switch (prop_id) {
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

//------------------------------

gboolean GstQtGLVideoSinkBase::start(GstBaseSink *base)
{
    GstQtVideoSinkBase *sink = GST_QT_VIDEO_SINK_BASE(base);

    //fail on purpose if the user hasn't set a context
    if (sink->delegate->supportedPainterTypes() == QtVideoSinkDelegate::Generic) {
        GST_WARNING_OBJECT(sink, "Neither GLSL nor ARB Fragment Program are supported "
                                 "for painting. Did you forget to set a gl context?");
        return FALSE;
    } else {
        return TRUE;
    }
}

GstCaps *GstQtGLVideoSinkBase::get_caps(GstBaseSink *base)
{
    Q_UNUSED(base);

    GstCaps *caps = gst_caps_new_empty();

    Q_FOREACH(GstVideoFormat format, OpenGLSurfacePainter::supportedPixelFormats()) {
        gst_caps_append(caps, BufferFormat::newTemplateCaps(format));
    }

    return caps;
}
