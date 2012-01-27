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

#ifndef GST_QT_GL_VIDEO_SINK_BASE_H
#define GST_QT_GL_VIDEO_SINK_BASE_H

#include "gstqtvideosinkbase.h"

#ifndef GST_QT_VIDEO_SINK_NO_OPENGL

#define GST_TYPE_QT_GL_VIDEO_SINK_BASE \
  (GstQtGLVideoSinkBase::get_type())

struct GstQtGLVideoSinkBase
{
public:
    GstQtVideoSinkBase parent;

    static GType get_type();

private:
    enum {
        PROP_0,
    };

    static void base_init(gpointer g_class);
    static void class_init(gpointer g_class, gpointer class_data);

    static void init(GTypeInstance *instance, gpointer g_class);
    static void init_interfaces(GType type);

    static void set_property(GObject *object, guint prop_id,
                             const GValue *value, GParamSpec *pspec);
    static void get_property(GObject *object, guint prop_id,
                             GValue *value, GParamSpec *pspec);

    static gboolean start(GstBaseSink *sink);
    static GstCaps *get_caps(GstBaseSink *sink);
};


struct GstQtGLVideoSinkBaseClass
{
    GstQtVideoSinkBaseClass parent_class;
};

#endif // GST_QT_VIDEO_SINK_NO_OPENGL
#endif // GST_QT_GL_VIDEO_SINK_BASE_H
